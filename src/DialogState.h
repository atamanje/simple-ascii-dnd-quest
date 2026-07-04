#pragma once
#include "GameState.h"
#include <string>

class Engine;

class DialogState : public GameState {
public:
    DialogState(Engine* engine, std::string speaker, std::string text);

    void ProcessInput() override;
    void Update(float deltaTime) override;
    void Render(Renderer* renderer) override;

private:
    Engine* m_Engine;
    std::string m_Speaker;
    std::string m_Text;
};
