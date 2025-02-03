#include "signup_screen.h"
#include "data/users.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ncurses.h>

void SignupScreenInit(SignupScreen *self) {
    self->message.type = MessageType_None;
    self->message.message[0] = '\0';
    
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

void SignupScreenHandleSwitch(void *selfv) {
    SignupScreen *self = (SignupScreen *)selfv;
    self->form.n_fields = 4;
    self->username[0] = 0;
    self->email[0] = 0;
    self->password[0] = 0;

    self->form.fields[0].textbox.cursor = 0;
    self->form.fields[1].textbox.cursor = 0;
    self->form.fields[2].textbox.cursor = 0;
}

static bool CheckPassword(SignupScreen *self) {
    if (strlen(self->password) < 7) {
        self->message.type = MessageType_Error;
        strcpy(self->message.message, "The password is shorter than 7 characters!");
        return false;
    }

    bool has_upper = false;
    bool has_lower = false;
    bool has_digit = false;

    for (char *c = self->password; *c; c++) {
        has_upper |= 'A' <= *c && *c <= 'Z';
        has_lower |= 'a' <= *c && *c <= 'z';
        has_digit |= '0' <= *c && *c <= '9';
    }

    if (!has_upper || !has_lower || !has_digit) {
        self->message.type = MessageType_Error;
        strcpy(self->message.message, "The password does not contain any");

        if (!has_upper) {
            strcat(self->message.message, " uppercase letters");
        }

        if (!has_lower) {
            if (!has_upper && has_digit)
                strcat(self->message.message, " and");
            else if (!has_upper)
                strcat(self->message.message, ",");

            strcat(self->message.message, " lowercase letters");
        }

        if (!has_digit) {
            if (!has_upper || !has_lower)
                strcat(self->message.message, " and");

            strcat(self->message.message, " digits");
        }

        strcat(self->message.message, "!");
        return false;
    }

    return true;
}

int SignupScreenHandleInput(void *selfv, int input) {
    if (input == KEY_RESIZE)
        clear();

    if (input == KEY_F(1)) {
        curs_set(0);
        return 0;
    }

    SignupScreen *self = (SignupScreen *)selfv;
    self->message.type = MessageType_None;

    if (input == '\t')
        self->form.fields[2].textbox.is_password = !self->form.fields[2].textbox.is_password;

    int out = SimpleFormHandleInput(&self->form, input);

    if (out == MAGIC_BACK) {
        return 0;
    }

    if (out > 0) {
        // ُTODO: Check email
        if (CheckPassword(self)) {
            int reg = UserManagerRegister(&usermanager, self->username, self->email, self->password);
            if (reg != 0) {
                self->message.type = MessageType_Error;
                strcpy(self->message.message, "This username already exists!");
            } else {
                int flush = UserManagerFlush(&usermanager);
                if (flush != 0) {
                    self->message.type = MessageType_Error;
                    strcpy(self->message.message, "Unable to write data to the disk!");
                } else {
                    self->message.type = MessageType_Info;
                    strcpy(self->message.message, "User created successfully!");
                }
                // fprintf(stderr, "registering %d %d\n", reg, flush);
            }
        }
    }

    return -1;
}

static void RenderPassVisibilityHint(int x, int y) {
    mvprintw(x / 2 + 5, y / 2 - 20, "(Press ");
    attron(A_ITALIC | A_BOLD);
    printw("tab");
    attroff(A_ITALIC | A_BOLD);
    printw(" to toggle password visibility)");
}

void SignupScreenRender(void *selfv) {
    SignupScreen *self = (SignupScreen *)selfv;

    int x, y;
    getmaxyx(stdscr, x, y);
    mvprintw(x / 2 - 6, y / 2 - 9, "   - ");
    attron(A_ITALIC | A_BOLD);
    printw("Register");
    attroff(A_ITALIC | A_BOLD);
    printw(" -   ");

    self->form.x = x / 2 - 4;
    self->form.y = y / 2 - 26;

    SimpleFormRender(&self->form);
    RenderPassVisibilityHint(x, y);
    MessageLineRender(&self->message, x / 2 + 9, y);

    SimpleFormSetCursor(&self->form);
    refresh();
}

void SignupScreenFree(void *selfv) {
    // SignupScreen *self = (SignupScreen *)selfv;
}
