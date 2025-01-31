#pragma once

#include "../base.h"
#include "../map.h"
#include "item.h"

#define FLOOR_COUNT 4
#define CURRENT_FLOOR game.floors[game.floor_id]

typedef struct Player {
    Coord coord;

    Item weapons[5];
    int n_weapons;

    int gold;

    // 0 - 20
    int hunger;

    // 0 - 20
    int health;
} Player;

typedef struct Game {
    Player player;

    int floor_id;
    Floor floors[FLOOR_COUNT];
} Game;

extern Game game;
extern char g_message_bar[500];

void NewGame();
bool Pickup(Item *item);
