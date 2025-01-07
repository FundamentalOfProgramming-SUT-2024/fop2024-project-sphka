#pragma once

#include <stdbool.h>

typedef struct {
    int x, y;

    char *buffer;
    char *title;
    int capacity;
    int cursor;
    bool is_password;
} Textbox;

int TextboxHandleInput(Textbox *textbox, int input);
void TextboxRender(Textbox *textbox, bool is_focused);
void TextboxMove(Textbox *textbox);
