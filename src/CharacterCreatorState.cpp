#include "CharacterCreatorState.h"
#include "Engine.h"
#include "Input.h"
#include "Renderer.h"
#include "Dice.h"
#include "MainMenuState.h"
#include <windows.h>

CharacterCreatorState::CharacterCreatorState(Engine* engine)
    : m_Engine(engine), m_Step(CreationStep::Name), m_SelectedSpeciesIndex(0), m_SelectedClassIndex(0),
      m_CursorTimer(0.0f), m_CursorVisible(true) 
{
    m_SpeciesOptions = { "Human", "Elf", "Dwarf", "Orc" };
    m_ClassOptions = { "Fighter", "Rogue", "Wizard" };
}

void CharacterCreatorState::RollStats() {
    m_RolledStats.Strength = Dice::RollStat();
    m_RolledStats.Dexterity = Dice::RollStat();
    m_RolledStats.Constitution = Dice::RollStat();
    m_RolledStats.Intelligence = Dice::RollStat();
    m_RolledStats.Wisdom = Dice::RollStat();
    m_RolledStats.Charisma = Dice::RollStat();
}

void CharacterCreatorState::ProcessInput() {
    if (m_Step == CreationStep::Name) {
        // Typing characters A-Z
        for (int i = 'A'; i <= 'Z'; i++) {
            if (Input::IsKeyPressed(i)) {
                if (m_Name.length() < 15) { // Limit name length
                    m_Name += (char)i;
                }
            }
        }
        if (Input::IsKeyPressed(VK_SPACE)) {
            if (m_Name.length() < 15) m_Name += ' ';
        }
        if (Input::IsKeyPressed(VK_BACK)) {
            if (!m_Name.empty()) {
                m_Name.pop_back();
            }
        }
        if (Input::IsKeyPressed(VK_RETURN)) {
            if (!m_Name.empty()) {
                m_Step = CreationStep::Species;
            }
        }
    }
    else if (m_Step == CreationStep::Species) {
        if (Input::IsKeyPressed(VK_UP)) {
            m_SelectedSpeciesIndex--;
            if (m_SelectedSpeciesIndex < 0) m_SelectedSpeciesIndex = (int)m_SpeciesOptions.size() - 1;
        }
        else if (Input::IsKeyPressed(VK_DOWN)) {
            m_SelectedSpeciesIndex++;
            if (m_SelectedSpeciesIndex >= (int)m_SpeciesOptions.size()) m_SelectedSpeciesIndex = 0;
        }
        else if (Input::IsKeyPressed(VK_RETURN)) {
            m_Step = CreationStep::Class;
        }
    }
    else if (m_Step == CreationStep::Class) {
        if (Input::IsKeyPressed(VK_UP)) {
            m_SelectedClassIndex--;
            if (m_SelectedClassIndex < 0) m_SelectedClassIndex = (int)m_ClassOptions.size() - 1;
        }
        else if (Input::IsKeyPressed(VK_DOWN)) {
            m_SelectedClassIndex++;
            if (m_SelectedClassIndex >= (int)m_ClassOptions.size()) m_SelectedClassIndex = 0;
        }
        else if (Input::IsKeyPressed(VK_RETURN)) {
            RollStats();
            m_Step = CreationStep::RollStats;
        }
    }
    else if (m_Step == CreationStep::RollStats) {
        if (Input::IsKeyPressed('R')) {
            RollStats(); // Re-roll
        }
        else if (Input::IsKeyPressed(VK_RETURN)) {
            m_Step = CreationStep::Confirm;
        }
    }
    else if (m_Step == CreationStep::Confirm) {
        if (Input::IsKeyPressed(VK_RETURN)) {
            // Wait for enter to return to main menu or transition to game
            m_Engine->ChangeState(new MainMenuState(m_Engine));
        }
    }
}

void CharacterCreatorState::Update(float deltaTime) {
    m_CursorTimer += deltaTime;
    if (m_CursorTimer >= 0.5f) {
        m_CursorVisible = !m_CursorVisible;
        m_CursorTimer = 0.0f;
    }
}

void CharacterCreatorState::Render(Renderer* renderer) {
    renderer->Clear();
    
    renderer->DrawString(renderer->GetWidth() / 2 - 12, 2, "--- CHARACTER CREATION ---", FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    int y = 5;
    
    // 1. Name
    WORD nameColor = (m_Step == CreationStep::Name) ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    renderer->DrawString(10, y, "Name:", nameColor);
    
    std::string displayString = m_Name;
    if (m_Step == CreationStep::Name && m_CursorVisible) {
        displayString += "_";
    }
    renderer->DrawString(30, y, displayString.c_str(), nameColor);
    
    y += 2;
    
    // 2. Species
    if (m_Step >= CreationStep::Species) {
        WORD speciesColor = (m_Step == CreationStep::Species) ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        renderer->DrawString(10, y, "Species:", speciesColor);
        
        if (m_Step == CreationStep::Species) {
            for (size_t i = 0; i < m_SpeciesOptions.size(); ++i) {
                WORD color = (i == m_SelectedSpeciesIndex) ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                if (i == m_SelectedSpeciesIndex) renderer->DrawString(28, y + i, ">", color);
                renderer->DrawString(30, y + i, m_SpeciesOptions[i].c_str(), color);
            }
            y += m_SpeciesOptions.size();
        } else {
            renderer->DrawString(30, y, m_SpeciesOptions[m_SelectedSpeciesIndex].c_str(), speciesColor);
            y += 1;
        }
    } else {
        y += 1; // placeholder for alignment
    }
    
    y += 1;

    // 3. Class
    if (m_Step >= CreationStep::Class) {
        WORD classColor = (m_Step == CreationStep::Class) ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        renderer->DrawString(10, y, "Class:", classColor);
        
        if (m_Step == CreationStep::Class) {
            for (size_t i = 0; i < m_ClassOptions.size(); ++i) {
                WORD color = (i == m_SelectedClassIndex) ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                if (i == m_SelectedClassIndex) renderer->DrawString(28, y + i, ">", color);
                renderer->DrawString(30, y + i, m_ClassOptions[i].c_str(), color);
            }
            y += m_ClassOptions.size();
        } else {
            renderer->DrawString(30, y, m_ClassOptions[m_SelectedClassIndex].c_str(), classColor);
            y += 1;
        }
    } else {
        y += 1; // placeholder
    }
    
    y += 1;

    // 4. Stats
    if (m_Step >= CreationStep::RollStats) {
        WORD statsColor = (m_Step == CreationStep::RollStats) ? (FOREGROUND_GREEN | FOREGROUND_INTENSITY) : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        renderer->DrawString(10, y, "Ability Scores:", statsColor);
        
        char buffer[32];
        sprintf_s(buffer, "STR: %d", m_RolledStats.Strength);
        renderer->DrawString(30, y++, buffer, statsColor);
        sprintf_s(buffer, "DEX: %d", m_RolledStats.Dexterity);
        renderer->DrawString(30, y++, buffer, statsColor);
        sprintf_s(buffer, "CON: %d", m_RolledStats.Constitution);
        renderer->DrawString(30, y++, buffer, statsColor);
        sprintf_s(buffer, "INT: %d", m_RolledStats.Intelligence);
        renderer->DrawString(30, y++, buffer, statsColor);
        sprintf_s(buffer, "WIS: %d", m_RolledStats.Wisdom);
        renderer->DrawString(30, y++, buffer, statsColor);
        sprintf_s(buffer, "CHA: %d", m_RolledStats.Charisma);
        renderer->DrawString(30, y++, buffer, statsColor);
        
        if (m_Step == CreationStep::RollStats) {
            renderer->DrawString(10, y + 1, "[R] Re-roll   [ENTER] Confirm", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        }
    }
    
    // 5. Confirm
    if (m_Step == CreationStep::Confirm) {
        renderer->DrawString(renderer->GetWidth() / 2 - 15, renderer->GetHeight() - 4, "CHARACTER CREATED SUCCESSFULLY!", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        renderer->DrawString(renderer->GetWidth() / 2 - 10, renderer->GetHeight() - 3, "[ENTER] to return", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    
    renderer->Present();
}
