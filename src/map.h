#include "base.h"

#include <stdbool.h>

typedef struct {
    char c;
} Tile;

typedef struct {
    Coord p0, p1;

    bool is_gone;

    int n_doors;
    Coord doors[4];
} Room;

#define MAXLINES 32
#define MAXCOLS  80
#define TILE(x, y) tiles[x * MAXCOLS + y]
#define TILEC(c) tiles[c.x * MAXCOLS + c.y]
typedef struct {
    Tile tiles[MAXLINES * MAXCOLS];

    Room rooms[9];
} Floor;

void GenerateFloor(Floor *floor);
Coord GetRandomCoordInRoom(Room *room);
Coord GetRandomCoord(Floor *floor);
