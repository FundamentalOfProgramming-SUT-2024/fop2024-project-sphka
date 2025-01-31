#pragma once

#include "map.h"
#include "../base.h"
#include "../data/game.h"

typedef struct {
} GameScreen;

void GameScreenInit(GameScreen *self);
int GameScreenHandleInput(void *selfv, int input);
void GameScreenRender(void *selfv);
void GameScreenFree(void *self);
