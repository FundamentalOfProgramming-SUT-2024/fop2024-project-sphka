#include "main_screen.h"

#include <stdlib.h>

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

    return MenuHandleInput(&(self->menu), input);
}

void MainScreenRender(void *selfv) {
    MainScreen *self = (MainScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);
    mvprintw(x / 2 - 2, y / 2 - 8, "Welcome to ROGUE!");
    MenuRender(&(self->menu), x / 2, y / 2 - 10);
}

void MainScreenFree(void *selfv) {
    MainScreen *self = (MainScreen *)selfv;
    free(self->menu.entries);
}
