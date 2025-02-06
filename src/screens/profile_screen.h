#pragma once

#include "widgets/simple_form.h"
#include "widgets/message_line.h"

typedef struct User User;

typedef struct {
} ProfileScreen;

void ProfileScreenInit(ProfileScreen *self);
void ProfileScreenHandleSwitch(void *selfv);
int ProfileScreenHandleInput(void *self, int input);
void ProfileScreenRender(void *selfv);
void ProfileScreenFree(void *self);
