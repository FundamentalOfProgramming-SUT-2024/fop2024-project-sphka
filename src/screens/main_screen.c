#include "main_screen.h"

#include <stdlib.h>
#include <ncurses.h>

void MainScreenInit(MainScreen *self) {
    self->menu.entries = malloc(3 * sizeof(char *));
    self->menu.entries[0] = "Create New User";
    self->menu.entries[1] = "     Login     ";
    self->menu.entries[2] = "  Leaderboard  ";
    self->menu.n_entries = 3;
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
