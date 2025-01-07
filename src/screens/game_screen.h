#pragma once

#include "map.h"

typedef struct {
    Coord player;
    Floor floor;
} GameScreen;

void GameScreenInit(GameScreen *self);
int GameScreenHandleInput(void *selfv, int input);
void GameScreenRender(void *selfv);
void GameScreenFree(void *self);
