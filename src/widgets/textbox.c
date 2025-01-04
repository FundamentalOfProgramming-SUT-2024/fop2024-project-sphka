#include "textbox.h"

#include <string.h>
#include <ctype.h>
#include <ncurses.h>

int TextboxHandleInput(Textbox *textbox, int input) {
    // fprintf(stderr, "'%d'\n", input, input);
    if (32 <= input && input <= 126) {
        // fprintf(stderr, "Adding alnum %c %d\n", input, input);

        if (strlen(textbox->buffer) + 2 > textbox->capacity)
            return true;

        memmove(textbox->buffer + textbox->cursor + 1, textbox->buffer + textbox->cursor, strlen(textbox->buffer + textbox->cursor) + 1);
        *(textbox->buffer + textbox->cursor) = input;
        textbox->cursor++;

        return true;
    } else if (input == KEY_BACKSPACE || input == KEY_DC) {
        if (input == KEY_DC)
            textbox->cursor++;
        if (textbox->cursor) {
            memmove(textbox->buffer + textbox->cursor - 1, textbox->buffer + textbox->cursor, strlen(textbox->buffer + textbox->cursor) + 1);
            textbox->cursor--;
        }
        return true;
    } else if (input == KEY_LEFT) {
        (textbox->cursor) && (textbox->cursor--);
        return true;
    } else if (input == KEY_RIGHT) {
        (textbox->cursor < strlen(textbox->buffer)) && (textbox->cursor++);
        return true;
    } else if (input == KEY_HOME) {
        textbox->cursor = 0;
    } else if (input == KEY_END) {
        textbox->cursor = strlen(textbox->buffer);
    }

    return false;
}

void RenderRectangle(int y1, int x1, int y2, int x2)
{
    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}

void TextboxRender(Textbox *textbox, int x, int y, bool is_focused) {
    // char backup = textbox->buffer[textbox->capacity];
    // textbox->buffer[textbox->capacity] = 0;

    // TODO: Avoid hard coding
    // TODO: Cache len
    if (strlen(textbox->buffer) + 1 >= textbox->capacity)
        attron(COLOR_PAIR(1));
    else if (is_focused)
        attron(COLOR_PAIR(3));

    RenderRectangle(x, y, x + 2, y + textbox->capacity + 2);
    if (textbox->title) {
        mvprintw(x, y + 1, " ");
        printw("%s", textbox->title);
        printw(" ");
    }

    attroff(COLOR_PAIR(1) | COLOR_PAIR(3));
    // attroff();

    int len = strlen(textbox->buffer);
    if (textbox->is_password) {
        move(x + 1, y + 2);
        for (int i = 0; i < len; i++)
            addch('.');
    } else {
        mvprintw(x + 1, y + 2, "%s", textbox->buffer);
    }

    for (int i = 0; i < textbox->capacity - len; i++)
        addch(' ');

    move(x + 1, y + 3 + textbox->capacity);
    if (strlen(textbox->buffer) + 1 >= textbox->capacity) {
        // printw("Input is too long");
    } else {
        // printw("                 ");
    }

    // if (is_focused)
    //     move(x + 1, y + 1 + textbox->cursor);
    // textbox->buffer[textbox->capacity] = backup;
}

void TextboxMove(Textbox *textbox, int x, int y) {
    curs_set(1);
    move(x + 1, y + 2 + textbox->cursor);
}
