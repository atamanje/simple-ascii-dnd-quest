#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "Player.h"

// Replaces QuestNode for generative workflow
struct DMResponse {
    bool RequiresRoll;
    std::string Stat; // "STR", "DEX", etc. (Only used if RequiresRoll is true)
    int DC;           // Difficulty Class (Only used if RequiresRoll is true)
    std::string Narrative;
};

class GenerativeDM {
public:
    GenerativeDM(const Player& player);
    
    // Sends the player's typed action to Gemini. 
    // `playerAction` is what the user typed. Leave empty for the very first scene.
    // Returns the structured response from the DM.
    DMResponse TakeAction(const std::string& playerAction);

    // Call this if the previous action required a roll, to tell the DM what the result was
    DMResponse ResolveRoll(int rollTotal);

private:
    Player m_Player;
    nlohmann::json m_ConversationHistory; // Array of {"role": "...", "parts": ...}
    
    std::string GetSystemPrompt() const;
    DMResponse ParseGeminiResponse(const std::string& jsonString);
};
