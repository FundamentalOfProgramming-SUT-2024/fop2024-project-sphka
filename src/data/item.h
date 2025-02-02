#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum ItemCategory {
    ItemCategory_Gold,
    ItemCategory_Weapon,
    ItemCategory_Food,

    ItemCategoryCount,
} ItemCategory;

typedef struct ItemInfo {
    uint32_t sprite;
    const char *name;
} ItemInfo;

typedef struct WeaponType WeaponType;

typedef struct Item {
    ItemCategory category;

    // High level information that is shared among many different items
    // And is defined for all kinds of items
    ItemInfo *info;
    int count;

    union {
        struct Weapon {
            WeaponType *type;
        } ex_weapon;

        struct Food {
            enum FoodType {
                FoodType_Magical,
                FoodType_Supreme,
                FoodType_Normal,
                FoodType_Rotten,

                FoodTypeCount
            } type;
        } ex_food;
    };
    
} Item;

typedef enum FoodType FoodType;

typedef enum WeaponTypeID {
    WeaponType_Mace,
    WeaponType_Dagger,
    WeaponType_MagicWand,
    WeaponType_Arrow,
    WeaponType_Sword,

    WeaponTypeCount,
} WeaponTypeID;

typedef struct WeaponType {
    WeaponTypeID id;
    // Holds generic info about this weapon type
    ItemInfo item_info;

    int damage;
    int collect_count;

    // 0 for not ranged
    int range;
} WeaponType;

extern WeaponType weapons[WeaponTypeCount];
extern ItemInfo foods[FoodTypeCount];

extern ItemInfo gold_item_info;
extern ItemInfo dark_gold_item_info;


bool AreOfSameType(Item *a, Item *b);
