#pragma once

#include "widgets/simple_form.h"
#include "widgets/message_line.h"

typedef struct {
    char username[50];
    char password[50];

    MessageLine message;

    SimpleForm form;
} LoginScreen;

void LoginScreenInit(LoginScreen *self);
int LoginScreenHandleInput(void *selfv, int input);
void LoginScreenRender(void *selfv);
void LoginScreenFree(void *self);
