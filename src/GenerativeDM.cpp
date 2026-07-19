#include "GenerativeDM.h"
#include "GeminiClient.h"
#include <iostream>

using json = nlohmann::json;

GenerativeDM::GenerativeDM(const Player& player) 
    : m_Player(player) 
{
    m_ConversationHistory = json::array();
}

std::string GenerativeDM::GetSystemPrompt() const {
    std::string speciesStr = "Human";
    if (m_Player.GetSpecies() == Species::Elf) speciesStr = "Elf";
    else if (m_Player.GetSpecies() == Species::Dwarf) speciesStr = "Dwarf";
    else if (m_Player.GetSpecies() == Species::Orc) speciesStr = "Orc";

    std::string classStr = "Fighter";
    if (m_Player.GetClass() == CharacterClass::Rogue) classStr = "Rogue";
    else if (m_Player.GetClass() == CharacterClass::Wizard) classStr = "Wizard";

    std::string prompt = 
        "You are a Dungeon Master for a text-based ASCII D&D game. "
        "The player's name is " + m_Player.GetName() + ". "
        "They are a " + speciesStr + " " + classStr + " "
        "with stats: STR:" + std::to_string(m_Player.GetStats().Strength) + " "
        "DEX:" + std::to_string(m_Player.GetStats().Dexterity) + " "
        "CON:" + std::to_string(m_Player.GetStats().Constitution) + " "
        "INT:" + std::to_string(m_Player.GetStats().Intelligence) + " "
        "WIS:" + std::to_string(m_Player.GetStats().Wisdom) + " "
        "CHA:" + std::to_string(m_Player.GetStats().Charisma) + ". "
        "You MUST respond ONLY with a valid JSON object matching exactly this schema:\n"
        "{\n"
        "  \"requires_roll\": boolean,\n"
        "  \"stat\": string (e.g. \"STR\", \"DEX\", only if requires_roll is true),\n"
        "  \"dc\": integer (Difficulty Class, e.g. 15, only if requires_roll is true),\n"
        "  \"narrative\": string (Your description of what happens or the current scene)\n"
        "}\n"
        "If the player's action is simple or guaranteed to succeed/fail, set requires_roll to false and narrate the outcome. "
        "If it is risky, set requires_roll to true and provide the stat and DC, and DO NOT narrate the outcome yet (wait for the roll result).";
    
    return prompt;
}

DMResponse GenerativeDM::TakeAction(const std::string& playerAction) {
    if (playerAction.empty()) {
        m_ConversationHistory.push_back({
            {"role", "user"},
            {"parts", json::array({{{"text", "Start the adventure."}}})}
        });
    } else {
        m_ConversationHistory.push_back({
            {"role", "user"},
            {"parts", json::array({{{"text", "I attempt to: " + playerAction}}})}
        });
    }

    std::string jsonResponse = GeminiClient::GenerateContent(GetSystemPrompt(), m_ConversationHistory);
    
    if (!jsonResponse.empty()) {
        m_ConversationHistory.push_back({
            {"role", "model"},
            {"parts", json::array({{{"text", jsonResponse}}})}
        });
    }
    
    return ParseGeminiResponse(jsonResponse);
}

DMResponse GenerativeDM::ResolveRoll(int rollTotal) {
    m_ConversationHistory.push_back({
        {"role", "user"},
        {"parts", json::array({{{"text", "I rolled a total of " + std::to_string(rollTotal) + ". What happens?"}}})}
    });

    std::string jsonResponse = GeminiClient::GenerateContent(GetSystemPrompt(), m_ConversationHistory);
    
    if (!jsonResponse.empty()) {
        m_ConversationHistory.push_back({
            {"role", "model"},
            {"parts", json::array({{{"text", jsonResponse}}})}
        });
    }
    
    return ParseGeminiResponse(jsonResponse);
}

DMResponse GenerativeDM::ParseGeminiResponse(const std::string& jsonString) {
    DMResponse res;
    res.RequiresRoll = false;
    res.DC = 10;
    res.Stat = "STR";
    res.Narrative = "The DM is thinking...";

    if (jsonString.empty()) {
        res.Narrative = "Error: No response from the DM.";
        return res;
    }

    std::string cleanJson = jsonString;
    size_t firstBrace = cleanJson.find_first_of('{');
    size_t lastBrace = cleanJson.find_last_of('}');
    if (firstBrace != std::string::npos && lastBrace != std::string::npos && lastBrace > firstBrace) {
        cleanJson = cleanJson.substr(firstBrace, lastBrace - firstBrace + 1);
    }

    try {
        json parsed = json::parse(cleanJson);
        res.RequiresRoll = parsed.value("requires_roll", false);
        if (res.RequiresRoll) {
            res.Stat = parsed.value("stat", "STR");
            res.DC = parsed.value("dc", 10);
        }
        res.Narrative = parsed.value("narrative", "...");
    } catch (...) {
        res.Narrative = "Error: The DM spoke in tongues (Invalid JSON). Raw: " + jsonString;
    }
    return res;
}
