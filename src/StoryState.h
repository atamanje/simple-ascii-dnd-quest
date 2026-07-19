#pragma once
#include "GameState.h"
#include "GenerativeDM.h"
#include "Player.h"
#include <string>
#include <future>

class Engine;

class StoryState : public GameState {
public:
    StoryState(Engine* engine, Player player);
    
    void OnEnter() override;
    void ProcessInput() override;
    void Update(float deltaTime) override;
    void Render(Renderer* renderer) override;

private:
    Engine* m_Engine;
    Player m_Player;
    GenerativeDM m_DM;
    
    std::string m_CurrentInput;
    std::string m_CurrentNarrative;
    
    // Async request state
    bool m_IsWaitingForDM;
    float m_WaitTimer;
    std::future<DMResponse> m_DMResponseFuture;
    
    // Dice rolling animation state
    bool m_IsRollingDice;
    float m_RollTimer;
    int m_LastRollResult;
    int m_LastTotalResult;
    DMResponse m_PendingDMAction;
    
    void RequestActionFromDM(const std::string& action);
    void RequestRollResolutionFromDM(int rollTotal);
};
