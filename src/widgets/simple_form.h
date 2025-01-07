#pragma once

#include "textbox.h"

#define MAGIC_BACK (-88)

typedef enum {
    FIELD_TYPE_BUTTON,
    FIELD_TYPE_TEXTBOX,
} FieldType;

typedef struct
{
    FieldType type;

    union {
        struct Button {
            char name[50];
            int x_offset;
        } button;
        Textbox textbox;
    };
} SimpleFormField;

typedef struct {
    int x, y;

    SimpleFormField fields[7];
    int n_fields;
    int focus;

    bool render_back_button;
} SimpleForm;

void SimpleFormRender(SimpleForm *form);
int SimpleFormHandleInput(SimpleForm *form, int input);
void SimpleFormSetCursor(SimpleForm *form);
