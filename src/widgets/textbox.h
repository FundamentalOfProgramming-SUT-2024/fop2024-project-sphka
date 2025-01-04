#pragma once

#include <stdbool.h>

typedef struct {
    char *buffer;
    char *title;
    int capacity;
    int cursor;
    bool is_password;
} Textbox;

int TextboxHandleInput(Textbox *textbox, int input);
void TextboxRender(Textbox *textbox, int x, int y, bool is_focused);
void TextboxMove(Textbox *textbox, int x, int y);
