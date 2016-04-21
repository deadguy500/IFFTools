#!/bin/sh

CC        = gcc

CFLAGS    = -std=c99 

LIBFILES  = lib/file.c \
			lib/iff.c

BPCONV    = bpconv
SPRCONV   = sprconv

all:
	$(CC) $(CFLAGS) $(LIBFILES) $(BPCONV)/$(BPCONV).c -o build/$(BPCONV)
	$(CC) $(CFLAGS) $(LIBFILES) $(SPRCONV)/$(SPRCONV).c -o build/$(SPRCONV)

clean:
	rm -rf build/$(BPCONV)
	rm -rf build/$(SPRCONV)