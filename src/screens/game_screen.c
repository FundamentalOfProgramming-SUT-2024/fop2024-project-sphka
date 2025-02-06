#include "game_screen.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <ncurses.h>

#include "input.h"
#include "../data/users.h"

void SerializeGame(FILE *file, const Game *game);

void GameScreenInit(GameScreen *self) {
}

int GameScreenHandleInput(void *selfv, int input) {
    // GameScreen *self = (GameScreen *)selfv;

    if (input == KEY_F(1)) {
        char filename[100];
        sprintf(filename, "%s-save.data", logged_in_user->username);
        FILE *file = fopen(filename, "wb");
        SerializeGame(file, &game);
        fflush(file);
        fclose(file);

        logged_in_user->has_save = true;

        if (logged_in_user->first_game_time == 0)
            logged_in_user->first_game_time = time(NULL);

        UserManagerFlush(&usermanager);

        return 3;
    }

    if (tolower(input) == 'm') {
        game.map_revealed = !game.map_revealed;
        return -1;
    }

    if (tolower(input) == 'g' && FastMove())
        return -1;

    if (input == '.') {
        game.skip_next_pickup = true;
        strcpy(g_message_bar, "You wont pickup anything in the next turn.");
        return -1;
    } else if (game.skip_next_pickup) {
        strcpy(g_message_bar, "");
    }

    if (input == KEY_RESIZE) {
        clear();
        return -1;
    }

    if (game.over) {
        if (input == ' ') {
            int score = game.player.gold * 2 + game.player.kills * 5;
            if (game.won)
                score += 1000;

            logged_in_user->score_sum += score;
            logged_in_user->gold_sum += game.player.gold;
            logged_in_user->game_count++;

            if (logged_in_user->first_game_time == 0)
                logged_in_user->first_game_time = time(NULL);

            UserManagerFlush(&usermanager);
            return 3;
        }
    } else {
        UpdatePlayer(input);

        if (game.player.buffs[PotionType_Speed] == 0 || EVERY(2)) {
            UpdateEnemies();

            // "Fade" buffs
            if (game.player.health_buff) game.player.health_buff--;
            if (game.player.speed_buff) game.player.speed_buff--;
            if (game.player.damage_buff) game.player.damage_buff--;
        }

        game.clock++;
    }

    game.skip_next_pickup = false;
    return -1;
}

static char *rip[] = {"                       __________",
                      "                      /          \\",
                      "                     /    REST    \\",
                      "                    /      IN      \\",
                      "                   /     PEACE      \\",
                      "                  /                  \\",
                      "                  |                  |",
                      "                  |                  |",
                      "                  |   killed by a    |",
                      "                  |                  |",
                      "                  |       1980       |",
                      "                 *|     *  *  *      | *",
                      "         ________)/\\\\_//(\\/(/\\)/\\//\\/|_)_______",
                      "",
                      "",
                      "                 Press space to continue",
                      0};


static char *trophy[] = {
    "                        ___________",
    "                       '._==_==_=_.'",
    "                       .-\\:      /-.",
    "                      | (|:.     |) |",
    "                       '-|:.     |-'",
    "                         \\::.    /",
    "                          '::. .'",
    "                            ) (",
    "                          _.' '._",
    "                         `\"\"\"\"\"\"\"`",
    "",
    "",
    "                         YOU  WON!",
    "",
    "                  Press space to continue",
    0
};


void TrimString(const char *in, char *out) {
    strcpy(out, in);

    if (strlen(out) > 16) {
        out[13] = '.';
        out[14] = '.';
        out[15] = '.';
        out[16] = 0;
    }
}

void RenderYouWon(int x, int y) {
    static WINDOW *win = NULL;

    // if (!win)
    if (win)
        delwin(win);

    win = newwin(22, 60, (x - 22) / 2, (y - 60) / 2);

    box(win, 0, 0);

    int i = 0;
    wattron(win, COLOR_PAIR(5));
    for (char **trophy_ptr = trophy; *trophy_ptr; trophy_ptr++) {
        if (i == 14)
            wattroff(win, COLOR_PAIR(5));
        mvwprintw(win, i + 3, 1, "%s", *trophy_ptr);
        i++;
    }
    // wattroff(win, COLOR_PAIR(5));

    wnoutrefresh(win);
}

void RenderRIP(int x, int y) {
    static WINDOW *win = NULL;

    // if (!win)
    if (win)
        delwin(win);

    win = newwin(22, 60, (x - 22) / 2, (y - 60) / 2);

    box(win, 0, 0);

    int i = 0;
    for (char **rip_ptr = rip; *rip_ptr; rip_ptr++) {
        if (i == 12)
            wattron(win, COLOR_PAIR(4));
        mvwprintw(win, i + 3, 1, "%s", *rip_ptr);
        if (i == 12)
            wattroff(win, COLOR_PAIR(4));
        i++;
    }

    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 14, 18, "*");
    mvwprintw(win, 14, 25, "*  *  *");
    mvwprintw(win, 14, 40, "*");
    wattroff(win, COLOR_PAIR(1));

    mvwprintw(win, 15, 19, "/");
    mvwprintw(win, 15, 38, "|");

    char buffer[55];
    TrimString(logged_in_user->username, buffer);
    mvwprintw(win, 9, (58 - strlen(buffer)) / 2, "%s", buffer);

    if (game.killer == 0)
        TrimString("p. of Rotten Food", buffer);
    else if (game.killer == (Enemy *)1) {
        mvwaddch(win, 11, 33, ' ');
        TrimString("ENCHANTMENT", buffer);
    }
    else if (game.killer == (Enemy *)2) {
        mvwaddch(win, 11, 33, ' ');
        TrimString("HUNGER", buffer);
    }
    else {
        if (game.killer->type->id == EnemyType_Undead)
            mvwaddch(win, 11, 34, 'n');
        TrimString(game.killer->type->name, buffer);
    }

    mvwprintw(win, 12, (58 - strlen(buffer)) / 2, "%s", buffer);
    wnoutrefresh(win);
}

void GameScreenRender(void *selfv) {
    // GameScreen *self = (GameScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);

    // mvprintw(x - 3, 0, "Press F1 to save and return.");
    UpdateMessageBar(false);
    RenderMap(x, y);

    if (game.over) {
        if (game.won)
            RenderYouWon(x, y);
        else
            RenderRIP(x, y);
    }

    doupdate();
}

void GameScreenFree(void *self) {
}
