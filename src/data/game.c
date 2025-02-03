#include "game.h"

#include <stdlib.h>

#include "input.h"
#include "item.h"
#include "dialogs/weapon_selector.h"
#include "dialogs/food_selector.h"
#include "dialogs/potion_selector.h"

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

    game.player.normal_food  = 0;
    game.player.supreme_food = 0;
    game.player.magical_food = 0;
    game.player.rotten_food  = 0;

    game.player.gold = 0;

    game.player.health = MAX_HEALTH;
    game.player.hunger = MAX_HUNGER;
}

void NewGame() {
    game.floor_id = 0;
    game.map_revealed = false;
    game.over = false;
    game.killer = NULL;
    game.clock = 0;

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
        if (item->info == &gold_item_info)
            sprintf(g_message_bar, "You picked up %d pieces of Gold.", item->count);
        else
            sprintf(g_message_bar, "You picked up %d pieces of Gold via Dark Gold.", item->count);

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
    } else if (item->category == ItemCategory_Food) {
        int *foods = game.player.foods;
        int total_foods = foods[0] + foods[1] + foods[2] + foods[3];
        if (total_foods < 5) {
            sprintf(g_message_bar, "You picked up a piece of %s.", item->info->name);
            foods[item->ex_food.type] += 1;
            return true;
        } else {
            sprintf(g_message_bar, "You can't carry more than 5 food items!");
            return false;
        }
    } else if (item->category == ItemCategory_Potion) {
        sprintf(g_message_bar, "You picked up a %s.", item->info->name);
        game.player.potions[item->ex_potion.type] += 1;
        return true;
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
                enemy->health -= CURRENT_WEAPON.ex_weapon.type->damage * (game.player.buffs[PotionType_Damage] ? 2 : 1);
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
            enemy->health -= CURRENT_WEAPON.ex_weapon.type->damage * (game.player.buffs[PotionType_Damage] ? 2 : 1);
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

    int x, y;
    getmaxyx(stdscr, x, y);

    if (input == 'i')
        WeaponSelector(x, y);

    if (input == 'f')
        FoodSelector(x, y);

    if (input == 'p')
        PotionSelector(x, y);

    // Attack
    if (input == ' ')
        Attack();

    bool double_speed = game.player.buffs[PotionType_Speed] > 0;
    if ((double_speed ? EVERY(4): EVERY(2)) && game.player.hunger > 30) {
        int heal = (game.player.hunger - 30) / 2;
        game.player.health += heal * (game.player.buffs[PotionType_Health] ? 2 : 1);

        if (game.player.health > MAX_HEALTH) {
            game.player.health = MAX_HEALTH;
        }
    }

    if (double_speed ? EVERY(10): EVERY(5)) {
        game.player.hunger--;
        if (game.player.hunger < 0)
            game.player.hunger = 0;
    }

    if (double_speed ? EVERY(20): EVERY(10)) {
        // Degrade foods
        if (game.player.magical_food) {
            game.player.magical_food--;
            game.player.supreme_food++;
        }

        if (game.player.supreme_food) {
            game.player.supreme_food--;
            game.player.normal_food++;
        }

        if (game.player.normal_food) {
            game.player.normal_food--;
            game.player.rotten_food++;
        }
    }
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

void ConsumeFood(FoodType type) {
    game.player.foods[type]--;

    switch (type)
    {
    case FoodType_Magical:
        game.player.health += 30;
        game.player.hunger += 30;
        break;

    case FoodType_Supreme:
        game.player.health += 20;
        game.player.hunger += 20;
        break;

    case FoodType_Normal:
        game.player.health += 15;
        game.player.hunger += 15;
        break;

    case FoodType_Rotten:
        game.player.health -= 10;
        break;

    default:
        break;
    }

    if (type != FoodType_Rotten)
        sprintf(g_message_bar, "You ate a piece of %s!", foods[type].name);
    else
        sprintf(g_message_bar, "You ate a piece of Rotten Food. You have been poisoned!");

    if (game.player.health <= 0) {
        game.player.health = 0;
        game.over = true;
    } else if (game.player.health > MAX_HEALTH) {
        game.player.health = MAX_HEALTH;
    }

    if (game.player.hunger > MAX_HUNGER)
        game.player.hunger = MAX_HUNGER;
}

void ConsumePotion(PotionType type) {
    game.player.potions[type]--;
    game.player.buffs[type] += 11;
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
