#include "ExplorationState.h"
#include "Engine.h"
#include "Input.h"
#include "Renderer.h"
#include "DialogState.h"
#include "CombatState.h"
#include "SkillCheckState.h"
#include "MainMenuState.h"
#include <windows.h>

ExplorationState::ExplorationState(Engine* engine)
    : m_Engine(engine), m_SelectedIndex(0), m_CurrentNodeId(0) {}

void ExplorationState::OnEnter() {
    SetNode(0);
}

void ExplorationState::OnResume() {
    // Return from a pushed state, could refresh logic here if needed
}

void ExplorationState::ProcessInput() {
    if (m_Choices.empty()) return;

    if (Input::IsKeyPressed(VK_UP)) {
        m_SelectedIndex--;
        if (m_SelectedIndex < 0) m_SelectedIndex = (int)m_Choices.size() - 1;
    }
    else if (Input::IsKeyPressed(VK_DOWN)) {
        m_SelectedIndex++;
        if (m_SelectedIndex >= (int)m_Choices.size()) m_SelectedIndex = 0;
    }
    else if (Input::IsKeyPressed(VK_RETURN)) {
        if (m_SelectedIndex >= 0 && m_SelectedIndex < m_Choices.size()) {
            m_Choices[m_SelectedIndex].onSelect();
        }
    }
}

void ExplorationState::Update(float deltaTime) {
}

void ExplorationState::Render(Renderer* renderer) {
    renderer->Clear();
    int w = renderer->GetWidth();
    
    // Draw Narration
    // Basic word wrap simulation (for now just print string)
    // We can do manual line breaks with multiple DrawString calls, but for now we'll just print it.
    // In a real app we'd have a WordWrap function.
    
    // Simple line splitter for m_Narration based on '\n'
    int y = 2;
    size_t start = 0;
    size_t end = m_Narration.find('\n');
    while (end != std::string::npos) {
        renderer->DrawString(5, y++, m_Narration.substr(start, end - start).c_str(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        start = end + 1;
        end = m_Narration.find('\n', start);
    }
    renderer->DrawString(5, y++, m_Narration.substr(start).c_str(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    y += 2;
    
    // Draw Choices
    for (size_t i = 0; i < m_Choices.size(); ++i) {
        WORD color = (i == m_SelectedIndex) ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        if (i == m_SelectedIndex) renderer->DrawString(3, y + (int)i, ">", color);
        renderer->DrawString(5, y + (int)i, m_Choices[i].text.c_str(), color);
    }
    
    renderer->Present();
}

void ExplorationState::SetNode(int nodeId) {
    m_CurrentNodeId = nodeId;
    m_Choices.clear();
    m_SelectedIndex = 0;
    
    switch(nodeId) {
        case 0: Node_Prologue_Tavern(); break;
        case 1: Node_Prologue_TheAttack(); break;
        case 2: Node_Prologue_Evacuation(); break;
        case 3: Node_Prologue_Malakor(); break;
        case 4: Node_Act1_Start(); break;
    }
}

void ExplorationState::Node_Prologue_Tavern() {
    m_Narration = "PROLOGUE: The Call to Action\n\n"
                  "You sit in the bustling frontier tavern of Oakhaven.\n"
                  "The fire crackles, casting a warm glow on the faces around you.\n"
                  "A bard strums a lute nearby, and a gruff dwarf nurses his ale.";
                  
    m_Choices.push_back({"Talk to Elara (The Bard)", [this]() {
        m_Engine->PushState(new DialogState(m_Engine, "Elara", "Oh! Hello there, traveler! Care to hear the Ballad of the Sunken King?"));
    }});
    
    m_Choices.push_back({"Talk to Gruff Ironbelly", [this]() {
        m_Engine->PushState(new DialogState(m_Engine, "Gruff", "Keep yer voice down. A smith's ears need rest same as his hands."));
    }});
    
    m_Choices.push_back({"Finish your drink and step outside", [this]() {
        SetNode(1);
    }});
}

void ExplorationState::Node_Prologue_TheAttack() {
    m_Narration = "As you step outside, the sky suddenly darkens.\n"
                  "Ash begins to fall like snow, suffocating the light.\n"
                  "Screams erupt from the village square as monstrous forms emerge\n"
                  "from the gray blizzard. Ash-hulks! One spots you and charges!";
                  
    m_Choices.push_back({"Draw your weapon and fight!", [this]() {
        Enemy hulk = {"Ash-Hulk", 15, 15, 2, 6};
        m_Engine->PushState(new CombatState(m_Engine, hulk, [this](bool won) {
            if (won) {
                SetNode(2); // Move to evacuation
            } else {
                // Game Over logic
                m_Narration = "You have fallen to the Ash-Hulk... GAME OVER.";
                m_Choices.clear();
                // To keep it simple, just add a choice to restart or quit
            }
        }));
    }});
}

void ExplorationState::Node_Prologue_Evacuation() {
    m_Narration = "The beast crumbles into a pile of lifeless ash.\n"
                  "Villagers are fleeing toward the tavern cellar, but the sturdy\n"
                  "oak doors are too heavy for them to barricade in time.";
                  
    m_Choices.push_back({"Barricade the doors! (STR Check)", [this]() {
        m_Engine->PushState(new SkillCheckState(m_Engine, "STR", 10, [this](bool success) {
            if (success) {
                m_Engine->GetSession()->innocents_saved = true;
                m_Engine->PushState(new DialogState(m_Engine, "Gruff", "Good lad! You saved 'em!"));
            } else {
                m_Engine->GetSession()->innocents_saved = false;
                m_Engine->PushState(new DialogState(m_Engine, "Gruff", "Too slow! Some of 'em didn't make it..."));
            }
            SetNode(3); // Move to Malakor scene
        }));
    }});
}

void ExplorationState::Node_Prologue_Malakor() {
    m_Narration = "Suddenly, the chaos stills. A figure floats above the town square.\n"
                  "Malakor the Ashen. He gestures, and the town elder turns to ash in an instant.\n"
                  "\"This village is unworthy of the new dawn,\" Malakor echoes.\n"
                  "He turns towards the Charred Woods to claim the Heart of the Forest, leaving you behind.";
                  
    m_Choices.push_back({"Swear vengeance and follow him (Proceed to Act 1)", [this]() {
        SetNode(4);
    }});
}

void ExplorationState::Node_Act1_Start() {
    m_Narration = "ACT 1: Crossing the Threshold\n\n"
                  "You stand at the edge of the Charred Woods.\n"
                  "The journey has just begun...\n"
                  "(End of Prologue Demo)";
                  
    m_Choices.push_back({"Return to Main Menu", [this]() {
        m_Engine->ChangeState(new MainMenuState(m_Engine));
    }});
}
