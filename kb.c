#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <X11/keysym.h>

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
usage(const char *name) {
	fprintf(stdout, "usage: %s [-c channel]\n", name);
}

int
main(int argc, char *argv[])
{
  Display *dpy;
  XEvent event;
  int note;
  KeyCode keycode;
  int channel = 0;
  char pressed[MAX_KEYCODE + 1] = { 0 };
  int octave = 5; /* counting from midi 0, 0 <= octave <=10 */

  int c;
	while ((c = getopt(argc, argv, "c:")) != -1) {
    switch (c) {
    case 'c':
      channel = atoi(optarg) - 1;
      if (channel < 0 || channel > 15) {
        die("error: channel must be between 1 and 16, inclusive");
      }
      break;
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

  jack_init();
  info("JACK started");

  running = 1;

  while (running) {
    XNextEvent(dpy, &event);

    switch (event.type) {
    case KeyPress:
      keycode = event.xkey.keycode;
      pressed[keycode] = 1;
      if (keybinds[keycode] >= 0) {
        /* play note */
        note = octave * 12 + keybinds[keycode];
        if (note <= MAX_MIDI_NOTE)
          write_note_on(channel, note);
      } else {
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
        default: break;
        }
      }
      break;
    case KeyRelease:
      keycode = event.xkey.keycode;
      if (is_auto_repeat(dpy, &event)) {
        /* probably a bug in X11, we will get an extra release event before press */
        if (pressed[keycode]) {
          XNextEvent(dpy, &event); /* consume auto repeat */
        }
      } else {
        /* turn off note */
        pressed[keycode] = 0;

        note = octave * 12 + keybinds[keycode];
        if (keybinds[keycode] >= 0 && note <= MAX_MIDI_NOTE) {
          write_note_off(channel, note);
        }
      }
      break;
    case FocusOut:
      focus_update(dpy);
      break;
    }
  }

  jack_shutdown();
  x_shutdown(dpy);

  return 0;
}
