#include "Dice.h"
#include <random>
#include <algorithm>

static std::mt19937 s_RandomEngine;

void Dice::Init() {
    std::random_device rd;
    s_RandomEngine.seed(rd());
}

int Dice::Roll(int sides) {
    if (sides <= 0) return 0;
    std::uniform_int_distribution<int> dist(1, sides);
    return dist(s_RandomEngine);
}

int Dice::RollStat() {
    // 4d6 drop lowest is the standard method for rolling stats in DND
    int rolls[4];
    for (int i = 0; i < 4; i++) {
        rolls[i] = Roll(6);
    }
    
    // Sort ascending, so the lowest is at index 0
    std::sort(rolls, rolls + 4);
    
    // Sum the highest 3
    return rolls[1] + rolls[2] + rolls[3];
}
