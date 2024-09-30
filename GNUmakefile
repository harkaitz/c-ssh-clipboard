.POSIX:
.SUFFIXES:
.PHONY: all clean install check

PROJECT   =ssh-clipboard
VERSION   =1.0.0
CC        =$(TPREFIX)gcc -Wall -g3 -std=c99
PREFIX    =/usr/local
BUILDDIR ?=.build
EXE      ?=$(shell uname -s | awk '/Windows/ || /MSYS/ || /CYG/ { print ".exe" }')
PROGS     =$(BUILDDIR)/ssh-clipboard$(EXE) $(BUILDDIR)/ssh-copy$(EXE) $(BUILDDIR)/ssh-paste$(EXE)
SOURCES   =auxiliary.c pexec_win32.c pexec_unix.c
HEADERS   =auxiliary.h pexec.h
TOOLCHAINS=x86_64-linux-musl aarch64-linux-musl x86_64-w64-mingw32

all: $(PROGS)
clean:
	rm -f $(PROGS)
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(PROGS) $(DESTDIR)$(PREFIX)/bin
check:
release:
	mkdir -p $(BUILDDIR)
	hrelease -t "$(TOOLCHAINS)" -N "$(PROJECT)" -R "$(VERSION)" -o $(BUILDDIR)/Release
	gh release create v$(VERSION) $$(cat $(BUILDDIR)/Release)


$(BUILDDIR)/%$(EXE): %.c $(SOURCES) $(HEADERS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $< $(SOURCES) $(LDFLAGS) $(LIBS)

## -- BLOCK:c --
clean: clean-c
clean-c:
	rm -f *.o
## -- BLOCK:c --
