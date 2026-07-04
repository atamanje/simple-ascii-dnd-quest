#pragma once
#include <string>

struct AbilityScores {
    int Strength = 10;
    int Dexterity = 10;
    int Constitution = 10;
    int Intelligence = 10;
    int Wisdom = 10;
    int Charisma = 10;

    static int GetModifier(int score) {
        // In DND, score 10 is +0. Every 2 points above is +1. Below is -1.
        // C++ integer division truncates towards zero, so we need to be careful with negative numbers
        // A safer formula for DND math that handles negatives properly:
        if (score < 10 && score % 2 != 0) {
            return ((score - 10) / 2) - 1;
        }
        return (score - 10) / 2;
    }
};

enum class Species {
    Human,
    Elf,
    Dwarf,
    Orc
};

enum class CharacterClass {
    Fighter,
    Rogue,
    Wizard
};

class Player {
public:
    Player(std::string name, Species species, CharacterClass charClass, AbilityScores startingStats);

    const std::string& GetName() const { return m_Name; }
    Species GetSpecies() const { return m_Species; }
    CharacterClass GetClass() const { return m_Class; }
    
    int GetMaxHP() const { return m_MaxHP; }
    int GetCurrentHP() const { return m_CurrentHP; }
    
    const AbilityScores& GetStats() const { return m_Stats; }
    
    // Helper to get modifier by string name (e.g. "STR", "DEX")
    int GetStatModifier(const std::string& statName) const;

    void TakeDamage(int amount);
    void Heal(int amount);

private:
    std::string m_Name;
    Species m_Species;
    CharacterClass m_Class;
    
    AbilityScores m_Stats;

    int m_MaxHP;
    int m_CurrentHP;

    void ApplySpeciesBonuses();
    void CalculateStartingHP();
};
