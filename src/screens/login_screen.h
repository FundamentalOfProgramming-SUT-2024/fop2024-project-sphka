#pragma once

#include "widgets/simple_form.h"

typedef struct {
    char username[50];
    char password[50];

    SimpleForm form;
} LoginScreen;

void LoginScreenInit(LoginScreen *self);
int LoginScreenHandleInput(void *selfv, int input);
void LoginScreenRender(void *selfv);
void LoginScreenFree(void *self);
