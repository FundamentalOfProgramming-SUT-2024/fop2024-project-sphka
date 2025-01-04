#include "login_screen.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ncurses.h>

void LoginScreenInit(LoginScreen *self) {
    self->focused_field = 1;

    self->username[0] = 0;
    self->username_tb.buffer = self->username;
    self->username_tb.title = "Username";
    self->username_tb.capacity = sizeof(self->username);
    self->username_tb.cursor = 0;
    self->username_tb.is_password = false;

    self->password[0] = 0;
    self->password_tb.buffer = self->password;
    self->password_tb.title = "Password";
    self->password_tb.capacity = sizeof(self->password);
    self->password_tb.cursor = 0;
    self->password_tb.is_password = true;
}

int LoginScreenHandleInput(void *selfv, int input) {
    if (input == KEY_RESIZE)
        clear();
    
    LoginScreen *self = (LoginScreen *)selfv;

    if (self->focused_field == 1)
        TextboxHandleInput(&self->username_tb, input);
    else if (self->focused_field == 2)
        TextboxHandleInput(&self->password_tb, input);

    if (input == '\t')
        self->password_tb.is_password = !self->password_tb.is_password;

    if ((input == '\n' || input == KEY_LEFT) && self->focused_field == 0)
        return 0;

    if ((input == KEY_DOWN || input == '\n') && self->focused_field < 4 - 1) {
        self->focused_field++;
        return -1;
    } else if (input == KEY_UP && self->focused_field > 0) {
        self->focused_field--;
        return -1;
    }

    return -1;
}

static void RenderBackButton(LoginScreen *self, int x, int y) {
    if (self->focused_field == 0) {
        // attron(A_REVERSE);
        attron(COLOR_PAIR(3) | A_UNDERLINE);
    }
    mvprintw(x / 2 - 5, y / 2 - 24, "< Back");
    if (self->focused_field == 0) {
        // attroff(A_REVERSE);
        attroff(COLOR_PAIR(3) | A_UNDERLINE);
    }
}

void RenderPassVisibilityHint(int x, int y) {
    mvprintw(x / 2 + 3, y / 2 - 19, "(Press ");
    attron(A_ITALIC | A_BOLD);
    printw("tab");
    attroff(A_ITALIC | A_BOLD);
    printw(" to toggle password visibility)");
}

void RenderLoginButton(LoginScreen *self, int x, int y) {
    if (self->focused_field == 3) {
        attron(A_REVERSE);
        mvprintw(x / 2 + 5, y / 2 - 4, "  Login  ");
        attroff(A_REVERSE);
        mvaddch(x / 2 + 5, y / 2 - 6, '>' | COLOR_PAIR(3));
        mvaddch(x / 2 + 5, y / 2 + 6, '<' | COLOR_PAIR(3));
    } else {
        mvprintw(x / 2 + 5, y / 2 - 4, "  Login  ");
        mvaddch(x / 2 + 5, y / 2 - 6, ' ');
        mvaddch(x / 2 + 5, y / 2 + 6, ' ');
    }
}

void SetCursor(LoginScreen *self, int x, int y) {
    if (self->focused_field == 1)
        TextboxMove(&self->username_tb, x / 2 - 3, y / 2 - 26);
    else if (self->focused_field == 2)
        TextboxMove(&self->password_tb, x / 2, y / 2 - 26);
    else
        curs_set(0);
}

void LoginScreenRender(void *selfv) {
    LoginScreen *self = (LoginScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);
    mvprintw(x / 2 - 5, y / 2 - 7, "   - ");
    attron(A_ITALIC | A_BOLD);
    printw("Login");
    attroff(A_ITALIC | A_BOLD);
    printw(" -   ");

    RenderBackButton(self, x, y);

    TextboxRender(&self->username_tb, x / 2 - 3, y / 2 - 26, self->focused_field == 1);
    TextboxRender(&self->password_tb, x / 2, y / 2 - 26, self->focused_field == 2);

    RenderPassVisibilityHint(x, y);
    RenderLoginButton(self, x, y);

    SetCursor(self, x, y);
}

void LoginScreenFree(void *selfv) {
    LoginScreen *self = (LoginScreen *)selfv;
}
