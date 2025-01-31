#include "item.h"

#include <ncurses.h>

WeaponType weapons[WeaponTypeCount] = {
    {
        .id = WeaponType_Mace,
        .item_info = { .sprite = 'M', .name = "\u2692 Mace" },
        .damage = 5,
        .collect_count = 0,
        .range = 0
    },
    {
        .id = WeaponType_Dagger,
        .item_info = { .sprite = 'D', .name = "\U0001f5e1 Dagger" },
        .damage = 12,
        .collect_count = 10,
        .range = 5
    },
    {
        .id = WeaponType_MagicWand,
        .item_info = { .sprite = 'W', .name = "\U0001fa84 Magic Wand" },
        .damage = 15,
        .collect_count = 8,
        .range = 10
    },
    {
        .id = WeaponType_Arrow,
        .item_info = { .sprite = 'A', .name = "\u27b3 Arrow" },
        .damage = 5,
        .collect_count = 20,
        .range = 5
    },
    {
        .id = WeaponType_Sword,
        .item_info = { .sprite = 'S', .name = "\u2694 Sword" },
        .damage = 10,
        .collect_count = 1,
        .range = 0
    }
};

ItemInfo gold_item_info = {
    .sprite = '$' | COLOR_PAIR(5),
    .name = "Gold"
};
