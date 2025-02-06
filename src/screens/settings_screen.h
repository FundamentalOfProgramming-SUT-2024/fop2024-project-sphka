#pragma once

#include "widgets/simple_form.h"
#include "widgets/message_line.h"

typedef struct {
    int focus;
} SettingsScreen;

void SettingsScreenInit(SettingsScreen *self);
void SettingsScreenHandleSwitch(void *selfv);
int SettingsScreenHandleInput(void *selfv, int input);
void SettingsScreenRender(void *selfv);
void SettingsScreenFree(void *self);
