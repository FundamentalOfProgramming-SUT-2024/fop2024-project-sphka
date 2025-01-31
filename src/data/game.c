#include "game.h"

#include <stdlib.h>

Game game;

void InitPlayer(Player *player, Coord coord) {
    player->coord = coord;

    // Give the player a starting weapon
    game.player.n_weapons = 1;
    game.player.weapons[0].category = ItemCategory_Weapon;
    WeaponType *w_type = &weapons[WeaponType_Mace];
    game.player.weapons[0].info = &w_type->item_info;
    game.player.weapons[0].ex_weapon.type = w_type;
    game.player.weapons[0].count = w_type->collect_count;

    game.player.gold = 0;

    game.player.hunger = 20;
    game.player.health = 20;
}

void NewGame() {
    game.floor_id = 0;

    for (int i = 0; i < FLOOR_COUNT; i++)
        GenerateFloor(&game.floors[i], i ? &game.floors[i - 1] : NULL);

    InitPlayer(&game.player, GetRandomCoord(&CURRENT_FLOOR));
    Discover(&CURRENT_FLOOR, game.player.coord);
}

bool Pickup(Item *item) {
    if (item->category == ItemCategory_Gold) {
        sprintf(g_message_bar, "You picked up %d pieces of Gold.", item->count);
        game.player.gold += item->count;
        return true;
    } else if (item->category == ItemCategory_Weapon) {

        // Try to find an existing weapon slot
        Item *slot = NULL;
        for (int i = 0; i < game.player.n_weapons; i++) {
            Item *weapon = &game.player.weapons[i];
            if (weapon->info == item->info) {
                slot = weapon;
                break;
            }
        }

        if (!slot)
           game.player.weapons[game.player.n_weapons++] = *item;
        else if (item->ex_weapon.type->range)
            slot->count += item->count;

        if (item->ex_weapon.type->range == 0) {
            // Melee weapon
            if (slot)
                sprintf(g_message_bar, "You already have a %s.", item->info->name);
            else
                sprintf(g_message_bar, "You picked up a %s.", item->info->name);
        } else {
            sprintf(g_message_bar, "You picked up %d %ss.", item->count, item->info->name);
        }

        return item->ex_weapon.type->range || !slot;
    }

    return false;
}

char g_message_bar[500] = "";
