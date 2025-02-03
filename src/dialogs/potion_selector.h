#pragma once

#include <curses.h>
#include <ctype.h>

#include "../data/game.h"
#include "../random.h"

void PotionSelector(int x, int y) {
    WINDOW *win = newwin(11, 64, (x - 11) / 2, (y - 64) / 2);
    box(win, 0, 0);

    wmove(win, 1, 1);
    mvwprintw(win, 1, 1, "                Count");

    char keys[] = { 'H', 'S', 'D' };
    char *descs[][2] = {
        { "Heals you", "faster" },
        { "Makes you move", "faster" },
        { "Makes your attacks deal", "damage" }
    };

    for (int i = 0; i < 3; i++) {
        ItemInfo *info = &potions[i];
        wattron(win, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(win, i + 2, 2, "%c ", keys[i]);
        wattroff(win, COLOR_PAIR(4) | A_BOLD);
        wprintw(win, "%s", info->name);
        mvwprintw(win, i + 2, 19, "%d    ", game.player.potions[i]);
        wprintw(win, "%s", descs[i][0]);
        wattron(win, COLOR_PAIR(4) | A_BOLD);
        wprintw(win, " 2x ");
        wattroff(win, COLOR_PAIR(4) | A_BOLD);
        wprintw(win, "%s", descs[i][1]);
    }

    mvwprintw(win, 8, 2, "Press a green letter's key to consume a potion of that type.");
    mvwprintw(win, 9, 2, "Press any other key to exit.");
    wrefresh(win);

    int ch = tolower(wgetch(win));
    PotionType potion_type;

    switch (ch)
    {
    case 'h':
        potion_type = PotionType_Health;
        break;

    case 's':
        potion_type = PotionType_Speed;
        break;

    case 'd':
        potion_type = PotionType_Damage;
        break;

    default:
        sprintf(g_message_bar, "Your didn't consume any potions (invalid/exit key pressed).");
        goto end;
    }

    if (game.player.potions[potion_type])
        ConsumePotion(potion_type);
    else
        sprintf(g_message_bar, "You don't have any %s to consume!", potions[potion_type].name);

end:
    clear();
    delwin(win);
}
