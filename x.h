/* x.h: interface to X window system */
#ifndef KB_X_H
#define KB_X_H

#include <X11/Xlib.h>

enum {
  /* see https://tronche.com/gui/x/xlib/input/keyboard-encoding.html */
  MAX_KEYCODE = 255,
};

/* check if event is from xinput */
int is_xi_event(Display *d, const XGenericEventCookie *cookie);

/* init xlib, return val can't be NULL */
Display * x_init(void);
/* shutdown xlib */
void x_shutdown(Display *d);

#endif
