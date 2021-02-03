/* jack.c: interface to jack */
#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>
#include <string.h>

#include "util.h"
#include "jack.h"

enum MidiStatus {
  NOTE_OFF    = 0x80,
  NOTE_ON     = 0x90,
  CTRL_CHANGE = 0xB0,
  PROG_CHANGE = 0xC0
};

typedef struct {
  jack_nframes_t frame;
  uint8_t len;
  uint8_t data[3];
} MidiMessage;

static jack_port_t *out_port = NULL;
static jack_ringbuffer_t *buffer = NULL;
static jack_client_t *client = NULL;
static char port_outdated = 1;

static int
process(jack_nframes_t nframes, void *arg)
{
  void *port_buf;
  size_t space;
  jack_nframes_t last_frame = jack_last_frame_time(client);

  (void)arg;

  port_buf = jack_port_get_buffer(out_port, nframes);
  if (!port_buf)
    return 0; /* can't allocate buffer, end cycle */

  jack_midi_clear_buffer(port_buf);

  while ((space = jack_ringbuffer_read_space(buffer)) >= sizeof(MidiMessage)) {
    MidiMessage msg;
    size_t read;
    jack_nframes_t time;

    read = jack_ringbuffer_peek(buffer, (char*)&msg, sizeof(msg));
    if (read != sizeof(msg))
      continue;

    if (msg.frame >= last_frame)
      /* spill over to next cycle */
      break;

    if (last_frame - msg.frame > nframes) {
      /* msg is from at least 2 cycles ago
       *
       * |--+--------|-----------|-----------|
       *    msg                  last_frame
       *
       * basically an overflow-safe way to write
       *   (msg.frame + nframes) - last_frame < 0
       */
      time = 0;
    } else {
      /* note that msg.frame is delayed by exactly nframes samples
       *
       * |--+--------|-----------|
       *    msg      last_frame
       */
      time = (msg.frame + nframes) - last_frame;
    }

    jack_midi_event_write(port_buf, time, msg.data, msg.len);
    jack_ringbuffer_read_advance(buffer, read);
  }

  return 0;
}

static void
shutdown_cb(void *arg)
{
  (void)arg;
  die("jack server is down, exiting...");
}

/* auto connect to any registered midi input port */
static void
auto_connect_cb(jack_port_id_t id, int registered, void *arg)
{
  (void)id;
  (void)arg;

  if (registered) {
    /* we can't refresh port in the callback because this is called in
     * a notification thread */
    port_outdated = 1;
  }
}

void
refresh_ports(void)
{
  const char **ports;

  if (!port_outdated)
    return;
  port_outdated = 0;

  ports = jack_get_ports(client, NULL, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput);
  if (ports) {
    int i;
    for (i = 0; ports[i]; ++i) {
      if (!jack_connect(client, jack_port_name(out_port), ports[i]))
        info("connected to %s", ports[i]);
    }
    jack_free(ports);
  }
}

static int
write_midi_at(jack_nframes_t frame, const char *msg, size_t len)
{
  size_t avail_write = jack_ringbuffer_write_space(buffer);
  size_t written;
  MidiMessage payload = {
    .frame = frame,
    .len = len
  };
  memcpy(payload.data, msg, len);

  if (avail_write < sizeof(payload))
    return 0; /* no space left */

  written = jack_ringbuffer_write(buffer, (const char*)&payload, sizeof(payload));
  if (written < sizeof(payload))
    return 0; /* write failed */

  return 1;
}

static int
write_midi(const char *msg, size_t len)
{
  return write_midi_at(jack_frame_time(client), msg, len);
}

int
write_note_on(char channel, char pitch, char vel)
{
  return write_midi((char[]) {NOTE_ON | channel, pitch, vel}, 3);
}

int
write_note_off(char channel, char pitch, char vel)
{
  return write_midi((char[]) {NOTE_OFF | channel, pitch, vel}, 3);
}

int
write_control(char channel, char controller, char val)
{
  return write_midi((char[]) {CTRL_CHANGE | channel, controller, val}, 3);
}

int
write_bank_sel(char channel, uint_least16_t val)
{
  jack_nframes_t frame = jack_frame_time(client);
  int rc1, rc2;

  rc1 = write_midi_at(frame, (char[]){
    CTRL_CHANGE | channel, CTRL_BANK_SEL_MSB, (val >> 7) & 0x7f
  }, 3);

  rc2 = write_midi_at(frame, (char[]){
    CTRL_CHANGE | channel, CTRL_BANK_SEL_LSB, val & 0x7f
  }, 3);

  return rc1 && rc2;
}

int write_prog_change(char channel, char prog)
{
  return write_midi((char[]) {PROG_CHANGE | channel, prog}, 2);
}

void
jack_init(int auto_connect)
{
  client = jack_client_open("kb", JackNoStartServer, NULL);
  if (!client)
    die("can't open client");

  if (jack_set_process_callback(client, process, NULL))
    die("can't set up process callback");

  if (auto_connect &&
      jack_set_port_registration_callback(client, auto_connect_cb, NULL))
    die("can't register auto-connect callback");

  jack_on_shutdown(client, shutdown_cb, NULL);

  out_port = jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE,
      JackPortIsOutput, 0);
  if (!out_port)
    die("can't register midi output port");

  buffer = jack_ringbuffer_create(1024 * sizeof(MidiMessage));
  if (!buffer)
    die("can't create midi message buffer");
  if (jack_ringbuffer_mlock(buffer))
    die("can't lock memory");

  if (jack_activate(client))
    die("cannot activate client");

  if (auto_connect)
    refresh_ports();
  else
    port_outdated = 0; /* never update port */
}

void
jack_shutdown(void)
{
  if (client) {
    jack_deactivate(client);
    jack_client_close(client);
  }
  if (buffer) {
    jack_ringbuffer_free(buffer);
  }
}
