#pragma once
#include "GameState.h"
#include <string>
#include <vector>
#include <functional>

class Engine;

struct Choice {
    std::string text;
    std::function<void()> onSelect;
};

class ExplorationState : public GameState {
public:
    ExplorationState(Engine* engine);

    void OnEnter() override;
    void OnResume() override;

    void ProcessInput() override;
    void Update(float deltaTime) override;
    void Render(Renderer* renderer) override;

private:
    Engine* m_Engine;
    
    std::string m_Narration;
    std::vector<Choice> m_Choices;
    int m_SelectedIndex;
    
    int m_CurrentNodeId; // Simple tracker for nodes
    
    void SetNode(int nodeId);
    
    void Node_Prologue_Tavern();
    void Node_Prologue_TheAttack();
    void Node_Prologue_Evacuation();
    void Node_Prologue_Malakor();
    void Node_Act1_Start();
};
