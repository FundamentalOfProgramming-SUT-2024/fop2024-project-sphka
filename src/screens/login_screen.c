#include "login_screen.h"
#include "data/users.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ncurses.h>

void LoginScreenInit(LoginScreen *self) {
    self->form.n_fields = 3;
    self->form.focus = 0;
    self->form.render_back_button = true;
    self->message.type = MessageType_None;

    self->username[0] = 0;
    self->form.fields[0].type = FIELD_TYPE_TEXTBOX;
    self->form.fields[0].textbox.buffer = self->username;
    self->form.fields[0].textbox.capacity = sizeof(self->username);
    self->form.fields[0].textbox.cursor = 0;
    self->form.fields[0].textbox.is_password = false;
    self->form.fields[0].textbox.title = "Username";

    self->password[0] = 0;
    self->form.fields[1].type = FIELD_TYPE_TEXTBOX;
    self->form.fields[1].textbox.buffer = self->password;
    self->form.fields[1].textbox.capacity = sizeof(self->password);
    self->form.fields[1].textbox.cursor = 0;
    self->form.fields[1].textbox.is_password = true;
    self->form.fields[1].textbox.title = "Password";

    self->form.fields[2].type = FIELD_TYPE_BUTTON;
    self->form.fields[2].button.x_offset = 2;
    strcpy(self->form.fields[2].button.name, "  Login  ");
}

void LoginScreenHandleSwitch(void *selfv) {
    LoginScreen *self = (LoginScreen *)selfv;
    self->form.focus = 0;
    self->username[0] = 0;
    self->password[0] = 0;
    self->message.type = MessageType_None;

    self->form.fields[0].textbox.cursor = 0;
    self->form.fields[1].textbox.cursor = 0;
}

int LoginScreenHandleInput(void *selfv, int input) {
    if (input == KEY_RESIZE)
        clear();

    if (input == KEY_F(1)) {
        curs_set(0);
        return 0;
    }

    LoginScreen *self = (LoginScreen *)selfv;

    if (input == '\t')
        self->form.fields[2].textbox.is_password = !self->form.fields[2].textbox.is_password;

    int out = SimpleFormHandleInput(&self->form, input);

    if (out == MAGIC_BACK) {
        return 0;
    }

    if (out > 0) {
        if (strlen(self->username) == 0 || strlen(self->password) == 0) {
            self->message.type = MessageType_Error;
            strcpy(self->message.message, "Please fill all fields!");
        } else {
            User *user = UserManagerLogin(&usermanager, self->username, self->password);
            if (user == NULL) {
                self->message.type = MessageType_Error;
                strcpy(self->message.message, "Wrong username and/or password!");
            } else {
                logged_in_user = user;
                return 3;
            }
        }
    }

    return -1;
}

// TODO: Merge with the on at signup_screen
static void RenderPassVisibilityHint(int x, int y) {
    mvprintw(x / 2 + 2, y / 2 - 20, "(Press ");
    attron(A_ITALIC | A_BOLD);
    printw("tab");
    attroff(A_ITALIC | A_BOLD);
    printw(" to toggle password visibility)");
}

void LoginScreenRender(void *selfv) {
    LoginScreen *self = (LoginScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);
    mvprintw(x / 2 - 6, y / 2 - 7, "   - ");
    attron(A_ITALIC | A_BOLD);
    printw("Login");
    attroff(A_ITALIC | A_BOLD);
    printw(" -   ");

    self->form.x = x / 2 - 4;
    self->form.y = y / 2 - 26;

    SimpleFormRender(&self->form);
    RenderPassVisibilityHint(x, y);
    MessageLineRender(&self->message, x / 2 + 6, y);

    SimpleFormSetCursor(&self->form);

    refresh();
}

void LoginScreenFree(void *selfv) {
    // LoginScreen *self = (LoginScreen *)selfv;
}
