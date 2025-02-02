#pragma once

#include "widgets/simple_form.h"
#include "widgets/message_line.h"

typedef struct User User;

typedef struct {
    User **users;
    int page;
} ScoreboardScreen;

void ScoreboardScreenInit(ScoreboardScreen *self);
int ScoreboardScreenHandleInput(void *self, int input);
void ScoreboardScreenRender(void *selfv);
void ScoreboardScreenFree(void *self);
