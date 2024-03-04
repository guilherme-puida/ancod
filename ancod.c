/*
 * ancod - generate formatted strings using ansi codes
 * Copyright (C) 2024  Guilherme Puida
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define _XOPEN_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#define XSTR(x) STR(x)
#define STR(x)  #x

#define ANCOD_VERSION 0.1

static bool output_color = true;

static void
version(void)
{
    puts("ancod " XSTR(ANCOD_VERSION));
}

static void
usage(FILE *f)
{
    fprintf(f, "Usage: ancod [-fhnrv] [STRING]...\n");
    fprintf(f, "  -f    force styled output, overriding the NO_COLOR environment variable\n");
    fprintf(f, "  -h    print this message and exit\n");
    fprintf(f, "  -n    do not output the trailing newline\n");
    fprintf(f, "  -r    do not reset styles after every input\n");
    fprintf(f, "  -v    print version information and exit\n");
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

    FG_BLACK   = 30,
    FG_RED     = 31,
    FG_GREEN   = 32,
    FG_YELLOW  = 33,
    FG_BLUE    = 34,
    FG_MAGENTA = 35,
    FG_CYAN    = 36,
    FG_WHITE   = 37,
    FG_DEFAULT = 39,

    BG_BLACK   = 40,
    BG_RED     = 41,
    BG_GREEN   = 42,
    BG_YELLOW  = 43,
    BG_BLUE    = 44,
    BG_MAGENTA = 45,
    BG_CYAN    = 46,
    BG_WHITE   = 47,
    BG_DEFAULT = 49,
};

static void
ansi_code_for(int value)
{
    if (output_color) {
        printf("\033[%dm", value);
    }
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

    // handle [/^], [^*], [/b], [/d], [/i], [/u] and [/s].
    if (len == 2 && *start == '/') {
        switch (start[1]) {
            case '^': return FG_DEFAULT;
            case '*': return BG_DEFAULT;
            case 'b': return NO_BOLD;
            case 'd': return NO_DIM;
            case 'i': return NO_ITALIC;
            case 'u': return NO_UNDER;
            case 's': return NO_STRIKE;
        }
    }

    // from now on, the only valid options are fg and bg colors.
    if (*start != '^' && *start != '*') {
        return INVALID;
    }

    bool is_background = *start == '*';

    // skip first character and ajudst length.
    start++;
    len--;

#define MATCH(name, fg, bg)                                                       \
    do {                                                                          \
        int name_len = sizeof(name) - 1;                                          \
        if (name_len == len && start[0] == name[0] &&                             \
            (len <= 1 || start[1] == name[1]) &&                                  \
            (len <= 2 || start[2] == name[2]) &&                                  \
            (len <= 3 || start[3] == name[3]) &&                                  \
            (len <= 4 || start[4] == name[4]) &&                                  \
            (len <= 5 || start[5] == name[5]) &&                                  \
            (len <= 6 || start[6] == name[6]) &&                                  \
            (len <= 7 || start[7] == name[7]) &&                                  \
            (len <= 8 || start[8] == name[8]) &&                                  \
            (len <= 9 || start[9] == name[9]) &&                                  \
            (len <= 10 || start[10] == name[10])) return is_background ? bg : fg; \
        } while (0)

    MATCH("black", FG_BLACK, BG_BLACK);
    MATCH("red", FG_RED, BG_RED);
    MATCH("green", FG_GREEN, BG_GREEN);
    MATCH("yellow", FG_YELLOW, BG_YELLOW);
    MATCH("blue", FG_BLUE, BG_BLUE);
    MATCH("magenta", FG_MAGENTA, BG_MAGENTA);
    MATCH("cyan", FG_CYAN, BG_CYAN);
    MATCH("white", FG_WHITE, BG_WHITE);

#undef MATCH

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
    bool trailing_newline = true;
    bool reset_between = true;

    // support the NO_COLOR environment variable.
    // see https://no-color.org
    char *no_color = getenv("NO_COLOR");
    if (no_color != NULL && *no_color != 0) {
        output_color = false;
    }

    int option;
    while ((option = getopt(argc, argv, "vh?fnr")) != -1) {
        switch (option) {
            case 'f':
                output_color = true;
                break;
            case 'n':
                trailing_newline = false;
                break;
            case 'r':
                reset_between = false;
                break;
            case 'v':
                version();
                exit(EXIT_SUCCESS);
            case 'h':
                usage(stdout);
                exit(EXIT_SUCCESS);
            default:
                usage(stderr);
                exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; ++i) {
        emit_from_tags(argv[i]);

        if (reset_between) {
            ansi_code_for(RESET);
        }

        if (i + 1 < argc) {
            putchar(' ');
        }
    }

    if (trailing_newline) {
        putchar('\n');
    }

    exit(EXIT_SUCCESS);
}
