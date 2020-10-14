/* config file for kb, it will be pasted to kb.c during compilation */

/*
 * The keybindings for kb.
 *
 * It is a map
 *
 *     KeyCode -> opcode_t
 *
 * You need to fill in this array in `keybinds_init`.
 */
static opcode_t keybinds[MAX_KEYCODE + 1];

/*
 * The following struct and array are defined only for easier configuration.
 * They are not used anywhere in kb.c.
 */
/* KeySym -> opcode_t */
struct SymBind {
  KeySym key;
  opcode_t opcode;
};

/* Change keybindings here */
static struct SymBind symbinds[] = {
 /* key              opcode */
  { XK_A,            OP_C            },
  { XK_S,            OP_D            },
  { XK_D,            OP_E            },
  { XK_F,            OP_F            },
  { XK_G,            OP_G            },
  { XK_H,            OP_A            },
  { XK_J,            OP_B            },
  { XK_W,            OP_CSHARP       },
  { XK_E,            OP_DSHARP       },
  { XK_T,            OP_FSHARP       },
  { XK_Y,            OP_GSHARP       },
  { XK_U,            OP_ASHARP       },
  { XK_K,            OP_C      + 12  }, /* next octave */
  { XK_L,            OP_D      + 12  },
  { XK_semicolon,    OP_E      + 12  },
  { XK_apostrophe,   OP_F      + 12  },
  { XK_O,            OP_CSHARP + 12  },
  { XK_P,            OP_DSHARP + 12  },
  { XK_bracketright, OP_FSHARP + 12  },
  { XK_X,            OP_INC_OCTAVE   },
  { XK_Z,            OP_DEC_OCTAVE   },
  { XK_V,            OP_INC_VELOCITY },
  { XK_C,            OP_DEC_VELOCITY },
};

/*
 * This function will be called in main to initialize keybindings.
 * You need to initialize the `keybinds` variable in this function.
 */
void
keybinds_init(Display *d) {
  size_t i;

  for (i = 0; i < sizeof(keybinds)/sizeof(keybinds[0]); ++i) {
    keybinds[i] = OP_INVALID;
  }

  for (i = 0; i < sizeof(symbinds)/sizeof(symbinds[0]); ++i) {
    keybinds[XKeysymToKeycode(d, symbinds[i].key)] = symbinds[i].opcode;
  }
}
