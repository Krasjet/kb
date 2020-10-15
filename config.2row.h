/* config.2row.h: alternative two octave layout */

enum {
  MAX_OCTAVE = 8,
  MIN_OCTAVE = 0,
};

static opcode_t keybinds[MAX_KEYCODE + 1];

struct SymBind {
  KeySym key;
  opcode_t opcode;
};

static struct SymBind symbinds[] = {
 /* key              opcode */
  /* row 1 white keys */
  { XK_Z,            OP_C      },
  { XK_X,            OP_D      },
  { XK_C,            OP_E      },
  { XK_V,            OP_F      },
  { XK_B,            OP_G      },
  { XK_N,            OP_A      },
  { XK_M,            OP_B      },
  { XK_comma,        OP_C + 12 }, /* next octave */
  { XK_period,       OP_D + 12 },
  { XK_slash,        OP_E + 12 },

  /* row 1 black keys */
  { XK_S,            OP_CSHARP      },
  { XK_D,            OP_DSHARP      },
  { XK_G,            OP_FSHARP      },
  { XK_H,            OP_GSHARP      },
  { XK_J,            OP_ASHARP      },
  { XK_L,            OP_CSHARP + 12 },
  { XK_semicolon,    OP_DSHARP + 12 },

  /* row 2 white keys */
  { XK_Q,            OP_C + 12 },
  { XK_W,            OP_D + 12 },
  { XK_E,            OP_E + 12 },
  { XK_R,            OP_F + 12 },
  { XK_T,            OP_G + 12 },
  { XK_Y,            OP_A + 12 },
  { XK_U,            OP_B + 12 },
  { XK_I,            OP_C + 24 }, /* next octave */
  { XK_O,            OP_D + 24 },
  { XK_P,            OP_E + 24 },
  { XK_bracketleft,  OP_F + 24 },
  { XK_bracketright, OP_G + 24 },

  /* row 2 black keys */
  { XK_2,            OP_CSHARP + 12 },
  { XK_3,            OP_DSHARP + 12 },
  { XK_5,            OP_FSHARP + 12 },
  { XK_6,            OP_GSHARP + 12 },
  { XK_7,            OP_ASHARP + 12 },
  { XK_9,            OP_CSHARP + 24 },
  { XK_0,            OP_DSHARP + 24 },
  { XK_plus,         OP_FSHARP + 24 },

  /* function keys */
  { XK_Up,           OP_INC_OCTAVE   },
  { XK_Down,         OP_DEC_OCTAVE   },
  { XK_Right,        OP_INC_VELOCITY },
  { XK_Left,         OP_DEC_VELOCITY },
};

void
keybinds_init(Display *d)
{
  size_t i;

  for (i = 0; i < sizeof(keybinds)/sizeof(keybinds[0]); ++i) {
    keybinds[i] = OP_INVALID;
  }

  for (i = 0; i < sizeof(symbinds)/sizeof(symbinds[0]); ++i) {
    keybinds[XKeysymToKeycode(d, symbinds[i].key)] = symbinds[i].opcode;
  }
}
