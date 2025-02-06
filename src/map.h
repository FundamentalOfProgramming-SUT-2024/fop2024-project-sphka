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
    // uint8_t dfs_depth;

    struct {
        uint8_t is_visible : 1;
        uint8_t has_item : 1;
        uint8_t hidden_door_axis : 1;
        uint8_t hidden_door_discovered : 1;
        uint8_t in_enchant_room;
    };

    Item item;
} Tile;

uint32_t GetTileSprite(Tile *tile);
bool IsTilePassable(Coord coord, Enemy **enemy_out);

typedef struct {
    Coord p0, p1;

    bool is_gone;

    int n_doors;
    struct Door {
        Coord coord;
        uint8_t axis;
        int other_end;
    } doors[4];

    enum RoomTheme {
        RoomTheme_Normal,
        RoomTheme_Enchant,
    } theme;
} Room;

#define MAXLINES 32
#define MAXCOLS  80
#define TILE(x, y) tiles[(x) * MAXCOLS + (y)]
#define TILEC(c) TILE(c.x, c.y)
typedef struct {
    Tile tiles[MAXLINES * MAXCOLS];

    Room rooms[9];
    Coord down_stairs;

    int n_enemies;
    Enemy enemies[10];
} Floor;

void GenerateFloor(Floor *floor, Floor *prev);
void GenerateTreasureRoom(Floor *floor);
int GetCoordRoom(Floor *floor, Coord coord);
Coord GetRandomCoordInRoom(Room *room);
Coord GetRandomCoord(Floor *floor);
void Discover(Floor *floor, Coord coord);

bool CanSee(Floor *floor, Coord a, Coord b);

void RenderMap(int x, int y);
