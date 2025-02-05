#include "main_screen.h"

#include <stdlib.h>
#include <ncurses.h>

#include "../data/users.h"
#include "../screen.h"

void MainScreenInit(MainScreen *self) {
    self->menu.n_entries = 4;
    self->menu.entries = malloc(self->menu.n_entries * sizeof(char *));
    self->menu.entries[0] = "    Register   ";
    self->menu.entries[1] = "     Login     ";
    self->menu.entries[2] = "  Guest login  ";
    self->menu.entries[3] = "   Scoreboard  ";
    self->menu.selected = 0;
}

int MainScreenHandleInput(void *selfv, int input) {
    MainScreen *self = (MainScreen *)selfv;

    if (input == KEY_RESIZE)
        clear();

    if (input == KEY_F(1)) {
        return MAGIC_EXIT;
    }

    if (input == '\n') {
        switch (self->menu.selected)
        {
        case 0: return 2;
        case 1: return 1;
        case 2:
            logged_in_user = &guest;
            return 3;
        case 3: return 5;
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

    move(x - 1, 0);
    printw("Exit: ");
    attron(A_ITALIC);
    printw("<F1>   ");
    attroff(A_ITALIC);

    mvprintw(x / 2 - 2, y / 2 - 8, "Welcome to ROGUE!");

    self->menu.x = x / 2;
    self->menu.y = y / 2 - 10;

    MenuRender(&self->menu);
    refresh();
}

void MainScreenFree(void *selfv) {
    MainScreen *self = (MainScreen *)selfv;
    free(self->menu.entries);
}
