#pragma once

class Dice {
public:
    static void Init();
    
    // Rolls a die with a given number of sides (e.g. Roll(20) for a d20)
    static int Roll(int sides);
    
    // Generates a random stat using the standard DND "4d6 drop lowest" method
    static int RollStat();
};
