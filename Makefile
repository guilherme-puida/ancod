.POSIX:
CC     = cc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic
PREFIX = /usr/local

all: ancod

ancod: ancod.c
	$(CC) $(CFLAGS) -o $@ $<

install: ancod
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 ancod $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/share/man/man1
	install -m 644 ancod.1 $(DESTDIR)$(PREFIX)/share/man/man1

test: ancod test.sh
	./test.sh $(TEST_VERBOSE)

clean:
	rm -f ancod
