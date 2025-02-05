#pragma once

#define MAGIC_EXIT (-42)

typedef struct {
    void (*Switch) (void *);
    int (*HandleInput) (void *, int);
    void (*Render) (void *);
    void (*Free) (void *);

    void *data;
} Screen;

static inline void ScreenSwitch(Screen *self) {
    if (self->Switch)
        self->Switch(self->data);
}

static inline int ScreenHandleInput(Screen *self, int input) {
    return self->HandleInput(self->data, input);
}

static inline void ScreenRender(Screen *self) {
    self->Render(self->data);
}

static inline void ScreenFree(Screen *self) {
    self->Free(self->data);
}
