#include "simple_form.h"

#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#define STRIDE 3

static void RenderBackButton(int x, int y, bool focused) {
    if (focused) {
        // attron(A_REVERSE);
        attron(COLOR_PAIR(3) | A_UNDERLINE);
    }
    mvprintw(x - 2, y, "< Back");
    if (focused) {
        // attroff(A_REVERSE);
        attroff(COLOR_PAIR(3) | A_UNDERLINE);
    }
}

void SimpleFormRender(SimpleForm *form) {
    int x = form->x;
    int y = form->y;

    if (form->render_back_button) {
        RenderBackButton(x, y, form->focus == -1);
    }

    for (int i = 0; i < form->n_fields; i++) {
        bool focused = i == form->focus;
        if (form->fields[i].type == FIELD_TYPE_TEXTBOX) {
            form->fields[i].textbox.x = x;
            form->fields[i].textbox.y = y;
            TextboxRender(&form->fields[i].textbox, focused);
        } else if (form->fields[i].type == FIELD_TYPE_BUTTON) {
            int buttonlen = strlen(form->fields[i].button.name) + 4;
            int delta_y = (53 - buttonlen) / 2;
            int delta_x = form->fields[i].button.x_offset;
            x += delta_x;
            y += delta_y;
            if (focused) {
                attron(A_REVERSE);
            }
            mvprintw(x, y, "  %s  ", form->fields[i].button.name);
            if (focused) {
                attroff(A_REVERSE);

                mvaddch(x, y - 2, '>' | COLOR_PAIR(3));
                mvaddch(x, y + buttonlen + 1, '<' | COLOR_PAIR(3));
            } else {
                mvaddch(x, y - 2, ' ');
                mvaddch(x, y + buttonlen + 1, ' ');
            }
            x -= delta_x;
            y -= delta_y;
        }

        x += STRIDE;
    }
}

int SimpleFormHandleInput(SimpleForm *form, int input) {
    if (form->focus >= 0)
    {
        SimpleFormField *focused_field = &form->fields[form->focus];

        if (input == '\n' && focused_field->type == FIELD_TYPE_BUTTON) {
            // fprintf(stderr, "returning %d\n", form->focus);
            return form->focus;
        }

        if (focused_field->type == FIELD_TYPE_TEXTBOX) {
            TextboxHandleInput(&focused_field->textbox, input);
        }
    } else if (form->focus == -1 && input == '\n') {
        return MAGIC_BACK;
    }

    if ((input == KEY_DOWN || input == '\n') && form->focus < form->n_fields - 1) {
        form->focus++;
        return -1;
    } else if (input == KEY_UP && form->focus > (form->render_back_button ? -1 : 0)) {
        form->focus--;
        return -1;
    }

    return -1;
}

void SimpleFormSetCursor(SimpleForm *form) {
    // int x = form->x;
    // int y = form->y;

    SimpleFormField *focused_field = &form->fields[form->focus];

    if (focused_field->type == FIELD_TYPE_TEXTBOX) {
        // focused_field->textbox.x = x + form->focus * STRIDE;
        // focused_field->textbox.y = y;
        TextboxMove(&focused_field->textbox);
    } else {
        curs_set(0);
    }
}
