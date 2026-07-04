#include "StoryState.h"
#include "Engine.h"
#include "Input.h"
#include "Renderer.h"
#include "Dice.h"
#include <windows.h>
#include <iostream>

StoryState::StoryState(Engine* engine, Player player)
    : m_Engine(engine), m_Player(std::move(player)), m_SelectedIndex(0), m_IsRollingDice(false), m_RollTimer(0.0f)
{
    // Load the JSON story
    if (!m_QuestManager.LoadStory("story.json")) {
        std::cerr << "Failed to load story.json!" << std::endl;
    }
}

void StoryState::OnEnter() {
    m_SelectedIndex = 0;
}

void StoryState::ProcessInput() {
    if (m_IsRollingDice) {
        // Wait until animation finishes before allowing dismiss
        if (Input::IsKeyPressed(VK_RETURN) && m_RollTimer > 1.5f) {
            m_IsRollingDice = false;
        }
        return; // Ignore other input while rolling
    }

    const QuestNode* node = m_QuestManager.GetCurrentNode();
    if (!node || node->Choices.empty()) return;

    if (Input::IsKeyPressed(VK_UP)) {
        m_SelectedIndex--;
        if (m_SelectedIndex < 0) m_SelectedIndex = (int)node->Choices.size() - 1;
    }
    else if (Input::IsKeyPressed(VK_DOWN)) {
        m_SelectedIndex++;
        if (m_SelectedIndex >= (int)node->Choices.size()) m_SelectedIndex = 0;
    }
    else if (Input::IsKeyPressed(VK_RETURN)) {
        ResolveChoice(node->Choices[m_SelectedIndex]);
        m_SelectedIndex = 0; // Reset index for next node
    }
}

void StoryState::ResolveChoice(const Choice& choice) {
    if (choice.StatRequirement == "NONE") {
        m_QuestManager.SetCurrentNode(choice.SuccessNodeID);
        return;
    }

    // Handle stat check
    m_IsRollingDice = true;
    m_RollTimer = 0.0f;
    
    int modifier = m_Player.GetStatModifier(choice.StatRequirement);
    int roll = Dice::Roll(20);
    int total = roll + modifier;
    
    m_LastRollResult = roll;
    m_LastTotalResult = total;
    
    if (total >= choice.DifficultyClass) {
        m_RollMessage = "SUCCESS!";
        m_QuestManager.SetCurrentNode(choice.SuccessNodeID);
    } else {
        m_RollMessage = "FAILURE!";
        m_QuestManager.SetCurrentNode(choice.FailureNodeID);
    }
}

void StoryState::Update(float deltaTime) {
    if (m_IsRollingDice) {
        m_RollTimer += deltaTime;
    }
}

void StoryState::Render(Renderer* renderer) {
    renderer->Clear();
    
    // Top UI Header
    WORD uiColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Cyan
    std::string header = m_Player.GetName() + " | HP: " + std::to_string(m_Player.GetCurrentHP()) + "/" + std::to_string(m_Player.GetMaxHP());
    renderer->DrawString(2, 1, header.c_str(), uiColor);
    
    const QuestNode* node = m_QuestManager.GetCurrentNode();
    if (!node) {
        renderer->DrawString(2, 4, "No story loaded. Please check story.json.", FOREGROUND_RED | FOREGROUND_INTENSITY);
        renderer->Present();
        return;
    }

    // Narrative Text
    renderer->DrawString(2, 4, node->NarrativeText.c_str(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    // Choices / Dice Roll Animation
    if (m_IsRollingDice) {
        // Simple timing-based animation
        if (m_RollTimer < 1.0f) {
            renderer->DrawString(2, 10, "Rolling the D20...", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        } else {
            std::string resultStr = "You rolled a " + std::to_string(m_LastRollResult) + 
                                    " + (" + std::to_string(m_LastTotalResult - m_LastRollResult) + ") " + 
                                    " = " + std::to_string(m_LastTotalResult);
            renderer->DrawString(2, 10, resultStr.c_str(), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            
            WORD outcomeColor = (m_RollMessage == "SUCCESS!") ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_INTENSITY);
            renderer->DrawString(2, 12, m_RollMessage.c_str(), outcomeColor);
            
            if (m_RollTimer > 1.5f) {
                renderer->DrawString(2, 14, "Press ENTER to continue...", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
        }
    } else {
        int startY = 10;
        for (size_t i = 0; i < node->Choices.size(); i++) {
            WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Default white
            
            if (i == m_SelectedIndex) {
                color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                renderer->DrawString(2, startY + (int)i, ">", color);
            }
            
            std::string choiceText = node->Choices[i].Description;
            if (node->Choices[i].StatRequirement != "NONE") {
                choiceText += " [Requires " + node->Choices[i].StatRequirement + " DC " + std::to_string(node->Choices[i].DifficultyClass) + "]";
            }
            
            renderer->DrawString(4, startY + (int)i, choiceText.c_str(), color);
        }
    }
    
    renderer->Present();
}
