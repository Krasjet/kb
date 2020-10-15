/* jack.c: interface to jack */
#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

#include "util.h"
#include "jack.h"

static jack_port_t *out_port = NULL;
static jack_ringbuffer_t *buffer = NULL;
static jack_client_t *client = NULL;

static int
process(jack_nframes_t nframes, void *arg)
{
  jack_nframes_t i = 0;
  size_t bytes_read;
  jack_midi_data_t msg[MSG_SIZE];
  void * port_buf;

  (void)arg;

  port_buf = jack_port_get_buffer(out_port, nframes);
  if (!port_buf) {
    return 0; /* can't allocate buffer, end cycle */
  }

  jack_midi_clear_buffer(port_buf);

  while (i < nframes && jack_ringbuffer_read_space(buffer) >= MSG_SIZE) {
    bytes_read = jack_ringbuffer_read(buffer, (char *) msg, MSG_SIZE);

    if (bytes_read != MSG_SIZE)
      continue; /* ignore this message */

    jack_midi_event_write(port_buf, i, msg, MSG_SIZE);

    i++;
  }

  return 0;
}

static void
shutdown_cb(void *arg)
{
  (void)arg;
  die("jack server is down, exiting...");
}

static int
write_midi(const char *msg, size_t size)
{
  size_t avail_write = jack_ringbuffer_write_space(buffer);

  if (avail_write < size)
    return 0; /* no space left */
  if (jack_ringbuffer_write(buffer, msg, size) < size)
    return 0; /* write failed */

  return 1;
}

int
write_note_on(char channel, char pitch, char vel)
{
  return write_midi((char[]) {0x90 | channel, pitch, vel}, MSG_SIZE);
}

int
write_note_off(char channel, char pitch, char vel)
{
  return write_midi((char[]) {0x80 | channel, pitch, vel}, MSG_SIZE);
}

void
jack_init(void)
{
  client = jack_client_open("kb", JackNoStartServer, NULL);
  if (!client)
    die("can't open client");

  if (jack_set_process_callback(client, process, NULL))
    die("can't set up process callback");
  jack_on_shutdown(client, shutdown_cb, NULL);

  out_port = jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE,
      JackPortIsOutput, 0);
  if (!out_port)
    die("can't register midi output port");

  buffer = jack_ringbuffer_create(1024 * MSG_SIZE);
  if (!buffer)
    die("can't create midi message buffer");
  if (jack_ringbuffer_mlock(buffer))
    die("can't lock memory");

  if (jack_activate(client))
    die("cannot activate client");
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
