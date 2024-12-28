#include "game_screen.h"

#include <stdlib.h>
#include <string.h>

#include <ncurses.h>


void GameScreenInit(GameScreen *self) {
    self->px = 6;
    self->py = 8;
    GenerateFloor(&self->floor);
}

int GameScreenHandleInput(void *selfv, int input) {
    GameScreen *self = (GameScreen *)selfv;

    if (input == KEY_RESIZE)
        clear();

    int px = self->px;
    int py = self->py;

    if (input == KEY_UP || input == 'w')
        self->px--;
    else if (input == KEY_DOWN || input == 's')
        self->px++;
    else if (input == KEY_RIGHT || input == 'd')
        self->py++;
    else if (input == KEY_LEFT || input == 'a')
        self->py--;

    if (self->px >= MAXLINES || self->py >= MAXCOLS || self->px <= 0 || self->py <= 0) {
        self->px = px;
        self->py = py;

        return 1;
    }

    char ch = self->floor.tiles[self->px * MAXCOLS + self->py].c;
    if (ch != '.' && ch != '+' && ch != '#' && ch != '<') {
        self->px = px;
        self->py = py;

        return 1;
    }

    return 0;
}

void GameScreenRender(void *selfv) {
    GameScreen *self = (GameScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);

    for (int _i = 1; _i < x - 1; _i++)
        for (int j = 0; j < y; j++) {
            int i = _i - 1;

            if (i < MAXLINES && j < MAXCOLS) {
                char ch = self->floor.TILE(i, j).c;
                int flags = 0;

                switch (ch)
                {
                case '#': flags = COLOR_PAIR(1); break;
                case '+': case '-': case '|':
                    flags = COLOR_PAIR(2); break;
                case '<': flags = COLOR_PAIR(4) | A_REVERSE; break;
                case 'O': flags = COLOR_PAIR(3); break;
                case '=': flags = COLOR_PAIR(3); break;
                default:
                    break;
                }

                mvaddch(i, j, ch | flags);
            }
        }

    mvaddch(self->px, self->py, '@' | COLOR_PAIR(4));
    mvaddch(x - 1, 0, ':');
}

void GameScreenFree(void *self) {
}
