#include "MainMenuState.h"
#include "Engine.h"
#include "Input.h"
#include "Renderer.h"
#include "CharacterCreatorState.h"
#include <windows.h>

MainMenuState::MainMenuState(Engine* engine) : m_Engine(engine), m_SelectedIndex(0) {
    m_Options = { "Start Game", "Options", "Exit" };
}

void MainMenuState::ProcessInput() {
    if (Input::IsKeyPressed(VK_UP)) {
        m_SelectedIndex--;
        if (m_SelectedIndex < 0) m_SelectedIndex = (int)m_Options.size() - 1;
    }
    else if (Input::IsKeyPressed(VK_DOWN)) {
        m_SelectedIndex++;
        if (m_SelectedIndex >= (int)m_Options.size()) m_SelectedIndex = 0;
    }
    else if (Input::IsKeyPressed(VK_RETURN)) {
        if (m_SelectedIndex == 0) {
            m_Engine->ChangeState(new CharacterCreatorState(m_Engine));
        } else if (m_SelectedIndex == 1) {
            // TODO: Options (Dummy for now)
        } else if (m_SelectedIndex == 2) {
            m_Engine->Quit();
        }
    }
}

void MainMenuState::Update(float deltaTime) {
    // Basic menu has no dynamic updates yet
}

void MainMenuState::Render(Renderer* renderer) {
    renderer->Clear();
    
    // Draw Title
    renderer->DrawString(renderer->GetWidth() / 2 - 10, 5, "ASCII DND QUEST", FOREGROUND_RED | FOREGROUND_INTENSITY);
    
    // Draw Options
    int startY = 10;
    for (size_t i = 0; i < m_Options.size(); i++) {
        WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Default White
        
        if (i == m_SelectedIndex) {
            // Highlight selected option
            color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            renderer->DrawString(renderer->GetWidth() / 2 - 8, startY + (int)i, ">", color);
        }
        
        renderer->DrawString(renderer->GetWidth() / 2 - 5, startY + (int)i, m_Options[i].c_str(), color);
    }
    
    renderer->Present();
}
