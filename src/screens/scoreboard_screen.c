#include "scoreboard_screen.h"
#include "data/users.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>

#include "../input.h"

#define PAGE_SIZE 15

int comp(const void *a_ptr, const void *b_ptr) {
    int a = (*((User **)a_ptr))->score_sum;
    int b = (*((User **)b_ptr))->score_sum;

    if (a > b) return -1;
    if (a < b) return 1;
    return 0;
}


void ScoreboardScreenInit(ScoreboardScreen *self) {
    self->page = 0;
    self->users = NULL;

    ScoreboardScreenHandleSwitch(self);
}

void ScoreboardScreenHandleSwitch(void *selfv) {
    ScoreboardScreen *self = (ScoreboardScreen *)selfv;

    self->page = 0;
    self->users = realloc(self->users, usermanager.n_users * sizeof(User *));

    memcpy(self->users, usermanager.users, usermanager.n_users * sizeof(User *));
    qsort(self->users, usermanager.n_users, sizeof(User *), comp);
}

int ScoreboardScreenHandleInput(void *selfv, int input) {
    if (input == KEY_RESIZE)
        clear();

    if (input == KEY_F(1)) {
        curs_set(0);
        return (logged_in_user ? 3 : 0);
    }

    ScoreboardScreen *self = (ScoreboardScreen *)selfv;
    int num_pages = usermanager.n_users / PAGE_SIZE + (usermanager.n_users % PAGE_SIZE > 0);

    if (IsLeft(input) && self->page) {
        clear();
        self->page--;
    }
    else if (IsRight(input) && self->page < num_pages - 1) {
        clear();
        self->page++;
    }

    return -1;
}

void ScoreboardScreenRender(void *selfv) {
    ScoreboardScreen *self = (ScoreboardScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);

    move(x - 1, 0);
    printw("Exit: ");
    attron(A_ITALIC);
    printw("<F1>   ");
    attroff(A_ITALIC);

    int num_pages = usermanager.n_users / PAGE_SIZE + (usermanager.n_users % PAGE_SIZE > 0);
    int range_max = self->page < num_pages - 1 ? PAGE_SIZE : usermanager.n_users % PAGE_SIZE;
    for (int i = 0; i < range_max; i++) {
        int idx = i + PAGE_SIZE * self->page;
        User *u = self->users[idx];

        char buffer[100];
        sprintf(buffer, "%dth - %s - Score sum: %d - Gold sum: %d - %d game played - Exp: %llus",
            idx + 1, u->username, u->score_sum, u->gold_sum,
            u->game_count, u->first_game_time ? time(NULL) - u->first_game_time : 0);

        int len = strlen(buffer) + (idx < 3 ? 2 : 0);

        if (idx == 0)
            len += 7;
        else if (idx < 3)
            len += 9;

        move(i + (x - range_max) / 2, (y - len) / 2);
        if (idx == 0) {
            attron(COLOR_PAIR(5));
            printw("(\U0001F947 Goat) ");
        }
        else if (idx == 1) {
            attron(COLOR_PAIR(2));
            printw("(\U0001F948 Legend) ");
        }
        else if (idx == 2) {
            attron(COLOR_PAIR(3));
            printw("(\U0001F949 Master) ");
        }
        if (u == logged_in_user)
            attron(A_BOLD);
        printw("%s", buffer);
        attroff(A_BOLD | COLOR_PAIR(5) | COLOR_PAIR(2) | COLOR_PAIR(3));
    }
    refresh();
}

void ScoreboardScreenFree(void *selfv) {
    // ScoreboardScreen *self = (ScoreboardScreen *)selfv;
}
