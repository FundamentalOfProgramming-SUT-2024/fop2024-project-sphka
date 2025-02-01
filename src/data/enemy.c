#include "enemy.h"

#include <ncurses.h>

#include "game.h"

EnemyType enemies[EnemyTypeCount] = {
    {
        .id = EnemyType_Demon,
        .sprite = 'D' | COLOR_PAIR(6),
        .name = "Demon",
        .health = 5,
        .damage = 3,
        .movement_limit = 8,
        .can_leave_rooms = false
    },
    {
        .id = EnemyType_FireBreathingMonster,
        .sprite = 'F' | COLOR_PAIR(6),
        .name = "Fire Breathing Monster",
        .health = 10,
        .damage = 5,
        .movement_limit = 12,
        .can_leave_rooms = false
    },
    {
        .id = EnemyType_Giant,
        .sprite = 'G' | COLOR_PAIR(6),
        .name = "Giant",
        .health = 15,
        .damage = 10,
        .movement_limit = 5,
        .can_leave_rooms = false
    },
    {
        .id = EnemyType_Snake,
        .sprite = 'S' | COLOR_PAIR(6),
        .name = "Snake",
        .health = 20,
        .damage = 15,
        .movement_limit = -1,
        .can_leave_rooms = true
    },
    {
        .id = EnemyType_Undead,
        .sprite = 'U' | COLOR_PAIR(6),
        .name = "Undead",
        .health = 30,
        .damage = 20,
        .movement_limit = 5,
        .can_leave_rooms = false
    },
};

void EnemyUpdate(Enemy *enemy) {
    if (enemy->health == 0 || !CanSee(&CURRENT_FLOOR, enemy->coord, game.player.prev_coord))
        return;

    Coord best_coord = enemy->coord;
    int best_dist = SqDistance(best_coord, game.player.coord);

    // Try to attack the player before moving to allow her to dodge
    if (best_dist <= 2) {
        if (enemy->type->id == EnemyType_Undead)
            enemy->active = true;
        Damage(enemy);
    }

    if (enemy->movement_left != 0 && (enemy->type->id != EnemyType_Undead || enemy->active)) {
        bool wants_to_move = false;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                if (x == 0 && y == 0)
                    continue;

                Coord coord = (Coord) { enemy->coord.x + x, enemy->coord.y + y };
                if (!IsTilePassable(coord, NULL))
                    continue;

                if (!enemy->type->can_leave_rooms && CURRENT_FLOOR.TILEC(coord).c == '+')
                    continue;

                int new_dist = SqDistance(coord, game.player.coord);
                if (new_dist < best_dist) {
                    best_dist = new_dist;
                    best_coord = coord;
                    wants_to_move = true;
                }
            }
        }

        enemy->coord = best_coord;
        if (wants_to_move)
            enemy->movement_left--;
    }

}
