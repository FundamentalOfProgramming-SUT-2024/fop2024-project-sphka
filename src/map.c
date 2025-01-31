#include "map.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <ncurses.h>

#include "random.h"
#include "data/game.h"

void DoRooms(Floor *floor, Floor *prev);
void DoCorridors(Floor *floor);
void ConnectRooms(Floor *floor, int i, int j);
void DoItems(Floor *floor);

bool IsTilePassable(Coord coord) {
    if (strchr(".+#<>", CURRENT_FLOOR.TILEC(coord).c) == NULL)
        return false;

    if (game.player.coord.x == coord.x && game.player.coord.y == coord.y)
        return false;

    for (int i = 0; i < CURRENT_FLOOR.n_enemies; i++) {
        Enemy *enemy = &CURRENT_FLOOR.enemies[i];
        if (enemy->health && enemy->coord.x == coord.x && enemy->coord.y == coord.y)
            return false;
    }

    return true;
}

static inline int min(int x, int y) {
    return x < y ? x : y;
}

static inline int max(int x, int y) {
    return x > y ? x : y;
}

Room *PickExistingRoom(Floor *floor) {
    Room *room;
    do {
        room = &floor->rooms[randn(9)];
    } while (room->is_gone);

    return room;
}

int GetCoordRoom(Floor *floor, Coord coord);

void GenerateFloor(Floor *floor, Floor *prev) {
    for (int x = 0; x < MAXLINES; x++)
        for (int y = 0; y < MAXCOLS; y++) {
            floor->TILE(x, y).c = ' ';
            floor->TILE(x, y).is_visible = false;
            floor->TILE(x, y).dfs_depth = 10;
            floor->TILE(x, y).has_item = false;
        }

    DoRooms(floor, prev);
    DoCorridors(floor);
    DoItems(floor);

    floor->n_enemies = 10;

    for (int i = 0; i < 10; i++) {
        floor->enemies[i].coord = GetRandomCoord(floor);
        floor->enemies[i].type = &enemies[EnemyType_Deamon];
        floor->enemies[i].health = enemies[EnemyType_Deamon].health;
    }
}

void DoRooms(Floor *floor, Floor *prev) {
    for (int i = 0; i < 9; i++)
        floor->rooms[i].is_gone = 0;

    for (int i = 0; i < randn(4); i++)
        PickExistingRoom(floor)->is_gone = 1;

    int cellh = MAXLINES / 3;
    int cellw = MAXCOLS / 3;
    int up_stairs_room = prev ? GetCoordRoom(prev, prev->down_stairs) : -1;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int idx = i * 3 + j;
            Room *room = &floor->rooms[idx];

            if (idx == up_stairs_room) {
                *room = prev->rooms[idx];
            } else {
                if (room->is_gone) {
                    int x0 = i * cellh + cellh / 3 + randn(cellh / 3);
                    int y0 = j * cellw + cellw / 3 + randn(cellw / 3);

                    room->p0.x = x0;
                    room->p0.y = y0;
                    room->p1.x = x0;
                    room->p1.y = y0;

                    floor->TILE(x0, y0).c = 'x';
                    continue;
                }

                int xs = randn(cellh - 6) + 4;
                int ys = randn(cellw - 6) + 4;
                int x0 = i * cellh + randn(cellh - xs) + 1;
                int y0 = j * cellw + randn(cellw - ys) + 1;
                int x1 = x0 + xs;
                int y1 = y0 + ys;

                room->p0.x = x0;
                room->p0.y = y0;
                room->p1.x = x1;
                room->p1.y = y1;
            }

            room->n_doors = 0;

            int x0 = room->p0.x;
            int y0 = room->p0.y;
            int x1 = room->p1.x;
            int y1 = room->p1.y;

            for (int x = x0; x < x1; x++)
                for (int y = y0; y < y1; y++) {
                    char c = '.';
                    if (y == y0 || y == y1 - 1)
                        c = '|';
                    else if (x == x0 || x == x1 - 1) 
                        c = '-';

                    floor->TILE(x, y).c = c;
                }
        }

    // Put stairs
    Coord down_stairs = GetRandomCoord(floor);
    floor->down_stairs = down_stairs;
    floor->TILEC(down_stairs).c = '<';

    if (prev)
        floor->TILEC(prev->down_stairs).c = '>';
}

struct RoomLUT {
    bool adjacent[9];
    bool is_connected[9];
    bool covered;
} lut[9] = {
    {{0, 1, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    {{1, 0, 1, 0, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    {{0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    {{1, 0, 0, 0, 1, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    {{0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    {{0, 0, 1, 0, 1, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    {{0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    {{0, 0, 0, 0, 1, 0, 1, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    {{0, 0, 0, 0, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
};

void DoCorridors(Floor *floor) {
    for (int i = 0; i < 9; i++) {
        memset(&lut[i].is_connected, 0, 9 * sizeof(bool));
        lut[i].covered = false;
    }

    lut[randn(9)].covered = 1;
    int rooms_covered = 1;
    while (rooms_covered < 9) {
        int r_uncovered, r_covered;

        // Pick a random uncovered room
        do {
            r_uncovered = randn(9);
        } while (lut[r_uncovered].covered);

        int n_candidates = 0, candidates[9];
        for (int i = 0; i < 9; i++) {
            if (lut[r_uncovered].adjacent[i] && lut[i].covered)
                candidates[n_candidates++] = i;
        }

        if (!n_candidates)
            continue;

        r_covered = candidates[randn(n_candidates)];

        ConnectRooms(floor, r_covered, r_uncovered);
        lut[r_uncovered].covered = 1;
        rooms_covered++;
    }

    for (int additional_corridors = randn(5); additional_corridors--; ) {
        int r1 = randn(9);

        int n_candidates = 0, candidates[9];
        for (int i = 0; i < 9; i++) {
            if (lut[r1].adjacent[i] && !lut[r1].is_connected[i])
                candidates[n_candidates++] = i;
        }

        if (n_candidates)
            ConnectRooms(floor, r1, candidates[randn(n_candidates)]);
    }
}

void ConnectRooms(Floor *floor, int i, int j) {
    lut[i].is_connected[j] = 1;
    lut[j].is_connected[i] = 1;

    int imin = min(i, j);
    int imax = max(i, j);

    Room *rmin = &floor->rooms[imin];
    Room *rmax = &floor->rooms[imax];

    Coord door_min, door_max;
    Coord main_axis = {0, 0};
    Coord perp_axis = {0, 0};
    int n_main, n_perp;

    if (imax - imin == 1) {
        main_axis.y = 1;

        if (!rmin->is_gone) {
            door_min.y = rmin->p1.y - 1;
            door_min.x = randn(rmin->p1.x - rmin->p0.x - 2) + rmin->p0.x + 1;
            rmin->doors[rmin->n_doors++] = door_min;
            floor->TILEC(door_min).c = '+';
        } else {
            door_min = rmin->p0;
            floor->TILEC(door_min).c = '#';
        }

        if (!rmax->is_gone) {
            door_max.y = rmax->p0.y;
            door_max.x = randn(rmax->p1.x - rmax->p0.x - 2) + rmax->p0.x + 1;
            rmax->doors[rmax->n_doors++] = door_max;
            floor->TILEC(door_max).c = '+';
        } else {
            door_max = rmax->p0;
            floor->TILEC(door_max).c = '#';
        }

        perp_axis.x = (door_max.x > door_min.x) * 2 - 1;
        n_main = door_max.y - door_min.y;
        n_perp = abs(door_max.x - door_min.x);
    } else {
        main_axis.x = 1;

        if (!rmin->is_gone) {
            door_min.x = rmin->p1.x - 1;
            door_min.y = randn(rmin->p1.y - rmin->p0.y - 2) + rmin->p0.y + 1;
            rmin->doors[rmin->n_doors++] = door_min;
            floor->TILEC(door_min).c = '+';
        } else {
            door_min = rmin->p0;
            floor->TILEC(door_min).c = '#';
        }

        if (!rmax->is_gone) {
            door_max.x = rmax->p0.x;
            door_max.y = randn(rmax->p1.y - rmax->p0.y - 2) + rmax->p0.y + 1;
            rmax->doors[rmax->n_doors++] = door_max;
            floor->TILEC(door_max).c = '+';
        } else {
            door_max = rmax->p0;
            floor->TILEC(door_max).c = '#';
        }

        perp_axis.y = (door_max.y > door_min.y) * 2 - 1;
        n_main = door_max.x - door_min.x;
        n_perp = abs(door_max.y - door_min.y);
    }

    // Draw the actual passage
    int prep_i = randn(n_main - 1) + 1;
    while (n_main-- > 1) {
        door_min.x += main_axis.x;
        door_min.y += main_axis.y;

        if (n_main == prep_i) {
            while (n_perp--) {
                floor->TILEC(door_min).c = '#';

                door_min.x += perp_axis.x;
                door_min.y += perp_axis.y;
            }
        }

        floor->TILEC(door_min).c = '#';
    }
}

Coord GetRandomCoordInRoom(Room *room) {
    int h = room->p1.x - room->p0.x - 2;
    int w = room->p1.y - room->p0.y - 2;

    Coord out = { room->p0.x + 1 + randn(h), room->p0.y + 1 + randn(w) };
    return out;
}

int GetCoordRoom(Floor *floor, Coord coord) {
    for (int i = 0; i < 9; i++) {
        if (!floor->rooms[i].is_gone) {
            bool in_x = floor->rooms[i].p0.x <= coord.x && coord.x <= floor->rooms[i].p1.x - 1;
            bool in_y = floor->rooms[i].p0.y <= coord.y && coord.y <= floor->rooms[i].p1.y - 1;

            if (in_x && in_y)
                return i;
        }
    }

    return -1;
}

Coord GetRandomCoord(Floor *floor) {
    return GetRandomCoordInRoom(PickExistingRoom(floor));
}

void DoItems(Floor *floor) {
    // Put gold pieces
    int items_left = 10;
    while (items_left) {
        Coord coord = GetRandomCoord(floor);
        if (floor->TILEC(coord).has_item || floor->TILEC(coord).c != '.')
            continue;

        floor->TILEC(coord).has_item = true;
        floor->TILEC(coord).item.category = ItemCategory_Gold;
        floor->TILEC(coord).item.info = &gold_item_info;
        floor->TILEC(coord).item.count = randn(6) + 5;

        items_left--;
    }

    // Put weapons
    items_left = 10;
    while (items_left) {
        Coord coord = GetRandomCoord(floor);
        if (floor->TILEC(coord).has_item || floor->TILEC(coord).c != '.')
            continue;

        floor->TILEC(coord).has_item = true;
        floor->TILEC(coord).item.category = ItemCategory_Weapon;
        WeaponType *w_type = &weapons[randn(WeaponTypeCount - 1) + 1];
        floor->TILEC(coord).item.info = &w_type->item_info;
        floor->TILEC(coord).item.ex_weapon.type = w_type;
        floor->TILEC(coord).item.count = w_type->collect_count;

        items_left--;
    }
}

// void DiscoverCorridor(Floor *floor, Coord coord, int depth, int max_depth) {
//     if (depth > max_depth || floor->TILEC(coord).dfs_depth <= depth)
//         return;

//     floor->TILEC(coord).dfs_depth = depth;
//     floor->TILEC(coord).is_visible = true;

// #define CHECK(xo, yo) if (floor->TILE(coord.x + xo, coord.y + yo).c == '#') \
//         DiscoverCorridor(floor, (Coord) {coord.x + xo, coord.y + yo}, depth + 1, max_depth)

//     CHECK(-1, 0);
//     CHECK(1, 0);
//     CHECK(0, 1);
//     CHECK(0, -1);
// }

void DiscoverCorridor(Floor *floor, Coord coord, int max_depth) {
    Coord coord_copy;

#define CHECK(iter) coord_copy = coord;                                             \
    for (int i = 0; i < max_depth; i++) {                                           \
        coord_copy.iter;                                                            \
        if (floor->TILEC(coord_copy).c == '#' || floor->TILEC(coord_copy).c == '+') \
            floor->TILEC(coord_copy).is_visible = true;                             \
        else                                                                        \
            break;                                                                  \
    }

    CHECK(x++)
    CHECK(y++)
    CHECK(x--)
    CHECK(y--)
}

void Discover(Floor *floor, Coord coord) {
    int room_id = GetCoordRoom(floor, coord);

    if (room_id >= 0) {
        Room *room = &floor->rooms[room_id];
        for (int x = room->p0.x; x < room->p1.x; x++)
            for (int y = room->p0.y; y < room->p1.y; y++)
                floor->TILE(x, y).is_visible = true;
    }
    
    if (room_id == -1 || floor->TILEC(coord).c == '+') {
        // Player is on a corridor tile
        // DiscoverCorridor(floor, coord, 0, 3);
        DiscoverCorridor(floor, coord, 5);
    }
}
