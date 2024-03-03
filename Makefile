.POSIX:
CC     = cc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic

all: ancod

ancod: ancod.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f ancod
