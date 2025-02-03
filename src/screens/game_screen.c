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

    if (tolower(input) == 'm') {
        game.map_revealed = !game.map_revealed;
        return -1;
    }

    if (input == KEY_RESIZE) {
        clear();
        return -1;
    }

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

        if (game.player.buffs[PotionType_Speed] == 0 || EVERY(2)) {
            UpdateEnemies();

            // "Fade" buffs
            if (game.player.health_buff) game.player.health_buff--;
            if (game.player.speed_buff) game.player.speed_buff--;
            if (game.player.damage_buff) game.player.damage_buff--;
        }

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
    TrimString(game.killer ? game.killer->type->name : "p. of Rotten Food", buffer);
    mvwprintw(win, 12, (58 - strlen(buffer)) / 2, "%s", buffer);

    if (game.killer && game.killer->type->id == EnemyType_Undead)
        mvwaddch(win, 11, 34, 'n');

    wnoutrefresh(win);
}

void GameScreenRender(void *selfv) {
    GameScreen *self = (GameScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);

    // mvprintw(x - 3, 0, "Press F1 to save and return.");
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
    mvprintw(x - 2, 0, "Gold: ");
    // attron(A_ITALIC);
    printw("%d$     ", game.player.gold);
    attroff(COLOR_PAIR(5) /*| A_ITALIC*/);

    attron(COLOR_PAIR(4));
    mvprintw(x - 2, 13, "Health: %d/%d  ", game.player.health, MAX_HEALTH);
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(1));
    mvprintw(x - 2, 30, "Hunger: %d/%d  ", game.player.hunger, MAX_HUNGER);
    attroff(COLOR_PAIR(1));
    mvprintw(x - 2, 48, "Floor: %d", game.floor_id + 1);
    mvprintw(x - 2, 60, "Equipped: %s         ", game.player.weapons[game.player.equipment].info->name);
    // mvprintw(x - 2, 100, "Foods: %d %d %d %d      ", game.player.foods[0], game.player.foods[1], game.player.foods[2], game.player.foods[3]);

    move(x - 1, 0);
    deleteln();
    printw("Exit: ");
    attron(A_ITALIC);
    printw("<F1>   ");
    attroff(A_ITALIC);

    printw("Buffs: ");
    bool has_any_buffs = false;

    if (game.player.health_buff) {
        attron(COLOR_PAIR(4));
        printw("Health: %d moves  ", game.player.health_buff);
        attroff(COLOR_PAIR(4));
        has_any_buffs = true;
    }

    if (game.player.speed_buff) {
        attron(COLOR_PAIR(6));
        printw("Speed: %d moves  ", game.player.speed_buff);
        attroff(COLOR_PAIR(6));
        has_any_buffs = true;
    }

    if (game.player.damage_buff) {
        attron(COLOR_PAIR(1));
        printw("Damage: %d moves  ", game.player.damage_buff);
        attroff(COLOR_PAIR(1));
        has_any_buffs = true;
    }

    if (!has_any_buffs) {
        attron(A_ITALIC);
        printw("None");
        attroff(A_ITALIC);
    }

    wnoutrefresh(stdscr);

    if (game.over)
        RenderRIP(x, y);

    doupdate();
}

void GameScreenFree(void *self) {
}
