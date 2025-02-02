#include "game_screen.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>

#include "input.h"
#include "../data/users.h"

void SerializeGame(FILE *file, const Game *game);

void GameScreenInit(GameScreen *self) {
}

int GameScreenHandleInput(void *selfv, int input) {
    GameScreen *self = (GameScreen *)selfv;

    if (input == KEY_F(1)) {
        char filename[100];
        sprintf(filename, "%s-save.data", logged_in_user->username);
        FILE *file = fopen(filename, "wb");
        SerializeGame(file, &game);
        fflush(file);
        fclose(file);

        return 3;
    }

    if (tolower(input) == 'm')
        game.map_revealed = !game.map_revealed;

    if (input == KEY_RESIZE)
        clear();

    if (game.over) {
        if (input == ' ') {
            int new_score = game.player.gold * 2;
            if (new_score > logged_in_user->highscore)
                logged_in_user->highscore = new_score;
            UserManagerFlush(&usermanager);
            return 3;
        }
    } else {
        UpdatePlayer(input);
        UpdateEnemies();
        game.clock++;
    }

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

void TrimString(const char *in, char *out) {
    strcpy(out, in);

    if (strlen(out) > 16) {
        out[13] = '.';
        out[14] = '.';
        out[15] = '.';
        out[16] = 0;
    }
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
    TrimString(game.killer->type->name, buffer);
    mvwprintw(win, 12, (58 - strlen(buffer)) / 2, "%s", buffer);

    if (game.killer->type->id == EnemyType_Undead)
        mvwaddch(win, 11, 34, 'n');

    wnoutrefresh(win);
}

void GameScreenRender(void *selfv) {
    GameScreen *self = (GameScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);

    mvprintw(x - 2, 0, "Press F1 to save and return.");
    UpdateMessageBar(false);

    for (int _i = 1; _i < x - 1; _i++)
        for (int j = 0; j < y; j++) {
            int i = _i;

            if (i < MAXLINES && j < MAXCOLS) {
                Tile *tile = &CURRENT_FLOOR.TILE(i, j);
                mvaddch(i, j, GetTileSprite(tile));
            }
        }

    // Render enemies
    for (int i = 0; i < CURRENT_FLOOR.n_enemies; i++) {
        Enemy *enemy = &CURRENT_FLOOR.enemies[i];

        if (enemy->health && (game.map_revealed || CanSee(&CURRENT_FLOOR, game.player.coord, enemy->coord)))
            mvaddch(enemy->coord.x, enemy->coord.y, enemy->type->sprite);
    }

    mvaddch(game.player.coord.x, game.player.coord.y, '@' | COLOR_PAIR(4));

    attron(COLOR_PAIR(5));
    mvprintw(x - 1, 0, "Gold: ");
    // attron(A_ITALIC);
    printw("%d$     ", game.player.gold);
    attroff(COLOR_PAIR(5) /*| A_ITALIC*/);

    attron(COLOR_PAIR(4));
    mvprintw(x - 1, 13, "Health: %d/%d  ", game.player.health, MAX_HEALTH);
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(1));
    mvprintw(x - 1, 30, "Hunger: %d/%d  ", game.player.hunger, MAX_HUNGER);
    attroff(COLOR_PAIR(1));
    mvprintw(x - 1, 48, "Floor: %d", game.floor_id + 1);
    mvprintw(x - 1, 60, "Equipped: %s         ", game.player.weapons[game.player.equipment].info->name);
    // mvprintw(x - 1, 100, "Foods: %d %d %d %d      ", game.player.foods[0], game.player.foods[1], game.player.foods[2], game.player.foods[3]);

    wnoutrefresh(stdscr);

    if (game.over)
        RenderRIP(x, y);

    doupdate();
}

void GameScreenFree(void *self) {
}
