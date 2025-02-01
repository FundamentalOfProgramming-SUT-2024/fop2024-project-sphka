#pragma once

#include <curses.h>
#include <ctype.h>

#include "../data/game.h"

void WeaponSelector() {
    WINDOW *win = newwin(11, 50, 1, 1);
    box(win, 0, 0);

    wmove(win, 1, 1);
    mvwprintw(win, 1, 1, "                Damage    Range    Count");

    for (int i = 0; i < game.player.n_weapons; i++) {
        Item *weapon = &game.player.weapons[i];
        wattron(win, COLOR_PAIR(3));
        mvwprintw(win, i + 2, 1, "%c ", weapon->info->sprite);
        wattroff(win, COLOR_PAIR(3));
        wprintw(win, "%s", weapon->info->name);
        mvwprintw(win, i + 2, 19, "%d", weapon->ex_weapon.type->damage);

        if (weapon->ex_weapon.type->range) {
            mvwprintw(win, i + 2, 29, "%d", weapon->ex_weapon.type->range);
            mvwprintw(win, i + 2, 37, "%d", weapon->count);
        }
    }

    mvwprintw(win, 8, 1, "Press a blue letter's key to equip that weapon.");
    mvwprintw(win, 9, 1, "Press any other key to exit.");
    wrefresh(win);

    int ch = wgetch(win);
    int new_equipment = -1;

    for (int i = 0; i < game.player.n_weapons; i++) {
        Item *weapon = &game.player.weapons[i];
        if (weapon->info->sprite == toupper(ch)) {
            new_equipment = i;
            break;
        }
    }

    if (new_equipment >= 0) {
        game.player.equipment = new_equipment;
        sprintf(g_message_bar, "You equiped your %s.", game.player.weapons[new_equipment].info->name);
    } else {
        sprintf(g_message_bar, "Your equipment was not modified (invalid/exit key pressed).");
    }
}
