#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
    if (argc != 2) {
        exit(EXIT_FAILURE);
    }

    char *input = argv[1];
    puts(input);

    exit(EXIT_SUCCESS);
}
