/* config.min.h: the minimum requirement of config */

enum {
  MAX_OCTAVE = 10,
  MIN_OCTAVE = 0,
};

static opcode_t keybinds[MAX_KEYCODE + 1];

void
keybinds_init(Display *d) {
  size_t i;

  /* map every key to C4 */
  for (i = 0; i < sizeof(keybinds)/sizeof(keybinds[0]); ++i) {
    keybinds[i] = OP_C;
  }
}
