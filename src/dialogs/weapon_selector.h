#pragma once

#include <curses.h>

#include "../data/game.h"

void WeaponSelector() {
    WINDOW *win = newwin(8, 50, 1, 1);
    box(win, 0, 0);

    wmove(win, 1, 1);
    mvwprintw(win, 1, 1, "                Damage    Range    Count");

    for (int i = 0; i < game.player.n_weapons; i++) {
        Item *weapon = &game.player.weapons[i];
        mvwprintw(win, i + 2, 1, "%c %s", weapon->info->sprite, weapon->info->name);
        mvwprintw(win, i + 2, 19, "%d", weapon->ex_weapon.type->damage);

        if (weapon->ex_weapon.type->range) {
            mvwprintw(win, i + 2, 29, "%d", weapon->ex_weapon.type->range);
            mvwprintw(win, i + 2, 37, "%d", weapon->count);
        }
    }
    wrefresh(win);

    getch();
}
