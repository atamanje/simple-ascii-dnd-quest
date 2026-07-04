#include "Engine.h"
#include "GameState.h"
#include "MainMenuState.h"
#include "Input.h"
#include <thread>
#include <windows.h> // For VK_ESCAPE

Engine::Engine(int width, int height) {
    m_Renderer = new Renderer(width, height);
    m_IsRunning = true;
    m_ActiveState = nullptr;

    // Start with the Main Menu state
    ChangeState(new MainMenuState(this));
}

Engine::~Engine() {
    if (m_ActiveState) {
        m_ActiveState->OnExit();
        delete m_ActiveState;
    }
    delete m_Renderer;
}

void Engine::ChangeState(GameState* newState) {
    if (m_ActiveState) {
        m_ActiveState->OnExit();
        delete m_ActiveState;
    }
    m_ActiveState = newState;
    if (m_ActiveState) {
        m_ActiveState->OnEnter();
    }
}

void Engine::Run() {
    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    while (m_IsRunning) {
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float deltaTime = elapsedTime.count();

        ProcessInput();
        Update(deltaTime);
        Render();

        // Cap to ~60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void Engine::ProcessInput() {
    Input::Update();
    
    // Global quit key for emergency exit
    if (Input::IsKeyPressed(VK_ESCAPE)) {
        Quit();
    }

    if (m_ActiveState) {
        m_ActiveState->ProcessInput();
    }
}

void Engine::Update(float deltaTime) {
    if (m_ActiveState) {
        m_ActiveState->Update(deltaTime);
    }
}

void Engine::Render() {
    if (m_ActiveState) {
        m_ActiveState->Render(m_Renderer);
    } else {
        m_Renderer->Clear();
        m_Renderer->Present();
    }
}
