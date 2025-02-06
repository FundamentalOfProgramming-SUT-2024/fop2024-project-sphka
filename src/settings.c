#include "settings.h"

Settings g_settings = {
    .enable_music = true,
    .player_color = 4,
    .difficulty = 0,
};

const char *player_colors[7] = {
    "",
    "Red",
    "Cyan",
    "Blue",
    "Green",
    "Yellow",
    "Magenta"
};

const char *difficulties[3] = {
    "Easy",
    "Hard",
    "Elite",
};
