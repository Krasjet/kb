/* kb.c: main module for kb */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput2.h>

#include "jack.h"
#include "x.h"
#include "util.h"

/* load config */
#include "config.h"

static volatile int running = 0;

/* i.e. the only way to properly shutdown the daemon */
static void
sig_handler(int signum)
{
  puts(""); /* start new line */
  info("got signal %d, exiting...", signum);
  running = 0;
}

static void
usage(const char *name)
{
  fprintf(stdout, "usage: %s [-c channel]\n", name);
}

int
main(int argc, char *argv[])
{
  Display *dpy;
  int note;
  KeyCode keycode;
  int channel = 0, auto_connect = 0;
  int velocity = 64;
  int octave = 5; /* counting from midi 0 */
  XEvent event;

  int c;
  while ((c = getopt(argc, argv, "ac:h")) != -1) {
    switch (c) {
    case 'c':
      channel = atoi(optarg) - 1;
      if (channel < 0 || channel > 15)
        die("error: channel must be between 1 and 16, inclusive");
      break;
    case 'a':
      auto_connect = 1;
      break;
    case 'h':
      usage(argv[0]);
      return 0;
    default:
      usage(argv[0]);
      return 1;
    }
  }

  /* catch signal for grace shutdown */
  signal(SIGQUIT, sig_handler);
  signal(SIGTERM, sig_handler);
  signal(SIGHUP, sig_handler);
  signal(SIGINT, sig_handler);

  dpy = x_init();
  info("X started");

  keybinds_init(dpy);
  info("keybinding loaded");

  jack_init(auto_connect);
  info("JACK started");

  running = 1;

  while (running) {
    /* check and connect to any new input ports */
    refresh_ports();
    XNextEvent(dpy, &event);

    if (!is_xi_event(dpy, &event.xcookie))
      continue; /* not an xinput event, skip */

    switch (event.xcookie.evtype) {
    case XI_RawKeyPress:
      keycode = ((XIRawEvent *)event.xcookie.data)->detail;
      if (keybinds[keycode] >= 0) {
        /* play note */
        note = octave * 12 + keybinds[keycode];
        if (note <= MAX_MIDI_NOTE)
          write_note_on(channel, note, velocity);
      } else {
        int i;
        /* opeartions */
        switch (keybinds[keycode]) {
        case OP_INC_OCTAVE:
          if (octave < MAX_OCTAVE)
            octave++;
          break;
        case OP_DEC_OCTAVE:
          if (octave > MIN_OCTAVE)
            octave--;
          break;
        case OP_INC_VELOCITY:
          if (velocity + 10 <= MAX_VELOCITY)
            velocity += 10;
          else
            velocity = MAX_VELOCITY;
          break;
        case OP_DEC_VELOCITY:
          if (velocity - 10 >= MIN_VELOCITY)
            velocity -= 10;
          else
            velocity = MIN_VELOCITY;
          break;
        case OP_PANIC: /* turn off all midi notes */
          for (i = 0; i <= MAX_MIDI_NOTE; ++i) {
            write_note_off(channel, i, velocity);
          }
          write_control(channel, CTRL_ALL_NOTES_OFF, 0x00);
          write_control(channel, CTRL_ALL_SOUND_OFF, 0x00);
          write_control(channel, CTRL_RESET_CONTROLLERS, 0x00);
          break;
        default: break;
        }
      }
      break;
    case XI_RawKeyRelease:
      keycode = ((XIRawEvent *)event.xcookie.data)->detail;

      note = octave * 12 + keybinds[keycode];
      if (keybinds[keycode] >= 0 && note <= MAX_MIDI_NOTE)
        write_note_off(channel, note, velocity);
      break;
    }
    XFreeEventData(dpy, &event.xcookie);
  }

  jack_shutdown();
  x_shutdown(dpy);

  return 0;
}
