#include "CombatState.h"
#include "Engine.h"
#include "Input.h"
#include "Renderer.h"
#include "Dice.h"
#include "Player.h"
#include <windows.h>
#include <string>

CombatState::CombatState(Engine* engine, Enemy enemy, std::function<void(bool)> onComplete)
    : m_Engine(engine), m_Enemy(std::move(enemy)), m_OnComplete(std::move(onComplete)),
      m_SelectedIndex(0), m_IsPlayerTurn(true), m_IsFinished(false), m_PlayerWon(false) 
{
    // Populate actions based on class
    Player* p = m_Engine->GetSession()->GetPlayer();
    m_Actions.push_back("Attack");
    m_Actions.push_back("Defend"); // Just reduces damage or skips turn for now
    
    if (p) {
        if (p->GetClass() == CharacterClass::Fighter) {
            m_Actions.push_back("Cleave (AoE/High Dmg)");
        } else if (p->GetClass() == CharacterClass::Rogue) {
            m_Actions.push_back("Sneak Attack (High Dmg)");
        } else if (p->GetClass() == CharacterClass::Wizard) {
            m_Actions.push_back("Fireball");
        }
    }
    
    AddLog("Combat started against " + m_Enemy.Name + "!");
}

void CombatState::AddLog(const std::string& msg) {
    m_CombatLog.push_back(msg);
    if (m_CombatLog.size() > 5) {
        m_CombatLog.erase(m_CombatLog.begin());
    }
}

void CombatState::ProcessInput() {
    if (m_IsFinished) {
        if (Input::IsKeyPressed(VK_RETURN) || Input::IsKeyPressed(VK_SPACE)) {
            m_Engine->PopState();
            if (m_OnComplete) {
                m_OnComplete(m_PlayerWon);
            }
        }
        return;
    }

    if (!m_IsPlayerTurn) {
        // Press Enter to progress enemy turn for pacing
        if (Input::IsKeyPressed(VK_RETURN)) {
            EnemyTurn();
        }
        return;
    }

    if (Input::IsKeyPressed(VK_UP)) {
        m_SelectedIndex--;
        if (m_SelectedIndex < 0) m_SelectedIndex = (int)m_Actions.size() - 1;
    }
    else if (Input::IsKeyPressed(VK_DOWN)) {
        m_SelectedIndex++;
        if (m_SelectedIndex >= (int)m_Actions.size()) m_SelectedIndex = 0;
    }
    else if (Input::IsKeyPressed(VK_RETURN)) {
        ExecutePlayerAction(m_SelectedIndex);
    }
}

void CombatState::ExecutePlayerAction(int actionIndex) {
    Player* p = m_Engine->GetSession()->GetPlayer();
    if (!p) return;

    std::string action = m_Actions[actionIndex];
    int damage = 0;
    
    if (action == "Attack") {
        damage = Dice::Roll(8) + p->GetStatModifier("STR");
        if (damage < 1) damage = 1;
        AddLog(p->GetName() + " attacks for " + std::to_string(damage) + " damage!");
        m_Enemy.HP -= damage;
    } 
    else if (action == "Defend") {
        AddLog(p->GetName() + " takes a defensive stance.");
        // We could set a defensive flag here
    }
    else {
        // Special class abilities
        if (p->GetClass() == CharacterClass::Fighter) {
            damage = Dice::Roll(12) + p->GetStatModifier("STR");
            AddLog(p->GetName() + " Cleaves for " + std::to_string(damage) + " damage!");
        } else if (p->GetClass() == CharacterClass::Rogue) {
            damage = Dice::Roll(6) + Dice::Roll(6) + p->GetStatModifier("DEX");
            AddLog(p->GetName() + " Sneak Attacks for " + std::to_string(damage) + " damage!");
        } else if (p->GetClass() == CharacterClass::Wizard) {
            damage = Dice::Roll(6) + Dice::Roll(6) + p->GetStatModifier("INT");
            AddLog(p->GetName() + " casts Fireball for " + std::to_string(damage) + " damage!");
        }
        if (damage < 1) damage = 1;
        m_Enemy.HP -= damage;
    }

    if (m_Enemy.HP <= 0) {
        m_Enemy.HP = 0;
        AddLog(m_Enemy.Name + " is defeated!");
        m_PlayerWon = true;
        m_IsFinished = true;
    } else {
        m_IsPlayerTurn = false;
        AddLog("Press [ENTER] for enemy turn...");
    }
}

void CombatState::EnemyTurn() {
    Player* p = m_Engine->GetSession()->GetPlayer();
    if (!p) return;

    // Simple attack logic
    int attackRoll = Dice::Roll(20) + m_Enemy.AttackBonus;
    // Assuming player AC is around 10 + DEX mod for now
    int playerAC = 10 + p->GetStatModifier("DEX");

    if (attackRoll >= playerAC) {
        int dmg = Dice::Roll(m_Enemy.DamageDie);
        AddLog(m_Enemy.Name + " hits for " + std::to_string(dmg) + " damage!");
        p->TakeDamage(dmg);
    } else {
        AddLog(m_Enemy.Name + " misses!");
    }

    if (p->GetCurrentHP() <= 0) {
        AddLog("You have been defeated...");
        m_PlayerWon = false;
        m_IsFinished = true;
    } else {
        m_IsPlayerTurn = true;
    }
}

void CombatState::Update(float deltaTime) {}

void CombatState::Render(Renderer* renderer) {
    renderer->Clear();
    
    Player* p = m_Engine->GetSession()->GetPlayer();
    int w = renderer->GetWidth();
    int h = renderer->GetHeight();
    
    // Draw Enemy
    renderer->DrawString(w/2 - 10, 2, ("Enemy: " + m_Enemy.Name).c_str(), FOREGROUND_RED | FOREGROUND_INTENSITY);
    char buf[32];
    sprintf_s(buf, "HP: %d / %d", m_Enemy.HP, m_Enemy.MaxHP);
    renderer->DrawString(w/2 - 10, 3, buf, FOREGROUND_RED);
    
    // Draw Player
    if (p) {
        renderer->DrawString(5, h - 10, p->GetName().c_str(), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        sprintf_s(buf, "HP: %d / %d", p->GetCurrentHP(), p->GetMaxHP());
        renderer->DrawString(5, h - 9, buf, FOREGROUND_GREEN);
    }

    // Draw Combat Log
    for (size_t i = 0; i < m_CombatLog.size(); ++i) {
        renderer->DrawString(w/2 + 5, h - 10 + (int)i, m_CombatLog[i].c_str(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    // Draw Actions
    if (!m_IsFinished && m_IsPlayerTurn) {
        renderer->DrawString(5, h - 7, "Choose Action:", FOREGROUND_INTENSITY);
        for (size_t i = 0; i < m_Actions.size(); ++i) {
            WORD color = (i == m_SelectedIndex) ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            if (i == m_SelectedIndex) renderer->DrawString(3, h - 6 + (int)i, ">", color);
            renderer->DrawString(5, h - 6 + (int)i, m_Actions[i].c_str(), color);
        }
    } else if (m_IsFinished) {
        renderer->DrawString(w/2 - 10, h/2, m_PlayerWon ? "VICTORY!" : "DEFEAT!", m_PlayerWon ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_INTENSITY));
        renderer->DrawString(w/2 - 10, h/2 + 1, "[ENTER] to continue", FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    renderer->Present();
}
