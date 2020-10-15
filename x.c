/* x.c: interface to X window system */
#include "util.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput2.h>

static int xi_opcode;

int
is_xi_event(Display *d, XGenericEventCookie *cookie)
{
  return XGetEventData(d, cookie) &&
         cookie->type == GenericEvent &&
         cookie->extension == xi_opcode;
}

Display *
x_init(void)
{
  XIEventMask emsk;
  unsigned char mask[XIMaskLen(XI_LASTEVENT)] = {0};
  Window root;

  Display *dpy = XOpenDisplay(NULL);
  if (!dpy)
    die("can't open display");
  root = DefaultRootWindow(dpy);

  int event, error;
  if (!XQueryExtension(dpy, "XInputExtension", &xi_opcode, &event, &error)) {
    die("xinput not available");
  }

  /* setup xinput event listener */
  emsk.deviceid = XIAllMasterDevices;
  emsk.mask_len = XIMaskLen(XI_LASTEVENT);
  emsk.mask = mask;
  XISetMask(emsk.mask, XI_RawKeyPress);
  XISetMask(emsk.mask, XI_RawKeyRelease);
  XISelectEvents(dpy, root, &emsk, 1);
  XSync(dpy, False);

  return dpy;
}

void x_shutdown(Display *d)
{
  XCloseDisplay(d);
}
