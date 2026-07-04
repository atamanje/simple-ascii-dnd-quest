#pragma once
#include "GameState.h"
#include <string>
#include <functional>

class Engine;

class SkillCheckState : public GameState {
public:
    SkillCheckState(Engine* engine, std::string statName, int dc, std::function<void(bool)> onComplete);

    void ProcessInput() override;
    void Update(float deltaTime) override;
    void Render(Renderer* renderer) override;

private:
    Engine* m_Engine;
    std::string m_StatName;
    int m_DC;
    std::function<void(bool)> m_OnComplete;
    
    bool m_HasRolled;
    int m_RollResult;
    int m_Modifier;
    bool m_Success;
};
