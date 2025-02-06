#pragma once

#include <stdbool.h>

typedef struct Settings {
    bool enable_music;
    unsigned player_color;
    unsigned difficulty;
} Settings;

extern Settings g_settings;
extern const char *player_colors[7];
extern const char *difficulties[3];
