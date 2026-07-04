#include "Player.h"
#include <algorithm>

Player::Player(std::string name, Species species, CharacterClass charClass, AbilityScores startingStats)
    : m_Name(std::move(name)), m_Species(species), m_Class(charClass), m_Stats(startingStats) 
{
    ApplySpeciesBonuses();
    CalculateStartingHP();
}

void Player::ApplySpeciesBonuses() {
    switch (m_Species) {
        case Species::Human:
            m_Stats.Strength++;
            m_Stats.Dexterity++;
            m_Stats.Constitution++;
            m_Stats.Intelligence++;
            m_Stats.Wisdom++;
            m_Stats.Charisma++;
            break;
        case Species::Elf:
            m_Stats.Dexterity += 2;
            break;
        case Species::Dwarf:
            m_Stats.Constitution += 2;
            break;
        case Species::Orc:
            m_Stats.Strength += 2;
            break;
    }
}

void Player::CalculateStartingHP() {
    int conModifier = AbilityScores::GetModifier(m_Stats.Constitution);
    
    switch (m_Class) {
        case CharacterClass::Fighter:
            m_MaxHP = 10 + conModifier;
            break;
        case CharacterClass::Rogue:
            m_MaxHP = 8 + conModifier;
            break;
        case CharacterClass::Wizard:
            m_MaxHP = 6 + conModifier;
            break;
    }
    
    // You always get at least 1 HP even if you have negative constitution
    m_MaxHP = std::max(1, m_MaxHP);
    m_CurrentHP = m_MaxHP;
}

int Player::GetStatModifier(const std::string& statName) const {
    if (statName == "STR") return AbilityScores::GetModifier(m_Stats.Strength);
    if (statName == "DEX") return AbilityScores::GetModifier(m_Stats.Dexterity);
    if (statName == "CON") return AbilityScores::GetModifier(m_Stats.Constitution);
    if (statName == "INT") return AbilityScores::GetModifier(m_Stats.Intelligence);
    if (statName == "WIS") return AbilityScores::GetModifier(m_Stats.Wisdom);
    if (statName == "CHA") return AbilityScores::GetModifier(m_Stats.Charisma);
    
    return 0; // Unknown stat
}

void Player::TakeDamage(int amount) {
    if (amount < 0) return;
    m_CurrentHP -= amount;
    if (m_CurrentHP < 0) m_CurrentHP = 0;
}

void Player::Heal(int amount) {
    if (amount < 0) return;
    m_CurrentHP += amount;
    if (m_CurrentHP > m_MaxHP) m_CurrentHP = m_MaxHP;
}
