#include "map.h"

#include <stdio.h>

#include "random.h"

void ConnectRooms(Floor *floor, int i, int j);
void DoCorridors(Floor *floor);

static inline int min(int x, int y) {
    return x < y ? x : y;
}

static inline int max(int x, int y) {
    return x > y ? x : y;
}

void GenerateFloor(Floor *floor) {
    for (int x = 0; x < MAXLINES; x++)
        for (int y = 0; y < MAXCOLS; y++)
            floor->tiles[x * MAXCOLS + y].c = ' ';

    int cellh = MAXLINES / 3;
    int cellw = MAXCOLS / 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            Room *room = &floor->rooms[i * 3 + j];
            room->n_doors = 0;
            room->is_gone = 0;


            if (randf() < 0.1) {
            // if (i == j && i == 1) {
            // if (0) {
            // {
                room->is_gone = 1;

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

                    // if (
                    //     (x == (x0 + x1) / 2 && (y == y0 || y == y1 - 1)) ||
                    //     (y == (y0 + y1) / 2 && (x == x0 || x == x1 - 1))
                    // )
                    //     c = '+';
                    floor->TILE(x, y).c = c;
                }
        }

    DoCorridors(floor);

#if 0
    floor->tiles[cellh * MAXCOLS + cellw / 2].c = '#';
    floor->tiles[cellh * MAXCOLS + cellw / 2 + cellw].c = '#';
    floor->tiles[cellh * MAXCOLS + cellw / 2 + cellw * 2].c = '#';

    floor->tiles[cellh * 2 * MAXCOLS + cellw / 2].c = '#';
    floor->tiles[cellh * 2 * MAXCOLS + cellw / 2 + cellw].c = '#';
    floor->tiles[cellh * 2 * MAXCOLS + cellw / 2 + cellw * 2].c = '#';

    floor->tiles[(cellh / 2 + cellh * 0) * MAXCOLS + cellw].c = '#';
    floor->tiles[(cellh / 2 + cellh * 0) * MAXCOLS + cellw * 2].c = '#';

    floor->tiles[(cellh / 2 + cellh * 1) * MAXCOLS + cellw].c = '#';
    floor->tiles[(cellh / 2 + cellh * 1) * MAXCOLS + cellw * 2].c = '#';

    floor->tiles[(cellh / 2 + cellh * 2) * MAXCOLS + cellw].c = '#';
    floor->tiles[(cellh / 2 + cellh * 2) * MAXCOLS + cellw * 2].c = '#';
#endif
}

typedef struct {
    char adjacent[9];
    char is_connected[9];
    char covered;
} RoomLUT;

void DoCorridors(Floor *floor) {
    static RoomLUT lut[9] = {
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

    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (lut[i].adjacent[j] && !lut[i].is_connected[j]) {
                lut[i].is_connected[j] = 1;
                lut[j].is_connected[i] = 1;
                ConnectRooms(floor, i, j);
            }
}

void ConnectRooms(Floor *floor, int i, int j) {
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
            floor->TILE(door_min.x, door_min.y).c = '#';
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
            floor->TILE(door_min.x, door_min.y).c = '#';
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
