.POSIX:
CC     = cc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic

all: ancod

ancod: ancod.c
	$(CC) $(CFLAGS) -o $@ $<

test: ancod test.sh
	./test.sh $(TEST_VERBOSE)

clean:
	rm -f ancod
