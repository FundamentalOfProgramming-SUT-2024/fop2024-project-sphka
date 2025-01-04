#include "game_screen.h"

#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "input.h"

void GameScreenInit(GameScreen *self) {
    GenerateFloor(&self->floor);
    self->player = GetRandomCoord(&self->floor);
}

int GameScreenHandleInput(void *selfv, int input) {
    GameScreen *self = (GameScreen *)selfv;

    if (input == KEY_RESIZE)
        clear();

    Coord player = self->player;
    Coord delta = InputDirection(input);

    self->player.x += delta.x;
    self->player.y += delta.y;

    // if (input == KEY_UP || input == 'w')
    //     self->player.x--;
    // else if (input == KEY_DOWN || input == 's')
    //     self->player.x++;
    // else if (input == KEY_RIGHT || input == 'd')
    //     self->player.y++;
    // else if (input == KEY_LEFT || input == 'a')
    //     self->player.y--;

    if (input == 'r') {
        GenerateFloor(&self->floor);
        self->player = GetRandomCoord(&self->floor);
    }

#if 0
    if (self->player.x >= MAXLINES || self->player.y >= MAXCOLS || self->player.x <= 0 || self->player.y <= 0) {
        self->player = player;

        return 1;
    }
#endif

    char ch = self->floor.TILEC(self->player).c;
    if (ch != '.' && ch != '+' && ch != '#' && ch != '<') {
        self->player = player;
    }

    return -1;
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

    mvaddch(self->player.x, self->player.y, '@' | COLOR_PAIR(4));
    mvaddch(x - 1, 0, ':');
}

void GameScreenFree(void *self) {
}
