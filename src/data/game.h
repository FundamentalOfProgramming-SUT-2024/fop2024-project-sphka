#pragma once

#include "../base.h"
#include "../map.h"
#include "item.h"

#define FLOOR_COUNT 4
#define CURRENT_FLOOR game.floors[game.floor_id]
#define CURRENT_WEAPON game.player.weapons[game.player.equipment]

#define MAX_HEALTH 50
#define MAX_HUNGER 50

typedef struct Player {
    Coord coord;
    Coord prev_coord;

    Item weapons[5];
    int n_weapons;
    int equipment;

    int gold;

    int health;
    int hunger;
} Player;

typedef struct Game {
    Player player;

    int floor_id;
    Floor floors[FLOOR_COUNT];

    bool over;
    Enemy *killer;
} Game;

extern Game game;

extern char g_message_bar[500];
void UpdateMessageBar(bool refresh);

void NewGame();

void UpdatePlayer(int input);
void UpdateEnemies();
void Damage(Enemy *enemy);
