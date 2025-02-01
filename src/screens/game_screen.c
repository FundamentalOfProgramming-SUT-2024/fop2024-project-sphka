#include "game_screen.h"

#include <stdlib.h>
#include <ncurses.h>

#include "input.h"
#include "../data/users.h"

void GameScreenInit(GameScreen *self) {
}

int GameScreenHandleInput(void *selfv, int input) {
    GameScreen *self = (GameScreen *)selfv;

    if (input == KEY_RESIZE)
        clear();

    if (!game.over) {
        UpdatePlayer(input);
        UpdateEnemies();
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
                      0};

void RenderRIP(int x, int y) {
    static WINDOW *win = NULL;

    // if (!win)
    if (win)
        delwin(win);

    win = newwin(20, 60, (x - 20) / 2, (y - 60) / 2);

    box(win, 0, 0);

    int i = 0;
    for (char **rip_ptr = rip; *rip_ptr; rip_ptr++) {
        if (i == 12)
            wattron(win, COLOR_PAIR(4));
        mvwprintw(win, i + 3, 1, *rip_ptr);
        if (i == 12)
            wattroff(win, COLOR_PAIR(4));
        i++;
    }

    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 14, 18, "*");
    mvwprintw(win, 14, 25, "*  *  *");
    mvwprintw(win, 14, 40, "*");
    wattroff(win, COLOR_PAIR(1));

    mvwprintw(win, 9, (58 - strlen(logged_in_user->username)) / 2, "%s", logged_in_user->username);
    mvwprintw(win, 12, (58 - strlen(game.killer->type->name)) / 2, "%s", game.killer->type->name);

    wnoutrefresh(win);
}

void GameScreenRender(void *selfv) {
    GameScreen *self = (GameScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);
    
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

        if (enemy->health && CanSee(&CURRENT_FLOOR, game.player.coord, enemy->coord))
            mvaddch(enemy->coord.x, enemy->coord.y, enemy->type->sprite);
    }

    mvaddch(game.player.coord.x, game.player.coord.y, '@' | COLOR_PAIR(4));

    attron(COLOR_PAIR(5));
    mvprintw(x - 1, 0, "Gold: ");
    // attron(A_ITALIC);
    printw("%d$     ", game.player.gold);
    attroff(COLOR_PAIR(5) /*| A_ITALIC*/);

    attron(COLOR_PAIR(4));
    mvprintw(x - 1, 20, "Health: %d/20  ", game.player.health);
    attroff(COLOR_PAIR(4));

    mvprintw(x - 1, 40, "Satiety: %d/20  ", game.player.hunger);
    mvprintw(x - 1, 60, "Floor: %d", game.floor_id + 1);
    mvprintw(x - 1, 80, "Equipped: %s         ", game.player.weapons[game.player.equipment].info->name);

    wnoutrefresh(stdscr);

    if (game.over)
        RenderRIP(x, y);

    doupdate();
}

void GameScreenFree(void *self) {
}
