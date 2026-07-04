#pragma once
#include "Item.h"
#include <vector>
#include <string>

class Inventory {
public:
    // Returns true if item added, false if adding it would exceed maxWeightCapacity
    bool AddItem(const Item& item, int maxWeightCapacity);
    
    // Returns true if an item with matching name was found and removed
    bool RemoveItem(const std::string& itemName);
    
    void AddGold(int amount);
    bool SpendGold(int amount); // Returns false if insufficient funds

    int GetTotalWeight() const;
    int GetGold() const { return m_Gold; }
    
    const std::vector<Item>& GetItems() const { return m_Items; }

private:
    std::vector<Item> m_Items;
    int m_Gold = 0;
};
