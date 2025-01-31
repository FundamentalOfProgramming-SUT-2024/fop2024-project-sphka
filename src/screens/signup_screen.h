#pragma once

#include "widgets/simple_form.h"
#include "widgets/message_line.h"

typedef struct {
    char username[50];
    char email[50];
    char password[50];

    MessageLine message;

    SimpleForm form;
} SignupScreen;

void SignupScreenInit(SignupScreen *self);
int SignupScreenHandleInput(void *self, int input);
void SignupScreenRender(void *selfv);
void SignupScreenFree(void *self);
