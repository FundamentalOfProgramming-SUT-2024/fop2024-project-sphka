#include "game.h"

#include <stdlib.h>

#include "input.h"
#include "dialogs/weapon_selector.h"

Game game;

void InitPlayer(Player *player, Coord coord) {
    player->coord = coord;
    player->prev_coord = coord;

    // Give the player a starting weapon
    game.player.n_weapons = 1;
    game.player.weapons[0].category = ItemCategory_Weapon;
    WeaponType *w_type = &weapons[WeaponType_Mace];
    game.player.weapons[0].info = &w_type->item_info;
    game.player.weapons[0].ex_weapon.type = w_type;
    game.player.weapons[0].count = w_type->collect_count;
    game.player.equipment = 0;

    game.player.gold = 0;

    game.player.health = MAX_HEALTH;
    game.player.hunger = MAX_HUNGER;
}

void NewGame() {
    game.floor_id = 0;
    game.over = false;
    game.killer = NULL;

    for (int i = 0; i < FLOOR_COUNT; i++)
        GenerateFloor(&game.floors[i], i ? &game.floors[i - 1] : NULL);

    InitPlayer(&game.player, GetRandomCoord(&CURRENT_FLOOR));
    Discover(&CURRENT_FLOOR, game.player.coord);
}

bool Move(int input) {
    Coord next_player_coord = game.player.coord;
    Coord delta = InputDirection(input);

    if (delta.x == 0 && delta.y == 0)
        return false;

    game.player.prev_coord = game.player.coord;

    next_player_coord.x += delta.x;
    next_player_coord.y += delta.y;

    if (IsTilePassable(next_player_coord, NULL))
        game.player.coord = next_player_coord;

    return true;
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

void ConsumeWeapon() {
    CURRENT_WEAPON.count--;

    if (CURRENT_WEAPON.count == 0) {
        memmove(&CURRENT_WEAPON, &CURRENT_WEAPON + 1, (game.player.n_weapons - 1 - game.player.equipment) * sizeof(Item));
        game.player.n_weapons--;

        if (game.player.equipment >= game.player.n_weapons)
            game.player.equipment = game.player.n_weapons - 1;
    }
}

bool RangedAttack(Enemy **attackee, int *attackees, int *dead_enemies) {
    sprintf(g_message_bar, "Attacking with %s. Press a direction key.", CURRENT_WEAPON.info->name);
    UpdateMessageBar(true);

    Coord delta = InputDirection(getch());

    if (delta.x == 0 && delta.y == 0) {
        sprintf(g_message_bar, "Invalid key pressed.");
        return true;
    }

    Coord r = game.player.coord;
    int i = 0;
    bool must_drop = false;
    while (true) {
        Coord next_r = r;
        next_r.x += delta.x;
        next_r.y += delta.y;

        Enemy *enemy = NULL;
        if (!IsTilePassable(next_r, &enemy)) {
            // We either hit an enemy, or a wall or sth
            if (enemy) {
                enemy->health -= CURRENT_WEAPON.ex_weapon.type->damage;
                ConsumeWeapon();
                (*attackees)++;
                *attackee = enemy;
                if (enemy->health <= 0) {
                    (*dead_enemies)++;
                    enemy->health = 0;
                }
            } else if (i != 0) {
                must_drop = true;
            }

            break;
        } else if (i >= CURRENT_WEAPON.ex_weapon.type->range) {
            must_drop = true;
            break;
        } else {
            // Move on
            r = next_r;
            i++;
        }
    }

    if (must_drop) {
        // TODO: Should we override what ever there was?
        Tile *tile = &CURRENT_FLOOR.TILEC(r);

        if (!tile->has_item) {
            tile->has_item = true;
            tile->item = CURRENT_WEAPON;
            tile->item.count = 1;
        } else if (AreOfSameType(&tile->item, &CURRENT_WEAPON)) {
            tile->item.count++;
        }

        ConsumeWeapon();
    }

    return false;
}

void MeleeAttack(Enemy **attackee, int *attackees, int *dead_enemies) {
    for (int i = 0; i < CURRENT_FLOOR.n_enemies; i++) {
        Enemy *enemy = &CURRENT_FLOOR.enemies[i];
        if (enemy->health && SqDistance(enemy->coord, game.player.coord) <= 2) {
            enemy->health -= CURRENT_WEAPON.ex_weapon.type->damage;
            (*attackees)++;
            *attackee = enemy;
            if (enemy->health <= 0) {
                (*dead_enemies)++;
                enemy->health = 0;
            }
        }
    }
}

void Attack() {
    Enemy *attackee = NULL;
    int attackees = 0;
    int dead_enemies = 0;

    if (CURRENT_WEAPON.ex_weapon.type->range) {
        if (RangedAttack(&attackee, &attackees, &dead_enemies))
            return;
    }
    else
        MeleeAttack(&attackee, &attackees, &dead_enemies);

    if (attackees == 0)
        strcpy(g_message_bar, "You miss!");
    else if (attackees > 1) {
        if (dead_enemies == 0)
            sprintf(g_message_bar, "You hit %d enemies!", attackees);
        else {
            if (dead_enemies == attackees)
                sprintf(g_message_bar, "You hit and kill %d enemies!", attackees);
            else
                sprintf(g_message_bar, "You hit %d enemies, killing %d of them!", attackees, dead_enemies);
        }
    }
    else
        sprintf(g_message_bar, "You hit%s the %s!", attackee->health ? "" : " and kill", attackee->type->name);
}

void UpdatePlayer(int input) {
    bool bro_moved = Move(input);
    Tile *tile = &CURRENT_FLOOR.TILEC(game.player.coord);

    if (bro_moved)
        // Pickup item
        tile->has_item = tile->has_item && !Pickup(&tile->item);

    if (tile->c == '<' && input == '<' && game.floor_id < FLOOR_COUNT - 1) {
        bro_moved = true;
        game.floor_id++;
    } else if (tile->c == '>' && input == '>' && game.floor_id) {
        bro_moved = true;
        game.floor_id--;
    }

    if (bro_moved)
        Discover(&CURRENT_FLOOR, game.player.coord);

    if (input == 'i')
        WeaponSelector();

    // Attack
    if (input == ' ')
        Attack();
}

void UpdateEnemies() {
    for (int i = 0; i < CURRENT_FLOOR.n_enemies; i++) {
        Enemy *enemy = &CURRENT_FLOOR.enemies[i];
        EnemyUpdate(enemy);
    }
}

void Damage(Enemy *enemy) {
    game.player.health -= enemy->type->damage;
    if (game.player.health <= 0) {
        game.player.health = 0;
        game.over = true;
        game.killer = enemy;
    }
}

char g_message_bar[500] = "";

void UpdateMessageBar(bool refresh) {
    int x, y;
    getmaxyx(stdscr, x, y);

    for (int i = 0; i < y; i++)
        mvaddch(0, i, ' ');

    mvprintw(0, 0, "%s", g_message_bar);

    if (refresh)
        refresh();
}
