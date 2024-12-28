#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "menu.h"
#include "screens/main_screen.h"
#include "screens/game_screen.h"
#include "screen.h"

int main() {
    srand(time(NULL));
    initscr();
    keypad(stdscr, TRUE);
    // raw();
    noecho();
    curs_set(0);
    start_color();
    // if (!can_change_color()) return 1;
    // init_color(COLOR_RED, 300, 300, 300);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    // init_pair(2, 1024, COLOR_BLACK);

    MainScreen ms;
    MainScreenInit(&ms);

    GameScreen gs;
    GameScreenInit(&gs);

    // Screen msVt = { 
    //     .HandleInput = MainScreenHandleInput,
    //     .Render = MainScreenRender,
    //     .Free = MainScreenFree,
    //     .data = &ms
    // };

    Screen msVt = { 
        .HandleInput = GameScreenHandleInput,
        .Render = GameScreenRender,
        .Free = GameScreenFree,
        .data = &gs
    };

    while (1) {
        ScreenRender(&msVt);
        refresh();
        int ch = getch();
        ScreenHandleInput(&msVt, ch);
    }
    
    endwin();
    return 0;
}
