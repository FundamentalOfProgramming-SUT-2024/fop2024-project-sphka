#include <ncurses.h>
#include <ctype.h>

#include "base.h"

static inline bool IgnoreCaseEq(int input, char c) {
    return tolower(input) == tolower(c);
}

static inline bool IsUp(int input) {
    return input == KEY_UP || IgnoreCaseEq(input, 'j') || IgnoreCaseEq(input, 'w');
}

static inline bool IsDown(int input) {
    return input == KEY_DOWN || IgnoreCaseEq(input, 'k') || IgnoreCaseEq(input, 's');
}

static inline bool IsRight(int input) {
    return input == KEY_RIGHT || IgnoreCaseEq(input, 'l') || IgnoreCaseEq(input, 'd');
}

static inline bool IsLeft(int input) {
    return input == KEY_LEFT || IgnoreCaseEq(input, 'h') || IgnoreCaseEq(input, 'a');
}

static inline Coord InputDirection(int input) {
    Coord out = { 0, 0 };

    if (IsUp(input))
        out.x = -1;

    if (IsDown(input))
        out.x = 1;

    if (IsRight(input))
        out.y = 1;

    if (IsLeft(input))
        out.y = -1;

    if (input == KEY_PPAGE || IgnoreCaseEq(input, 'u')) {
        out.x = -1;
        out.y = 1;
    }

    if (input == KEY_NPAGE || IgnoreCaseEq(input, 'n')) {
        out.x = 1;
        out.y = 1;
    }

    if (input == KEY_HOME || IgnoreCaseEq(input, 'y')) {
        out.x = -1;
        out.y = -1;
    }

    if (input == KEY_END || IgnoreCaseEq(input, 'b')) {
        out.x = 1;
        out.y = -1;
    }

    return out;
}
