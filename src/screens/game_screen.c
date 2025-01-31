#include "game_screen.h"

#include <stdlib.h>
#include <ncurses.h>

#include "input.h"
#include "../dialogs/weapon_selector.h"

void GameScreenInit(GameScreen *self) {
    NewGame();
}

int GameScreenHandleInput(void *selfv, int input) {
    GameScreen *self = (GameScreen *)selfv;

    if (input == KEY_RESIZE)
        clear();

    Coord next_player_coord = game.player.coord;
    Coord delta = InputDirection(input);

    next_player_coord.x += delta.x;
    next_player_coord.y += delta.y;

    // if (input == 'r') {
    //     GenerateFloor(self->floor);
    //     self->player = GetRandomCoord(self->floor);
    // }

    if (CURRENT_FLOOR.TILEC(game.player.coord).c == '<' && input == '<' && game.floor_id < FLOOR_COUNT - 1) {
        game.floor_id++;
    }

    if (CURRENT_FLOOR.TILEC(game.player.coord).c == '>' && input == '>' && game.floor_id) {
        game.floor_id--;
    }

    if (input == 'i')
        WeaponSelector();

    if (IsTilePassable(next_player_coord)) {
        game.player.coord = next_player_coord;
    }

    Discover(&CURRENT_FLOOR, game.player.coord);

    // Pickup item
    if (CURRENT_FLOOR.TILEC(game.player.coord).has_item) {
        Item *to_pickup = &CURRENT_FLOOR.TILEC(game.player.coord).item;
        CURRENT_FLOOR.TILEC(game.player.coord).has_item = !Pickup(to_pickup);
    }

    for (int i = 0; i < CURRENT_FLOOR.n_enemies; i++) {
        Enemy *enemy = &CURRENT_FLOOR.enemies[i];

        if (enemy->health)
            EnemyUpdate(enemy);
    }

    if (input == ' ') {
        // Attack
        for (int i = 0; i < CURRENT_FLOOR.n_enemies; i++) {
            Enemy *enemy = &CURRENT_FLOOR.enemies[i];
            if (enemy->health && SqDistance(enemy->coord, game.player.coord) <= 2) {
                enemy->health -= 2;
                if (enemy->health < 0)
                    enemy->health = 0;
            }
        }
    }

    return -1;
}

void GameScreenRender(void *selfv) {
    GameScreen *self = (GameScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);

    for (int i = 0; i < y; i++)
        mvaddch(0, i, ' ');

    mvprintw(0, 0, "%s", g_message_bar);

    for (int _i = 1; _i < x - 1; _i++)
        for (int j = 0; j < y; j++) {
            int i = _i;

            if (i < MAXLINES && j < MAXCOLS) {
                Tile *tile = &CURRENT_FLOOR.TILE(i, j);
                mvaddch(i, j, GetTileSprite(tile));
            }
        }

    for (int i = 0; i < CURRENT_FLOOR.n_enemies; i++) {
        Enemy *enemy = &CURRENT_FLOOR.enemies[i];

        if (enemy->health)
            mvaddch(enemy->coord.x, enemy->coord.y, enemy->type->sprite);
    }

    mvaddch(game.player.coord.x, game.player.coord.y, '@' | COLOR_PAIR(4));


    attron(COLOR_PAIR(5));
    mvprintw(x - 1, 0, "Gold: ");
    // attron(A_ITALIC);
    printw("%d$     ", game.player.gold);
    attroff(COLOR_PAIR(5) /*| A_ITALIC*/);
}

void GameScreenFree(void *self) {
}
