.POSIX:

PREFIX = /usr/local
CFLAGS = -Wall -Wextra -std=c99 -g3 -pedantic -Os
LDLIBS = -lX11 -ljack

SRC = kb.c jack.c x.c util.c
OBJ = ${SRC:.c=.o}

kb: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDLIBS)

config.h: config.def.h
	sed 's/def\.//' config.def.h > $@

kb.o: config.h jack.h x.h util.h
jack.o: jack.h util.h
x.o: util.h

run: kb
	@./kb

clean:
	rm -f kb *.o

nuke: clean
	rm -f config.h

install: kb
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 755 kb $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/fretl

.PHONY: run clean nuke install uninstall
