#include "Engine.h"
#include <thread>

Engine::Engine(int width, int height) {
    m_Renderer = new Renderer(width, height);
    m_IsRunning = true;

    // Initialize bouncing object at center
    m_ObjX = width / 2.0f;
    m_ObjY = height / 2.0f;
    
    // Set some velocity (units per second)
    m_VelX = 20.0f; 
    m_VelY = 10.0f;
}

Engine::~Engine() {
    delete m_Renderer;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }
}

void Engine::ProcessInput() {
    // Check if ESC is pressed
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        m_IsRunning = false;
    }
}

void Engine::Update(float deltaTime) {
    m_ObjX += m_VelX * deltaTime;
    m_ObjY += m_VelY * deltaTime;

    // Bounce off walls
    if (m_ObjX <= 0 || m_ObjX >= m_Renderer->GetWidth() - 4) { // -4 for string width
        m_VelX = -m_VelX;
        // Clamp to avoid sticking
        m_ObjX = m_ObjX <= 0 ? 0 : m_Renderer->GetWidth() - 4; 
    }
    
    if (m_ObjY <= 0 || m_ObjY >= m_Renderer->GetHeight() - 1) {
        m_VelY = -m_VelY;
        m_ObjY = m_ObjY <= 0 ? 0 : m_Renderer->GetHeight() - 1;
    }
}

void Engine::Render() {
    m_Renderer->Clear();

    // Draw some static text
    m_Renderer->DrawString(2, 2, "ASCII DND Engine - Tech Demo");
    m_Renderer->DrawString(2, 3, "Press ESC to quit.");

    // Draw the bouncing object (a simple animated "sprite")
    // Colors: combine flags. RED | GREEN = YELLOW
    WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    m_Renderer->DrawString((int)m_ObjX, (int)m_ObjY, "[@@]", color);

    m_Renderer->Present();
}
