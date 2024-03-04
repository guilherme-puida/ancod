#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#define XSTR(x) STR(x)
#define STR(x)  #x

#define ANCOD_VERSION 0.1

static void
version(void)
{
    puts("ancod " XSTR(ANCOD_VERSION));
}

static void
usage(FILE *f)
{
    fprintf(f, "Usage: ancod MESSAGE...\n");
    fprintf(f, "  -h, -?    print this message and exit\n");
    fprintf(f, "  -v        print version information and exit\n");
}

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
        if (*end == '\\' && end[1] == '[') {
            putchar('[');
            end++;
        } else if (*end == '[' && level++ == 0) {
            start = end;
        } else if (*end == ']' && level > 0 && --level == 0) {
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

    if (level) {
        int len = end - start + 1;
        printf("%.*s", len, start);
    }
}

int
main(int argc, char **argv)
{
    int option;
    while ((option = getopt(argc, argv, "vh?")) != -1) {
        switch (option) {
            case 'v':
                version();
                exit(EXIT_SUCCESS);
            case 'h': case '?':
                usage(stdout);
                exit(EXIT_SUCCESS);
            default:
                usage(stderr);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    for (int i = optind; i < argc; ++i) {
        emit_from_tags(argv[i]);

        if (i + 1 < argc) {
            putchar(' ');
        }
    }

    exit(EXIT_SUCCESS);
}
