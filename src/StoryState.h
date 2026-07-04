#pragma once
#include "GameState.h"
#include "QuestManager.h"
#include "Player.h"
#include <string>

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
    QuestManager m_QuestManager;
    
    int m_SelectedIndex;
    
    // Dice rolling animation state
    bool m_IsRollingDice;
    float m_RollTimer;
    int m_LastRollResult;
    int m_LastTotalResult;
    std::string m_RollMessage;
    
    // Art rendering state
    std::vector<std::string> m_ArtFrames;
    int m_CurrentArtFrame;
    float m_ArtFrameTimer;
    std::string m_LoadedArtPath;
    
    void ResolveChoice(const Choice& choice);
    void LoadArtAsset(const std::string& filepath);
};
