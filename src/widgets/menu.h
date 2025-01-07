#pragma once

// A simple multi-selection menu
typedef struct {
    int x, y;

    char **entries;
    int n_entries;
    int selected;
} Menu;

int MenuHandleInput(Menu *menu, int input);
void MenuRender(Menu *menu);
