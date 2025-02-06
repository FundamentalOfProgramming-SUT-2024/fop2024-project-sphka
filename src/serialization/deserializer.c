#include "data/item.h"
#include "data/game.h"

void fread_exact(FILE *file, int n, char *out) {
    for (int i = 0; i < n; i++)
        *(out++) = fgetc(file);
}

void DeserializeItem(FILE *file, Item *item) {
    fread_exact(file, sizeof(ItemCategory), (char *) &item->category);
    fread_exact(file, sizeof(int), (char *) &item->count);

    switch (item->category)
    {
    case ItemCategory_Gold:
        {
            char is_dark;
            fread_exact(file, sizeof(char), (char *) &is_dark);
            item->info = is_dark ? &dark_gold_item_info : &gold_item_info;
            break;
        }

    case ItemCategory_Weapon:
        {
            WeaponTypeID wt;
            fread_exact(file, sizeof(WeaponTypeID), (char *) &wt);
            item->ex_weapon.type = &weapons[wt];
            item->info = &weapons[wt].item_info;
            break;
        }

    case ItemCategory_Food:
        fread_exact(file, sizeof(FoodType), (char *) &item->ex_food.type);
        item->info = &foods[item->ex_food.type];
        break;

    case ItemCategory_Potion:
        fread_exact(file, sizeof(FoodType), (char *) &item->ex_potion.type);
        item->info = &potions[item->ex_potion.type];
        break;
    
    default:
        break;
    }
}

void DeserializePlayer(FILE *file, Player *player) {
    fread_exact(file, ((char *) player->weapons) - ((char *) player), (char *) player);

    for (int i = 0; i < player->n_weapons; i++) {
        DeserializeItem(file, &player->weapons[i]);
    }

    fread_exact(file, ((char *) &player->anchor) - ((char *) &player->equipment), (char *) &player->equipment);

    fprintf(stderr, "Desed player h=%d\n", player->health);
}

void DeserializeTile(FILE *file, Tile *tile) {
    fread_exact(file, ((char *) &tile->item) - ((char *) tile), (char *) tile);

    if (tile->has_item)
        DeserializeItem(file, &tile->item);
}

void DeserializeEnemy(FILE *file, Enemy *enemy) {
    fread_exact(file, ((char *) &enemy->type) - ((char *) enemy), (char *) enemy);

    EnemyTypeID et;
    fread_exact(file, sizeof(EnemyTypeID), (char *) &et);
    enemy->type = &enemies[et];
}

void DeserializeFloor(FILE *file, Floor *floor) {
    for (int i = 0; i < MAXLINES * MAXCOLS; i++) {
        DeserializeTile(file, &floor->tiles[i]);
    }

    fread_exact(file, ((char *) floor->enemies) - ((char *) floor->rooms), (char *) floor->rooms);

    for (int i = 0; i < floor->n_enemies; i++) {
        DeserializeEnemy(file, &floor->enemies[i]);
    }
}

void DeserializeGame(FILE *file, Game *game) {
    DeserializePlayer(file, &game->player);
    fread_exact(file, ((char *) game->floors) - ((char *) &game->clock), (char *) &game->clock);

    for (int i = 0; i < FLOOR_COUNT + 1; i++) {
        DeserializeFloor(file, &game->floors[i]);
    }

    fread_exact(file, ((char *) &game->killer) - ((char *) &game->map_revealed), (char *) &game->map_revealed);
}
