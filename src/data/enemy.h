#pragma once

#include <stdint.h>

#include "../base.h"

typedef enum EnemyTypeID {
    EnemyType_Deamon,

    EnemyTypeCount
} EnemyTypeID;

typedef struct EnemyType {
    EnemyTypeID id;

    uint32_t sprite;
    const char *name;

    int health;
    int damage;
} EnemyType;

typedef struct Enemy {
    Coord coord;

    EnemyType *type;
    int health;
} Enemy;

extern EnemyType enemies[EnemyTypeCount];

void EnemyUpdate(Enemy *enemy);
