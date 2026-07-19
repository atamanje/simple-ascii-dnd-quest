#include "StoryState.h"
#include "Engine.h"
#include "Input.h"
#include "Renderer.h"
#include "Dice.h"
#include "GeminiClient.h"
#include <windows.h>
#include <iostream>

StoryState::StoryState(Engine* engine, Player player)
    : m_Engine(engine), m_Player(std::move(player)), m_DM(m_Player), 
      m_IsWaitingForDM(false), m_WaitTimer(0.0f), m_IsRollingDice(false), m_RollTimer(0.0f)
{
    // Ensure GeminiClient is initialized
    GeminiClient::Initialize();
}

void StoryState::OnEnter() {
    // Request initial scene from the DM
    RequestActionFromDM("");
}

void StoryState::RequestActionFromDM(const std::string& action) {
    m_IsWaitingForDM = true;
    m_WaitTimer = 0.0f;
    m_DMResponseFuture = std::async(std::launch::async, [this, action]() {
        return m_DM.TakeAction(action);
    });
}

void StoryState::RequestRollResolutionFromDM(int rollTotal) {
    m_IsWaitingForDM = true;
    m_WaitTimer = 0.0f;
    m_DMResponseFuture = std::async(std::launch::async, [this, rollTotal]() {
        return m_DM.ResolveRoll(rollTotal);
    });
}

void StoryState::ProcessInput() {
    if (m_IsWaitingForDM) return; 

    if (m_IsRollingDice) {
        if (Input::IsKeyPressed(VK_RETURN) && m_RollTimer > 1.5f) {
            m_IsRollingDice = false;
            // Now tell the DM the result
            RequestRollResolutionFromDM(m_LastTotalResult);
        }
        return;
    }

    // Very basic text input for prototype (A-Z, Space, Backspace)
    for (int key = 'A'; key <= 'Z'; key++) {
        if (Input::IsKeyPressed(key)) {
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            m_CurrentInput += shift ? (char)key : (char)(key + 32);
        }
    }
    
    // Basic punctuation support
    if (Input::IsKeyPressed(VK_SPACE)) m_CurrentInput += ' ';
    if (Input::IsKeyPressed(VK_OEM_PERIOD)) m_CurrentInput += '.';
    if (Input::IsKeyPressed(VK_OEM_COMMA)) m_CurrentInput += ',';
    
    if (Input::IsKeyPressed(VK_BACK) && !m_CurrentInput.empty()) m_CurrentInput.pop_back();

    if (Input::IsKeyPressed(VK_RETURN) && !m_CurrentInput.empty()) {
        std::string action = m_CurrentInput;
        m_CurrentInput.clear();
        RequestActionFromDM(action);
    }
}

void StoryState::Update(float deltaTime) {
    if (m_IsWaitingForDM) {
        m_WaitTimer += deltaTime;
        
        // Check if future is ready
        if (m_DMResponseFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            m_IsWaitingForDM = false;
            m_PendingDMAction = m_DMResponseFuture.get();
            m_CurrentNarrative = m_PendingDMAction.Narrative;
            
            if (m_PendingDMAction.RequiresRoll) {
                m_IsRollingDice = true;
                m_RollTimer = 0.0f;
                int modifier = m_Player.GetStatModifier(m_PendingDMAction.Stat);
                m_LastRollResult = Dice::Roll(20);
                m_LastTotalResult = m_LastRollResult + modifier;
            }
        }
    }

    if (m_IsRollingDice) {
        m_RollTimer += deltaTime;
    }
}

void StoryState::Render(Renderer* renderer) {
    renderer->Clear();
    
    // UI Header
    WORD uiColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    std::string header = m_Player.GetName() + " | HP: " + std::to_string(m_Player.GetCurrentHP()) + "/" + std::to_string(m_Player.GetMaxHP());
    renderer->DrawString(2, 1, header.c_str(), uiColor);
    
    // Narrative
    renderer->DrawStringWrapped(2, 3, m_CurrentNarrative.c_str(), renderer->GetWidth() - 4, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    int interactY = 22;

    if (m_IsWaitingForDM) {
        // Loading animation
        std::string dots = "";
        int numDots = ((int)(m_WaitTimer * 3.0f)) % 4;
        for (int i=0; i<numDots; i++) dots += ".";
        renderer->DrawString(2, interactY, ("The Dungeon Master is thinking" + dots).c_str(), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    } 
    else if (m_IsRollingDice) {
        if (m_RollTimer < 1.0f) {
            std::string msg = "Rolling a D20 for " + m_PendingDMAction.Stat + " (DC " + std::to_string(m_PendingDMAction.DC) + ")...";
            renderer->DrawString(2, interactY, msg.c_str(), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        } else {
            std::string resultStr = "You rolled a " + std::to_string(m_LastRollResult) + 
                                    " + (" + std::to_string(m_LastTotalResult - m_LastRollResult) + ") " + 
                                    " = " + std::to_string(m_LastTotalResult);
            renderer->DrawString(2, interactY, resultStr.c_str(), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            
            if (m_RollTimer > 1.5f) {
                renderer->DrawString(2, interactY + 2, "Press ENTER to send result to DM...", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
        }
    } 
    else {
        // Input box
        renderer->DrawString(2, interactY, "What do you do?", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        renderer->DrawString(2, interactY + 1, ("> " + m_CurrentInput + "_").c_str(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    
    renderer->Present();
}
