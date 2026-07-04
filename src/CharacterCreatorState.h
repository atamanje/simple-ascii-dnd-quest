#pragma once
#include "GameState.h"
#include "Player.h"
#include <string>
#include <vector>

class Engine;

enum class CreationStep {
    Name,
    Species,
    Class,
    RollStats,
    Confirm
};

class CharacterCreatorState : public GameState {
public:
    CharacterCreatorState(Engine* engine);

    void ProcessInput() override;
    void Update(float deltaTime) override;
    void Render(Renderer* renderer) override;

private:
    Engine* m_Engine;
    CreationStep m_Step;

    // Selections
    std::string m_Name;
    int m_SelectedSpeciesIndex;
    int m_SelectedClassIndex;
    AbilityScores m_RolledStats;

    std::vector<std::string> m_SpeciesOptions;
    std::vector<std::string> m_ClassOptions;

    float m_CursorTimer;
    bool m_CursorVisible;
    
    void RollStats();
};
