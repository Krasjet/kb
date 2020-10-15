/* x.c: interface to X window system */
#include "util.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>

static Window root, focus;

static int
ignore(Display *d, XErrorEvent *e)
{
  (void)d;
  if (e->error_code != BadWindow) {
    die("unexpected X error");
  }
  return 0;
}

int
is_auto_repeat(Display *d, XEvent *e)
{
  XEvent next;
  if (e->type == KeyRelease && XEventsQueued(d, QueuedAfterReading)) {
    XPeekEvent(d, &next);
    return next.type == KeyPress &&
           next.xkey.time == e->xkey.time &&
           next.xkey.keycode == e->xkey.keycode;
  }
  return 0;
}

void
focus_update(Display *d)
{
  int state;

  if (focus != root)
    XSelectInput(d, focus, NoEventMask);

  /* get new focus */
  XGetInputFocus(d, &focus, &state);
  if (focus == PointerRoot)
    focus = root;
  XSelectInput(d, focus, KeyPressMask | KeyReleaseMask | FocusChangeMask);
}

Display *
x_init(void)
{
  int state;

  Display *dpy = XOpenDisplay(NULL);
  if (!dpy)
    die("can't open display");
  root = DefaultRootWindow(dpy);

  XGetInputFocus(dpy, &focus, &state);
  XSelectInput(dpy, focus, KeyPressMask | KeyReleaseMask | FocusChangeMask);

  /* ignore if window has been closed */
  XSetErrorHandler(ignore);

  return dpy;
}

void x_shutdown(Display *d)
{
  XCloseDisplay(d);
}
