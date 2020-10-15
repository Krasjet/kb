.POSIX:

# you might need to customize the flags for your system
PREFIX  = /usr/local
CFLAGS  = -Wall -Wextra -std=c99 -pedantic -Os
LDLIBS  = -lX11 -ljack -lXi
LDFLAGS =

OBJ = kb.o jack.o x.o util.o

kb: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

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

install: kb kb.1
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	install -m 755 kb $(DESTDIR)$(PREFIX)/bin
	gzip < kb.1 > $(DESTDIR)$(PREFIX)/share/man/man1/kb.1.gz

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/fretl
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/kb.1.gz

.PHONY: run clean nuke install uninstall
