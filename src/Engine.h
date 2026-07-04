#pragma once
#include "Renderer.h"
#include <chrono>
#include <vector>
#include "GameSession.h"

class GameState;

class Engine {
public:
    Engine(int width, int height);
    ~Engine();

    void Run();
    void Quit() { m_IsRunning = false; }
    
    void ChangeState(GameState* newState);
    void PushState(GameState* newState);
    void PopState();
    
    GameSession* GetSession() { return &m_Session; }

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    Renderer* m_Renderer;
    bool m_IsRunning;

    std::vector<GameState*> m_States;
    GameSession m_Session;
};
