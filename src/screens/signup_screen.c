#include "signup_screen.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ncurses.h>

void SignupScreenInit(SignupScreen *self) {
    self->form.n_fields = 4;
    self->form.focus = 0;
    self->form.render_back_button = true;

    self->username[0] = 0;
    self->form.fields[0].type = FIELD_TYPE_TEXTBOX;
    self->form.fields[0].textbox.buffer = self->username;
    self->form.fields[0].textbox.capacity = sizeof(self->username);
    self->form.fields[0].textbox.cursor = 0;
    self->form.fields[0].textbox.is_password = false;
    self->form.fields[0].textbox.title = "Username";

    self->email[0] = 0;
    self->form.fields[1].type = FIELD_TYPE_TEXTBOX;
    self->form.fields[1].textbox.buffer = self->email;
    self->form.fields[1].textbox.capacity = sizeof(self->email);
    self->form.fields[1].textbox.cursor = 0;
    self->form.fields[1].textbox.is_password = false;
    self->form.fields[1].textbox.title = "Email";

    self->password[0] = 0;
    self->form.fields[2].type = FIELD_TYPE_TEXTBOX;
    self->form.fields[2].textbox.buffer = self->password;
    self->form.fields[2].textbox.capacity = sizeof(self->password);
    self->form.fields[2].textbox.cursor = 0;
    self->form.fields[2].textbox.is_password = true;
    self->form.fields[2].textbox.title = "Password";

    self->form.fields[3].type = FIELD_TYPE_BUTTON;
    self->form.fields[3].button.x_offset = 2;
    strcpy(self->form.fields[3].button.name, "  Create  ");
}

int SignupScreenHandleInput(void *selfv, int input) {
    if (input == KEY_RESIZE)
        clear();

    SignupScreen *self = (SignupScreen *)selfv;

    if (input == '\t')
        self->form.fields[2].textbox.is_password = !self->form.fields[2].textbox.is_password;

    int out = SimpleFormHandleInput(&self->form, input);

    if (out == MAGIC_BACK) {
        return 0;
    }

    return -1;
}

static void RenderPassVisibilityHint(int x, int y) {
    mvprintw(x / 2 + 5, y / 2 - 19, "(Press ");
    attron(A_ITALIC | A_BOLD);
    printw("tab");
    attroff(A_ITALIC | A_BOLD);
    printw(" to toggle password visibility)");
}

void SignupScreenRender(void *selfv) {
    SignupScreen *self = (SignupScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);
    mvprintw(x / 2 - 6, y / 2 - 7, "   - ");
    attron(A_ITALIC | A_BOLD);
    printw("Signup");
    attroff(A_ITALIC | A_BOLD);
    printw(" -   ");

    SimpleFormRender(&self->form, x / 2 - 4, y / 2 - 26);
    RenderPassVisibilityHint(x, y);
    SimpleFormSetCursor(&self->form, x / 2 - 4, y / 2 - 26);
}

void SignupScreenFree(void *selfv) {
    SignupScreen *self = (SignupScreen *)selfv;
}
