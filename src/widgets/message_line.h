#pragma once

#include <ncurses.h>
#include <string.h>

typedef enum MessageType {
    MessageType_None,
    MessageType_Info,
    MessageType_Error,
} MessageType;

typedef struct MessageLine {
    MessageType type;
    char message[100];
} MessageLine;

static inline void MessageLineRender(MessageLine *self, int line_x, int width) {
    int color_pair;
    switch (self->type)
    {
    case MessageType_Info: color_pair = 4; break;
    case MessageType_Error: color_pair = 1; break;
    default: color_pair = 0; break;
    }

    if (self->type != MessageType_None) {
        attron(COLOR_PAIR(color_pair));
        mvprintw(line_x, (width + 1 - strlen(self->message)) / 2, "%s", self->message);
        attroff(COLOR_PAIR(color_pair));
    } else {
        move(line_x, 0);
        deleteln();
    }
}
