#pragma once
#include "GameState.h"
#include <string>
#include <vector>

class Engine;

class MainMenuState : public GameState {
public:
    MainMenuState(Engine* engine);
    
    void ProcessInput() override;
    void Update(float deltaTime) override;
    void Render(Renderer* renderer) override;

private:
    Engine* m_Engine;
    int m_SelectedIndex;
    std::vector<std::string> m_Options;
};
