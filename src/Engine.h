#pragma once
#include "Renderer.h"
#include <chrono>

class GameState;

class Engine {
public:
    Engine(int width, int height);
    ~Engine();

    void Run();
    void Quit() { m_IsRunning = false; }
    void ChangeState(GameState* newState);

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    Renderer* m_Renderer;
    bool m_IsRunning;

    GameState* m_ActiveState;
};
