.POSIX:
.SUFFIXES:
.PHONY: all clean install check

PROJECT   =ssh-clipboard
VERSION   =1.0.0
CC        = $(TPREFIX)gcc -Wall -g3 -std=c99
PREFIX    =/usr/local
BUILDDIR ?=.build
EXE      ?=$(shell uname -s | awk '/Windows/ || /MSYS/ || /CYG/ { print ".exe" }')
PROGS     =$(BUILDDIR)/ssh-clipboard$(EXE) $(BUILDDIR)/ssh-copy$(EXE) $(BUILDDIR)/ssh-paste$(EXE)
SOURCES   =auxiliary.c pexec_win32.c pexec_unix.c
HEADERS   =auxiliary.h pexec.h

all: $(PROGS)
clean:
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(PROGS) $(DESTDIR)$(PREFIX)/bin
check:

$(BUILDDIR)/%$(EXE): %.c $(SOURCES) $(HEADERS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $< $(SOURCES) $(LDFLAGS) $(LIBS)

## -- BLOCK:c --
clean: clean-c
clean-c:
	rm -f *.o
## -- BLOCK:c --
