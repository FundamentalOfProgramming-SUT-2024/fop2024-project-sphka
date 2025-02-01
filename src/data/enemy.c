#include "enemy.h"

#include <ncurses.h>

#include "game.h"

EnemyType enemies[EnemyTypeCount] = {
    {
        .id = EnemyType_Deamon,
        .sprite = 'D' | COLOR_PAIR(6),
        .name = "Deamon",
        .health = 5,
        .damage = 3,
    }
};

void EnemyUpdate(Enemy *enemy) {
    if (enemy->health == 0 || !CanSee(&CURRENT_FLOOR, enemy->coord, game.player.prev_coord))
        return;

    Coord best_coord = enemy->coord;
    int best_dist = SqDistance(best_coord, game.player.coord);

    // Try to attack the player before moving to allow her to dodge
    if (best_dist <= 2)
        Damage(enemy);


    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0)
                continue;

            Coord coord = (Coord) { enemy->coord.x + x, enemy->coord.y + y };
            if (!IsTilePassable(coord, NULL))
                continue;

            int new_dist = SqDistance(coord, game.player.coord);
            if (new_dist < best_dist) {
                best_dist = new_dist;
                best_coord = coord;
            }
        }
    }

    enemy->coord = best_coord;
}
