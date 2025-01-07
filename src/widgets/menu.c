#include "menu.h"

#include <ncurses.h>

int MenuHandleInput(Menu *menu, int input) {
    if (input == KEY_DOWN && menu->selected < menu->n_entries - 1) {
        menu->selected++;
        return true;
    } else if (input == KEY_UP && menu->selected > 0) {
        menu->selected--;
        return true;
    }

    return false;
}

void MenuRender(Menu *menu) {
    int x = menu->x;
    int y = menu->y;

    for (int i = 0; i < menu->n_entries; i++) {
        if (i == menu->selected) {
            move(x + i, y);
            addch('>' | COLOR_PAIR(3));
            printw(" ");
            attron(A_REVERSE);
        } else {
            mvprintw(x + i, y, "  ");
        }
        mvprintw(x + i, y + 2, " %s ", menu->entries[i]);
        if (i == menu->selected) {
            attroff(A_REVERSE);
            printw(" ");
            addch('<' | COLOR_PAIR(3));
        } else {
            printw("  ");
        }
    }
}
