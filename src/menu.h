#pragma once

#include <ncurses.h>

// A simple multi-selection menu
typedef struct {
    char **entries;
    int n_entries;
    int selected;
} Menu;

int MenuHandleInput(Menu *menu, int input);
void MenuRender(Menu *menu, int x, int y);
