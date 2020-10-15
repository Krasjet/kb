kb
==

kb is a customizable virtual MIDI keyboard daemon for JACK Audio Connection
Kit. It provides MIDI input for standalone JACK synthesizers and can be a
useful debugging tool for developing JACK-based audio applications.

Usage
-----

First start a JACK server using `jackd(1)` or `qjackctl(1)` (see [1] for
details on how to set it up), then run

    $ kb

to start the MIDI keyboard daemon. Then, you need to connect the `midi_out`
port of kb to other applications (for qjackctl, use the `Graph` button):

    +----+-----------+       +-------+-----------+
    | kb |           |       | synth |           |
    +----+           |       +-------+           |
    |                |       |         audio_out +--->
    |       midi_out +------>+ midi_in           |
    +----------------+       +-------------------+

kb will capture all the key presses and convert them into MIDI messages
according to your configuration.

For the default configuration, the home row of a QWERTY keyboard

    ASDFGHJKL;'

corresponds to the white keys on a piano keyboard, and

    WETYUOP]

corresponds to the black keys. Additionally,

    ZX

can be used to decrease and increase octave and

    CV

can be used to decrease and increase velocity. You can easily customize these
keybindings. See CUSTOMIZE section for details.

If you want to shut down the daemon, press `Ctrl-C` in the terminal to send a
`SIGINT` signal or run

    $ pkill kb

to kill the daemon process. For more information, see `kb(1)`.

Build
-----

First, make sure JACK, Xlib, and libxi are installed. For Arch-based distros,
they can be installed by

    $ pacman -S jack2 libx11 libxi

For Debian-based distros, install the dependencies by

    $ apt-get install libjack-jackd2-dev libx11-dev libxi-dev

If you don't want to use libxi, check out the `no_ext` branch. The `no_ext`
version works exactly the same, but it might have some issues capturing input
within REAPER on Linux.

You also need a C99-compatible C compiler and POSIX make. After all the
requirement are satisfied, run

    $ make

to build kb and

    # make install

to install it to your system.

Customize
---------

kb can be customized by editing the `config.h` file and recompiling the source
code. The default configuration is in `config.def.h`. It provides a simple
one-row keyboard layout. Run

    $ make config.h

to obtain the default config file.

Take a look at the alternative layouts in

- config.2row.h: two row keyboard layout (2 octaves)
- config.min.h: the minimal config file

to understand how to customize kb.

Caveats
-------

1. kb has only been tested on Linux and with JACK 2. It should work on any
   system with X11 and JACK support, but I have not tested other systems yet.
   Feedbacks are welcome.
2. Note that kb *captures* your key presses in any window. It does not
   *intercept* them. This is intentional, because I frequently need to edit
   code while testing the MIDI input. As an additional benefit, you can now
   enjoy some crappy generative music while typing.


[1]: https://ccrma.stanford.edu/docs/common/JACK.html
