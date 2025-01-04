#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "menu.h"
#include "screens/main_screen.h"
#include "screens/game_screen.h"
#include "screens/login_screen.h"
#include "screens/signup_screen.h"
#include "screen.h"

#include "data/users.h"

int main() {
    UserManager um;
    if (UserManagerInit(&um) < 0) {
        fprintf(stderr, "Failed to init usermanager.");
        return 1;
    }

    // if (UserManagerRegister(&um, "superscott", "aqaye_faaliyat@gmail.com", "sina") < 0) {
    //     fprintf(stderr, "Failed to register user.");
    //     return 1;
    // }

    // return 0;

    // if (users == NULL) {
    //     printf("Error.\n");
    //     return 1;
    // }

    // WriteUsers("users2.data", users, n_users);

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

    LoginScreen ls;
    LoginScreenInit(&ls);

    SignupScreen ss;
    SignupScreenInit(&ss);

    GameScreen gs;
    GameScreenInit(&gs);

    Screen main_screen = {
        .HandleInput = MainScreenHandleInput,
        .Render = MainScreenRender,
        .Free = MainScreenFree,
        .data = &ms
    };

    Screen login_screen = {
       .HandleInput = LoginScreenHandleInput,
       .Render = LoginScreenRender,
       .Free = LoginScreenFree,
       .data = &ls
    };

    Screen signup_screen = {
       .HandleInput = SignupScreenHandleInput,
       .Render = SignupScreenRender,
       .Free = SignupScreenFree,
       .data = &ss
    };

    Screen game_screen = {
        .HandleInput = GameScreenHandleInput,
        .Render = GameScreenRender,
        .Free = GameScreenFree,
        .data = &gs
    };

    Screen *currentScreen = &main_screen;
    Screen *screens[] = { &main_screen, &login_screen, &signup_screen, &game_screen };

    while (1) {
        ScreenRender(currentScreen);
        refresh();
        int ch = getch();
        int ret = ScreenHandleInput(currentScreen, ch);
        // fprintf(stderr, "%d\n", ret);
        if (ret >= 0) {
            // TODO: Reset screen?
            currentScreen = screens[ret];
            clear();
        }
    }
    
    endwin();
    return 0;
}
