/* jack.h: interface to jack */
#ifndef KB_JACK_H
#define KB_JACK_H

#include <stdint.h>

enum {
  /* valid MIDI data byte 0-127 */
  MAX_DATA = (1u << 7u) - 1,
  MIN_DATA = 0,
  MAX_BANK = (1u << 14u) - 1,
};

enum Controls {
  CTRL_BANK_SEL_MSB = 0,
  CTRL_BANK_SEL_LSB = 32,
  CTRL_ALL_SOUND_OFF = 120,
  CTRL_RESET_CONTROLLERS = 121,
  CTRL_ALL_NOTES_OFF = 123,
};

/*
 * opcodes for keybindings
 *
 * >=0 values represent MIDI notes to be played
 * < 0 values represent operations
 */
typedef enum {
  OP_INVALID = -10,
  OP_INC_OCTAVE,
  OP_DEC_OCTAVE,
  OP_INC_VELOCITY,
  OP_DEC_VELOCITY,
  OP_INC_BANK,
  OP_DEC_BANK,
  OP_INC_PROG,
  OP_DEC_PROG,
  OP_PANIC,
  OP_C = 0,
  OP_CSHARP,
  OP_D,
  OP_DSHARP,
  OP_E,
  OP_F,
  OP_FSHARP,
  OP_G,
  OP_GSHARP,
  OP_A,
  OP_ASHARP,
  OP_B,
} opcode_t;

/*
 * writes NoteOn midi message to buffer.
 * return: 1 on success, 0 on failure.
 */
int write_note_on(char channel, char pitch, char vel);
/*
 * writes NoteOff midi message to buffer.
 * return: 1 on success, 0 on failure.
 */
int write_note_off(char channel, char pitch, char vel);
/*
 * writes control midi message to buffer.
 * return: 1 on success, 0 on failure.
 */
int write_control(char channel, char controller, char val);
/*
 * writes bank select midi message to buffer.
 * return: 1 on success, 0 on failure.
 */
int write_bank_sel(char channel, uint_least16_t val);
/*
 * writes prog change message to buffer.
 * return: 1 on success, 0 on failure.
 */
int write_prog_change(char channel, char prog);
/* auto connect to any midi input port */
void refresh_ports(void);

/* init jack */
void jack_init(int auto_connect);
/* grace shutdown jack */
void jack_shutdown(void);

#endif
