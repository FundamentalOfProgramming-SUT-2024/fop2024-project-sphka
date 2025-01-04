#include "map.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "random.h"

void ConnectRooms(Floor *floor, int i, int j);
void DoCorridors(Floor *floor);

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

void GenerateFloor(Floor *floor) {
    for (int x = 0; x < MAXLINES; x++)
        for (int y = 0; y < MAXCOLS; y++)
            floor->TILE(x, y).c = ' ';
    
    for (int i = 0; i < 9; i++)
        floor->rooms[i].is_gone = 0;

    for (int i = 0; i < randn(4); i++)
        PickExistingRoom(floor)->is_gone = 1;

    int cellh = MAXLINES / 3;
    int cellw = MAXCOLS / 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            Room *room = &floor->rooms[i * 3 + j];
            room->n_doors = 0;


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

            // continue;

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

    DoCorridors(floor);
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
            floor->TILE(door_min.x, door_min.y).c = '+';
        } else {
            door_min = rmin->p0;
            floor->TILE(door_min.x, door_min.y).c = '#';
        }

        if (!rmax->is_gone) {
            door_max.y = rmax->p0.y;
            door_max.x = randn(rmax->p1.x - rmax->p0.x - 2) + rmax->p0.x + 1;
            rmax->doors[rmax->n_doors++] = door_max;
            floor->TILE(door_max.x, door_max.y).c = '+';
        } else {
            door_max = rmax->p0;
            floor->TILE(door_max.x, door_max.y).c = '#';
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
            floor->TILE(door_min.x, door_min.y).c = '+';
        } else {
            door_min = rmin->p0;
            floor->TILE(door_min.x, door_min.y).c = '#';
        }

        if (!rmax->is_gone) {
            door_max.x = rmax->p0.x;
            door_max.y = randn(rmax->p1.y - rmax->p0.y - 2) + rmax->p0.y + 1;
            rmax->doors[rmax->n_doors++] = door_max;
            floor->TILE(door_max.x, door_max.y).c = '+';
        } else {
            door_max = rmax->p0;
            floor->TILE(door_max.x, door_max.y).c = '#';
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
                floor->TILE(door_min.x, door_min.y).c = '#';

                door_min.x += perp_axis.x;
                door_min.y += perp_axis.y;
            }
        }

        floor->TILE(door_min.x, door_min.y).c = '#';
    }
}

Coord GetRandomCoordInRoom(Room *room) {
    int h = room->p1.x - room->p0.x - 2;
    int w = room->p1.y - room->p0.y - 2;

    Coord out = { room->p0.x + 1 + randn(h), room->p0.y + 1 + randn(w) };
    return out;
}

Coord GetRandomCoord(Floor *floor) {
    return GetRandomCoordInRoom(PickExistingRoom(floor));
}
