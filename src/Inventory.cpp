#include "Inventory.h"
#include <algorithm>

bool Inventory::AddItem(const Item& item, int maxWeightCapacity) {
    if (GetTotalWeight() + item.Weight > maxWeightCapacity) {
        return false; // Encumbered
    }
    m_Items.push_back(item);
    return true;
}

bool Inventory::RemoveItem(const std::string& itemName) {
    auto it = std::find_if(m_Items.begin(), m_Items.end(),
        [&itemName](const Item& i) { return i.Name == itemName; });
        
    if (it != m_Items.end()) {
        m_Items.erase(it);
        return true;
    }
    return false;
}

void Inventory::AddGold(int amount) {
    if (amount > 0) {
        m_Gold += amount;
    }
}

bool Inventory::SpendGold(int amount) {
    if (amount < 0) return false;
    if (m_Gold >= amount) {
        m_Gold -= amount;
        return true;
    }
    return false;
}

int Inventory::GetTotalWeight() const {
    int total = 0;
    for (const auto& item : m_Items) {
        total += item.Weight;
    }
    return total; // in pounds
}
