#pragma once

#include "widgets/menu.h"

typedef struct {
    Menu menu;
} MainScreen;

void MainScreenInit(MainScreen *self);
int MainScreenHandleInput(void *selfv, int input);
void MainScreenRender(void *selfv);
void MainScreenFree(void *self);
