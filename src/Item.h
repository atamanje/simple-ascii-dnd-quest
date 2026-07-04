#pragma once
#include <string>

enum class ItemType {
    Weapon,
    Armor,
    Consumable,
    Valuable,
    Misc
};

struct Item {
    std::string Name;
    std::string Description;
    ItemType Type;
    int Weight; // In pounds
    int Value;  // In gold pieces
};
