#include "pregame_screen.h"

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../data/users.h"
#include "../data/game.h"

void DeserializeGame(FILE *file, Game *game);

void PregameScreenInit(PregameScreen *self) {
    self->menu.n_entries = 6;
    self->menu.entries = malloc(self->menu.n_entries * sizeof(char *));
    self->menu.entries[0] = "   New  Game   ";
    // self->menu.entries[1] = "   Load Game   ";
    self->menu.entries[2] = "   Scoreboard  ";
    self->menu.entries[3] = "    Profile    ";
    self->menu.entries[4] = "    Settings   ";
    self->menu.entries[5] = "     Logout    ";
    self->menu.selected = 0;
}

void PregameScreenSwitch(void *selfv) {
    PregameScreen *self = (PregameScreen *)selfv;

    if (logged_in_user->has_save)
        self->menu.entries[1] = "   Load Game   ";
    else
        self->menu.entries[1] = " No game saved ";
}

int PregameScreenHandleInput(void *selfv, int input) {
    PregameScreen *self = (PregameScreen *)selfv;

    if (input == KEY_RESIZE)
        clear();

    if (input == '\n') {
        switch (self->menu.selected)
        {
        case 0:
            NewGame();
            return 4;
        case 1:
            {
                if (logged_in_user->has_save) {
                    char filename[100];
                    sprintf(filename, "%s-save.data", logged_in_user->username);
                    FILE *file = fopen(filename, "rb");
                    DeserializeGame(file, &game);
                    fclose(file);
                    return 4;
                }
                break;
            }
        case 2:
            return 5;
        case 3:
            return 7;
        case 4:
            return 6;
        case 5:
            logged_in_user = NULL;
            return 0;
        default:
            break;
        }
    }

    MenuHandleInput(&(self->menu), input);

    return -1;
}

void PregameScreenRender(void *selfv) {
    PregameScreen *self = (PregameScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);
    mvprintw(x / 2 - 2, (y - (9 + strlen(logged_in_user->username))) / 2, "Welcome, %s!", logged_in_user->username);

    self->menu.x = x / 2;
    self->menu.y = y / 2 - 10;

    MenuRender(&self->menu);
    refresh();
}

void PregameScreenFree(void *selfv) {
    PregameScreen *self = (PregameScreen *)selfv;
    free(self->menu.entries);
}
