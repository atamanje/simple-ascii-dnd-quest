#include "SkillCheckState.h"
#include "Engine.h"
#include "Input.h"
#include "Renderer.h"
#include "Dice.h"
#include <windows.h>

SkillCheckState::SkillCheckState(Engine* engine, std::string statName, int dc, std::function<void(bool)> onComplete)
    : m_Engine(engine), m_StatName(std::move(statName)), m_DC(dc), m_OnComplete(std::move(onComplete)),
      m_HasRolled(false), m_RollResult(0), m_Modifier(0), m_Success(false) 
{
    // Retrieve modifier from Player in GameSession
    Player* player = m_Engine->GetSession()->GetPlayer();
    if (player) {
        m_Modifier = player->GetStatModifier(m_StatName);
    }
}

void SkillCheckState::ProcessInput() {
    if (Input::IsKeyPressed(VK_RETURN) || Input::IsKeyPressed(VK_SPACE)) {
        if (!m_HasRolled) {
            m_RollResult = Dice::Roll(20);
            m_Success = (m_RollResult + m_Modifier) >= m_DC;
            
            // Auto-fail on 1, Auto-success on 20
            if (m_RollResult == 1) m_Success = false;
            if (m_RollResult == 20) m_Success = true;
            
            m_HasRolled = true;
        } else {
            // Already rolled, exit and call callback
            m_Engine->PopState();
            if (m_OnComplete) {
                m_OnComplete(m_Success);
            }
        }
    }
}

void SkillCheckState::Update(float deltaTime) {
}

void SkillCheckState::Render(Renderer* renderer) {
    int w = renderer->GetWidth();
    int h = renderer->GetHeight();
    
    int boxY = h / 2 - 4;
    
    // Draw Box
    for(int y = boxY; y < boxY + 8; y++) {
        for(int x = w/2 - 20; x < w/2 + 20; x++) {
            if (y == boxY || y == boxY + 7 || x == w/2 - 20 || x == w/2 + 19)
                renderer->DrawString(x, y, "*", FOREGROUND_RED | FOREGROUND_GREEN);
            else
                renderer->DrawString(x, y, " ", 0); // clear background
        }
    }
    
    char buffer[64];
    sprintf_s(buffer, "SKILL CHECK: %s (DC %d)", m_StatName.c_str(), m_DC);
    renderer->DrawString(w/2 - 15, boxY + 2, buffer, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    
    if (!m_HasRolled) {
        renderer->DrawString(w/2 - 10, boxY + 4, "Press [ENTER] to Roll", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    } else {
        sprintf_s(buffer, "Roll: %d + Mod: %d = %d", m_RollResult, m_Modifier, m_RollResult + m_Modifier);
        renderer->DrawString(w/2 - 12, boxY + 4, buffer, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        
        if (m_Success) {
            renderer->DrawString(w/2 - 5, boxY + 5, "SUCCESS!", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        } else {
            renderer->DrawString(w/2 - 5, boxY + 5, "FAILURE!", FOREGROUND_RED | FOREGROUND_INTENSITY);
        }
        
        renderer->DrawString(w/2 - 12, boxY + 6, "[ENTER] to continue", FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}
