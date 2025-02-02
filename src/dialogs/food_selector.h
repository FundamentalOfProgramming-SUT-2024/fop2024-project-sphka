#pragma once

#include <curses.h>
#include <ctype.h>

#include "../data/game.h"
#include "../random.h"

static void RenderHungerEffect(WINDOW *win) {
    wattron(win, COLOR_PAIR(1));
    wprintw(win, "\u2193");
    wattroff(win, COLOR_PAIR(1));
    wprintw(win, " hunger");
}

static void RenderStrengthEffect(WINDOW *win) {
    wprintw(win, " + ");
    wattron(win, COLOR_PAIR(4));
    wprintw(win, "\u2191");
    wattroff(win, COLOR_PAIR(4));
    wprintw(win, " strength");
}

static void RenderPoisionEffect(WINDOW *win) {
    wattron(win, COLOR_PAIR(6));
    wprintw(win, " (might be rotten)");
    wattroff(win, COLOR_PAIR(6));
}

void FoodSelector(int x, int y) {
    WINDOW *win = newwin(11, 60, (x - 11) / 2, (y - 60) / 2);
    box(win, 0, 0);

    wmove(win, 1, 1);
    mvwprintw(win, 1, 1, "                Count");

    char keys[] = { 'M', 'S', 'N' };
    int rotten_count = game.player.foods[FoodType_Rotten];

    for (int i = 0; i < 3; i++) {
        ItemInfo *info = &foods[i];
        wattron(win, COLOR_PAIR(1) | A_BOLD);
        mvwprintw(win, i + 2, 2, "%c ", keys[i]);
        wattroff(win, COLOR_PAIR(1) | A_BOLD);
        wprintw(win, "%s", info->name);
        mvwprintw(win, i + 2, 19, "%d    ", (i == FoodType_Normal ? rotten_count : 0) + game.player.foods[i]);
        RenderHungerEffect(win);
        if (i > 0)
            RenderStrengthEffect(win);
        else
            RenderPoisionEffect(win);
    }

    mvwprintw(win, 8, 2, "Press a red letter's key to consume that type of food.");
    mvwprintw(win, 9, 2, "Press any other key to exit.");
    wrefresh(win);

    int ch = tolower(wgetch(win));
    FoodType food_type;

    switch (ch)
    {
    case 'n':
        food_type = FoodType_Normal;
        break;

    case 's':
        food_type = FoodType_Supreme;
        break;

    case 'm':
        food_type = FoodType_Magical;
        break;

    default:
        sprintf(g_message_bar, "Your didn't consume any food (invalid/exit key pressed).");
        goto end;
    }

    if (game.player.foods[food_type] || (food_type == FoodType_Normal && rotten_count)) {
        if (food_type == FoodType_Normal) {
            int true_type = randn(game.player.foods[FoodType_Normal] + rotten_count);
            if (true_type < rotten_count)
                food_type = FoodType_Rotten;
        }

        ConsumeFood(food_type);
    } else {
        sprintf(g_message_bar, "You don't have any %s to consume!", foods[food_type].name);
    }

end:
    clear();
    delwin(win);
}
