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

    int n_weapons;
    Item weapons[5];
    int equipment;
    union {
        int foods[4];
        struct {
            int normal_food;
            int supreme_food;
            int magical_food;
            int rotten_food;
        };
    };

    union {
        int potions[4];
        struct {
            int health_potion;
            int speed_potion;
            int damage_potion;
        };
    };

    union {
        int buffs[4];
        struct {
            int health_buff;
            int speed_buff;
            int damage_buff;
        };
    };

    int gold;
    int kills;

    int health;
    int hunger;
    uint8_t anchor;
} Player;

#define EVERY(n) ((game.clock + 1) % n == 0)
typedef struct Game {
    Player player;
    unsigned clock;
    bool skip_next_pickup;

    int floor_id;
    Floor floors[FLOOR_COUNT];
    bool map_revealed;

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

void ConsumeFood(FoodType type);
void ConsumePotion(PotionType type);
bool FastMove();
