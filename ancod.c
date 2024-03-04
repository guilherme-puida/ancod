#include <stdio.h>
#include <stdlib.h>

enum ansi_code {
    INVALID = -1,
    RESET   = 0,

    BOLD   = 1,
    DIM    = 2,
    ITALIC = 3,
    UNDER  = 4,
    STRIKE = 9,

    // NOTE: bold and dim are both reset with 22.
    //       this is not an error.
    NO_BOLD   = 22,
    NO_DIM    = 22,
    NO_ITALIC = 23,
    NO_UNDER  = 24,
    NO_STRIKE = 29,
};

static void
ansi_code_for(int value)
{
    printf("\033[%dm", value);
}

static enum ansi_code
tag_to_ansi(const char *start, const char *end)
{
    int len = end - start + 1;

    // handle [/], [b], [i], [d], [u] and [s].
    if (len == 1) {
        switch (*start) {
            case '/': return RESET;
            case 'b': return BOLD;
            case 'd': return DIM;
            case 'i': return ITALIC;
            case 'u': return UNDER;
            case 's': return STRIKE;
        }
    }

    return INVALID;
}

static void
emit_from_tags(const char *input)
{
    const char *start = input;
    const char *end   = input;

    int level = 0;

    while (*end) {
        if (*end == '[' && level++ == 0) {
            start = end;
        } else if (*end == ']' && --level == 0) {
            int ansi = tag_to_ansi(start + 1, end - 1);
            if (ansi != INVALID) {
                ansi_code_for(ansi);
            } else {
                int len = end - start + 1;
                printf("%.*s", len, start);
            }
        } else if (!level) {
            putchar(*end);
        }

        end++;
    }
}

int
main(int argc, char **argv)
{
    if (argc != 2) {
        exit(EXIT_FAILURE);
    }

    char *input = argv[1];
    emit_from_tags(input);

    exit(EXIT_SUCCESS);
}
