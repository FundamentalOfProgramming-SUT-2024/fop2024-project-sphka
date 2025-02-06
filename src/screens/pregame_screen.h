#pragma once

#include "widgets/menu.h"

typedef struct {
    Menu menu;
} PregameScreen;

void PregameScreenInit(PregameScreen *self);
void PregameScreenSwitch(void *selfv);
int PregameScreenHandleInput(void *selfv, int input);
void PregameScreenRender(void *selfv);
void PregameScreenFree(void *self);
