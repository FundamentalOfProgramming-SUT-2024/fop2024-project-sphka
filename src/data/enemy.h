#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "../base.h"

typedef enum EnemyTypeID {
    EnemyType_Demon,
    EnemyType_FireBreathingMonster,
    EnemyType_Giant,
    EnemyType_Snake,
    EnemyType_Undead,

    EnemyTypeCount
} EnemyTypeID;

typedef struct EnemyType {
    EnemyTypeID id;

    uint32_t sprite;
    const char *name;

    int health;
    int damage;

    // 0 for unlimited
    int movement_limit;
    bool can_leave_rooms;
} EnemyType;

typedef struct Enemy {
    Coord coord;

    int health;
    int movement_left;

    bool active; // Only for Undead
    EnemyType *type;
} Enemy;

extern EnemyType enemies[EnemyTypeCount];

void EnemyUpdate(Enemy *enemy);
