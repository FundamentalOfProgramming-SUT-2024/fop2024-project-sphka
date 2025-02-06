#include "profile_screen.h"
#include "data/users.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>

#include "../input.h"

void ProfileScreenInit(ProfileScreen *self) {
}

void ProfileScreenHandleSwitch(void *selfv) {
    // ProfileScreen *self = (ProfileScreen *)selfv;
}

int ProfileScreenHandleInput(void *selfv, int input) {
    if (input == KEY_RESIZE)
        clear();

    if (input == KEY_F(1)) {
        curs_set(0);
        return 3;
    }

    // ProfileScreen *self = (ProfileScreen *)selfv;

    return -1;
}

static void PrintCentered(int x, const char *str) {
    int y = getmaxx(stdscr);

    move(x, (y - strlen(str)) / 2);
    printw("%s", str);
}

void ProfileScreenRender(void *selfv) {
    // ProfileScreen *self = (ProfileScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);

    move(x - 1, 0);
    printw("Exit: ");
    attron(A_ITALIC);
    printw("<F1>   ");
    attroff(A_ITALIC);

    char buffer[100];

    mvprintw(x / 2 - 4, (y - 11 - strlen(logged_in_user->username)) / 2 - 5, "   - ");
    attron(A_ITALIC | A_BOLD);
    sprintf(buffer, "Profile of %s", logged_in_user->username); PrintCentered(x / 2 - 4, buffer);
    attroff(A_ITALIC | A_BOLD);
    printw(" -   ");

    sprintf(buffer, "Email: %s", logged_in_user->email); PrintCentered(x / 2 - 2, buffer);
    sprintf(buffer, "Total scores: %d", logged_in_user->score_sum); PrintCentered(x / 2 - 1, buffer);
    attron(COLOR_PAIR(5));
    sprintf(buffer, "Total gold collected: %d", logged_in_user->gold_sum); PrintCentered(x / 2, buffer);
    attroff(COLOR_PAIR(5));
    sprintf(buffer, "Total games played: %d", logged_in_user->game_count); PrintCentered(x / 2 + 1, buffer);
    sprintf(buffer, "Time since first game: %llus", logged_in_user->first_game_time ? time(NULL) - logged_in_user->first_game_time : 0); PrintCentered(x / 2 + 2, buffer);

    refresh();
}

void ProfileScreenFree(void *selfv) {
    // ProfileScreen *self = (ProfileScreen *)selfv;
}
