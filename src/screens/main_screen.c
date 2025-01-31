#include "main_screen.h"

#include <stdlib.h>
#include <ncurses.h>

void MainScreenInit(MainScreen *self) {
    self->menu.n_entries = 4;
    self->menu.entries = malloc(self->menu.n_entries * sizeof(char *));
    self->menu.entries[0] = "  Create user  ";
    self->menu.entries[1] = "     Login     ";
    self->menu.entries[2] = "  Guest login  ";
    self->menu.entries[3] = "  Leaderboard  ";
    self->menu.selected = 0;
}

int MainScreenHandleInput(void *selfv, int input) {
    MainScreen *self = (MainScreen *)selfv;

    if (input == KEY_RESIZE)
        clear();

    if (input == '\n') {
        switch (self->menu.selected)
        {
        case 0: return 2;
        case 1: return 1;
        default:
            break;
        }
    }

    MenuHandleInput(&(self->menu), input);

    return -1;
}

void MainScreenRender(void *selfv) {
    MainScreen *self = (MainScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);
    mvprintw(x / 2 - 2, y / 2 - 8, "Welcome to ROGUE!");

    self->menu.x = x / 2;
    self->menu.y = y / 2 - 10;

    MenuRender(&self->menu);
}

void MainScreenFree(void *selfv) {
    MainScreen *self = (MainScreen *)selfv;
    free(self->menu.entries);
}
