#include "settings_screen.h"
#include "data/users.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ncurses.h>

#include "../settings.h"
#include "../music.h"

void SettingsScreenInit(SettingsScreen *self) {
    self->focus = 0;
}

void SettingsScreenHandleSwitch(void *selfv) {
    // SettingsScreen *self = (SettingsScreen *)selfv;
}

int SettingsScreenHandleInput(void *selfv, int input) {
    if (input == KEY_RESIZE)
        clear();

    if (input == KEY_F(1)) {
        return 3;
    }

    SettingsScreen *self = (SettingsScreen *)selfv;

    if (input == '\n') {
        if (self->focus == 0) {
            if (g_settings.enable_music)
                DisableMusic();
            else
                EnableMusic();

            g_settings.enable_music = !g_settings.enable_music;
        }
    }

    if (self->focus == 1) {
        if (input == KEY_RIGHT) {
            g_settings.player_color++;
        } else if (input == KEY_LEFT) {
            g_settings.player_color--;
        }

        g_settings.player_color = ((g_settings.player_color - 1) % 6) + 1;
    }

    if (self->focus == 2) {
        if (input == KEY_RIGHT) {
            g_settings.difficulty++;
        } else if (input == KEY_LEFT) {
            g_settings.difficulty--;
        }

        g_settings.difficulty = g_settings.difficulty % 3;
    }

    if (input == KEY_DOWN && self->focus < 3 - 1) {
        self->focus++;
    } else if (input == KEY_UP && self->focus > 0) {
        self->focus--;
    }

    return -1;
}

void RenderMusicToogle(SettingsScreen *self, int x, int y) {
    mvaddch(x / 2 - 3, (y - 22) / 2 - 2, (self->focus == 0 ? '>' : ' ') | COLOR_PAIR(3));

    if (self->focus == 0) {
        attron(A_REVERSE);
    }

    mvprintw(x / 2 - 3, (y - 22) / 2, "  Enable Music  ");

    if (self->focus == 0) {
        attroff(A_REVERSE);
    }

    printw("  [");

    attron(COLOR_PAIR(3));
    printw(g_settings.enable_music ? "x" : " ");
    attroff(COLOR_PAIR(3));
    printw("]  ");
}

void RenderPlayerColorChooser(SettingsScreen *self, int x, int y) {
    mvaddch(x / 2 - 2, (y - 22) / 2 - 2, (self->focus == 1 ? '>' : ' ') | COLOR_PAIR(3));

    if (self->focus == 1) {
        attron(A_REVERSE);
    }

    mvprintw(x / 2 - 2, (y - 22) / 2, "  Player Color  ");

    if (self->focus == 1) {
        attroff(A_REVERSE);
    }

    printw("  < ");

    attron(COLOR_PAIR(g_settings.player_color));
    printw("%s", player_colors[g_settings.player_color]);
    attroff(COLOR_PAIR(g_settings.player_color));
    printw(" >      ");
}

void RenderDifficultyChooser(SettingsScreen *self, int x, int y) {
    mvaddch(x / 2 - 1, (y - 22) / 2 - 2, (self->focus == 2 ? '>' : ' ') | COLOR_PAIR(3));

    if (self->focus == 2) {
        attron(A_REVERSE);
    }

    mvprintw(x / 2 - 1, (y - 22) / 2, "  Difficulty    ");

    if (self->focus == 2) {
        attroff(A_REVERSE);
    }

    printw("  < ");

    // attron(COLOR_PAIR(g_settings.player_color));
    printw("%s", difficulties[g_settings.difficulty]);
    // attroff(COLOR_PAIR(g_settings.player_color));
    printw(" >      ");
}

void SettingsScreenRender(void *selfv) {
    SettingsScreen *self = (SettingsScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);
    mvprintw(x / 2 - 6, (y - 18) / 2, "   - ");
    attron(A_ITALIC | A_BOLD);
    printw("Settings");
    attroff(A_ITALIC | A_BOLD);
    printw(" -   ");

    move(x - 1, 0);
    deleteln();
    printw("Exit: ");
    attron(A_ITALIC);
    printw("<F1>   ");
    attroff(A_ITALIC);

    RenderMusicToogle(self, x, y);
    RenderPlayerColorChooser(self, x, y);
    RenderDifficultyChooser(self, x, y);

    refresh();
}

void SettingsScreenFree(void *selfv) {
    // SettingsScreen *self = (SettingsScreen *)selfv;
}
