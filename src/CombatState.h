#pragma once
#include "GameState.h"
#include <string>
#include <vector>
#include <functional>

class Engine;

struct Enemy {
    std::string Name;
    int HP;
    int MaxHP;
    int AttackBonus;
    int DamageDie;
};

class CombatState : public GameState {
public:
    CombatState(Engine* engine, Enemy enemy, std::function<void(bool)> onComplete);

    void ProcessInput() override;
    void Update(float deltaTime) override;
    void Render(Renderer* renderer) override;

private:
    Engine* m_Engine;
    Enemy m_Enemy;
    std::function<void(bool)> m_OnComplete;
    
    std::vector<std::string> m_Actions;
    int m_SelectedIndex;
    
    std::vector<std::string> m_CombatLog;
    void AddLog(const std::string& msg);
    
    bool m_IsPlayerTurn;
    bool m_IsFinished;
    bool m_PlayerWon;
    
    void ExecutePlayerAction(int actionIndex);
    void EnemyTurn();
};
