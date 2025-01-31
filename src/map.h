#pragma once

#include "base.h"
#include "data/item.h"
#include "data/enemy.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ncurses.h>

#define CURRENT_FLOOR game.floors[game.floor_id]

// #define TILE_FLOOR      '.'
// #define TILE_WALL_V     '|'
// #define TILE_WALL_H     '-'
// #define TILE_DOOR       '+'
// #define TILE_STAIR_DOWN '<'
// #define TILE_STAIR_UP   '>'

typedef struct {
    char c;
    bool is_visible;
    uint8_t dfs_depth;

    bool has_item;
    Item item;
} Tile;

static inline uint32_t GetTileCharFlags(char c) {
    int flags = 0;
    switch (c)
    {
    case '#': flags = COLOR_PAIR(1); break;
    case '+': case '-': case '|':
        flags = COLOR_PAIR(2); break;
    case '<': case '>':
        flags = COLOR_PAIR(4) | A_REVERSE; break;
    case 'O': flags = COLOR_PAIR(3); break;
    case '=': flags = COLOR_PAIR(3); break;
    default:
        break;
    }

    return flags;
}

static inline uint32_t GetTileSprite(Tile *tile) {
    if (tile->is_visible) {
        if (tile->has_item)
            return tile->item.info->sprite;
        else
            return tile->c | GetTileCharFlags(tile->c);
    } else {
        return ' ';
    }
}

bool IsTilePassable(Coord coord);

typedef struct {
    Coord p0, p1;

    bool is_gone;

    int n_doors;
    Coord doors[4];
} Room;

#define MAXLINES 32
#define MAXCOLS  80
#define TILE(x, y) tiles[(x) * MAXCOLS + (y)]
#define TILEC(c) TILE(c.x, c.y)
typedef struct {
    Tile tiles[MAXLINES * MAXCOLS];

    Room rooms[9];
    Coord down_stairs;

    Enemy enemies[10];
    int n_enemies;
} Floor;

void GenerateFloor(Floor *floor, Floor *prev);
Coord GetRandomCoordInRoom(Room *room);
Coord GetRandomCoord(Floor *floor);
void Discover(Floor *floor, Coord coord);
