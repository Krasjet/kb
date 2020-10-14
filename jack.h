#ifndef KB_JACK_H
#define KB_JACK_H

enum {
  /* we only need 3 byte MIDI message */
  MSG_SIZE = 3,
  /* valid MIDI notes 0-127 */
  MAX_MIDI_NOTE = 127,
  MAX_OCTAVE = 10,
  MIN_OCTAVE = 0,
  MAX_VELOCITY = 127,
  MIN_VELOCITY = 0
};

/*
 * opcodes for keybindings
 *
 * >=0 values represent MIDI notes to be played
 * < 0 values represent operations
 */
typedef enum {
  OP_INVALID = -5,
  OP_INC_OCTAVE,
  OP_DEC_OCTAVE,
  OP_INC_VELOCITY,
  OP_DEC_VELOCITY,
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

/* init jack */
void jack_init(void);
/* grace shutdown jack */
void jack_shutdown(void);

#endif
