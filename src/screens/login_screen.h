#pragma once

#include "widgets/textbox.h"

typedef struct {
    char username[50];
    Textbox username_tb;

    char password[50];
    Textbox password_tb;

    int focused_field;
} LoginScreen;

void LoginScreenInit(LoginScreen *self);
int LoginScreenHandleInput(void *selfv, int input);
void LoginScreenRender(void *selfv);
void LoginScreenFree(void *self);
