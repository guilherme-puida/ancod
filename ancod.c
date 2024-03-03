#include <stdio.h>
#include <stdlib.h>

static void
ansi_code_for(int value)
{
    printf("\033[%dm", value);
}

int
main(int argc, char **argv)
{
    if (argc != 2) {
        exit(EXIT_FAILURE);
    }

    char *input = argv[1];

    ansi_code_for(32);
    puts(input);

    exit(EXIT_SUCCESS);
}
