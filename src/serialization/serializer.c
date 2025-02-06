#include "data/item.h"
#include "data/game.h"

void fwrite_exact(FILE *file, int n, char *in) {
    for (int i = 0; i < n; i++)
        fputc(*(in++), file);
}

void SerializeItem(FILE *file, const Item *item) {
    fwrite_exact(file, sizeof(ItemCategory), (char *) &item->category);
    fwrite_exact(file, sizeof(int), (char *) &item->count);

    switch (item->category)
    {
    case ItemCategory_Gold:
        {
            char is_dark = item->info == &dark_gold_item_info;
            fwrite_exact(file, sizeof(char), (char *) &is_dark);
            break;
        }

    case ItemCategory_Weapon:
        fwrite_exact(file, sizeof(WeaponTypeID), (char *) &item->ex_weapon.type->id);
        break;

    case ItemCategory_Food:
        fwrite_exact(file, sizeof(FoodType), (char *) &item->ex_food.type);
        break;
    
    case ItemCategory_Potion:
        fwrite_exact(file, sizeof(FoodType), (char *) &item->ex_potion.type);
        break;

    default:
        break;
    }
}

void SerializePlayer(FILE *file, const Player *player) {
    fwrite_exact(file, ((char *) player->weapons) - ((char *) player), (char *) player);

    for (int i = 0; i < player->n_weapons; i++) {
        SerializeItem(file, &player->weapons[i]);
    }

    fwrite_exact(file, ((char *) &player->anchor) - ((char *) &player->equipment), (char *) &player->equipment);
}

void SerializeTile(FILE *file, const Tile *tile) {
    fwrite_exact(file, ((char *) &tile->item) - ((char *) tile), (char *) tile);

    if (tile->has_item)
        SerializeItem(file, &tile->item);
}

void SerializeEnemy(FILE *file, const Enemy *enemy) {
    fwrite_exact(file, ((char *) &enemy->type) - ((char *) enemy), (char *) enemy);
    fwrite_exact(file, sizeof(EnemyTypeID), (char *) &enemy->type->id);
}

void SerializeFloor(FILE *file, const Floor *floor) {
    for (int i = 0; i < MAXLINES * MAXCOLS; i++) {
        SerializeTile(file, &floor->tiles[i]);
    }

    fwrite_exact(file, ((char *) floor->enemies) - ((char *) floor->rooms), (char *) floor->rooms);

    for (int i = 0; i < floor->n_enemies; i++) {
        SerializeEnemy(file, &floor->enemies[i]);
    }
}

void SerializeGame(FILE *file, const Game *game) {
    SerializePlayer(file, &game->player);
    fwrite_exact(file, ((char *) game->floors) - ((char *) &game->clock), (char *) &game->clock);

    for (int i = 0; i < FLOOR_COUNT + 1; i++) {
        SerializeFloor(file, &game->floors[i]);
    }

    fwrite_exact(file, ((char *) &game->killer) - ((char *) &game->map_revealed), (char *) &game->map_revealed);
}
