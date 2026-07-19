#include "Engine.h"
#include "GameState.h"
#include "MainMenuState.h"
#include "Input.h"
#include <thread>
#include <windows.h> // For VK_ESCAPE

#include "StoryState.h"
#include "Player.h"

Engine::Engine(int width, int height) {
    m_Renderer = new Renderer(width, height);
    m_IsRunning = true;

    // Start with the Main Menu
    ChangeState(new MainMenuState(this));
}

Engine::~Engine() {
    while (!m_States.empty()) {
        m_States.back()->OnExit();
        delete m_States.back();
        m_States.pop_back();
    }
    delete m_Renderer;
}

void Engine::ChangeState(GameState* newState) {
    if (!m_States.empty()) {
        m_States.back()->OnExit();
        delete m_States.back();
        m_States.pop_back();
    }
    m_States.push_back(newState);
    if (newState) {
        newState->OnEnter();
    }
}

void Engine::PushState(GameState* newState) {
    if (!m_States.empty()) {
        m_States.back()->OnPause();
    }
    m_States.push_back(newState);
    if (newState) {
        newState->OnEnter();
    }
}

void Engine::PopState() {
    if (!m_States.empty()) {
        m_States.back()->OnExit();
        delete m_States.back();
        m_States.pop_back();
    }
    if (!m_States.empty()) {
        m_States.back()->OnResume();
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

    if (!m_States.empty()) {
        m_States.back()->ProcessInput();
    }
}

void Engine::Update(float deltaTime) {
    if (!m_States.empty()) {
        m_States.back()->Update(deltaTime);
    }
}

void Engine::Render() {
    m_Renderer->Clear();
    for (GameState* state : m_States) {
        state->Render(m_Renderer);
    }
    m_Renderer->Present();
}
