#pragma once
#include "Renderer.h"
#include <chrono>

class Engine {
public:
    Engine(int width, int height);
    ~Engine();

    void Run();

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    Renderer* m_Renderer;
    bool m_IsRunning;

    // Tech demo bouncing variables
    float m_ObjX;
    float m_ObjY;
    float m_VelX;
    float m_VelY;
};
