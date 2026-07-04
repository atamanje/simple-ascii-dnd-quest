#include "DialogState.h"
#include "Engine.h"
#include "Input.h"
#include "Renderer.h"
#include <windows.h>

DialogState::DialogState(Engine* engine, std::string speaker, std::string text)
    : m_Engine(engine), m_Speaker(std::move(speaker)), m_Text(std::move(text)) {}

void DialogState::ProcessInput() {
    if (Input::IsKeyPressed(VK_RETURN) || Input::IsKeyPressed(VK_SPACE)) {
        m_Engine->PopState();
    }
}

void DialogState::Update(float deltaTime) {
    // Basic dialog state doesn't need to update much yet, maybe a blinking cursor
}

void DialogState::Render(Renderer* renderer) {
    // Draw Dialog Box Borders over the previous state
    int w = renderer->GetWidth();
    int h = renderer->GetHeight();
    
    int boxY = h - 6;
    
    for(int x = 2; x < w - 2; x++) {
        renderer->DrawString(x, boxY, "-", FOREGROUND_GREEN | FOREGROUND_BLUE);
        renderer->DrawString(x, h - 1, "-", FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    
    // Speaker Name
    renderer->DrawString(4, boxY, ("[" + m_Speaker + "]").c_str(), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    
    // Text
    renderer->DrawString(4, boxY + 2, m_Text.c_str(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    
    renderer->DrawString(w - 25, h - 2, "[ENTER] to continue", FOREGROUND_GREEN | FOREGROUND_BLUE);
}
