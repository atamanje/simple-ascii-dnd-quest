#pragma once
#include <string>
#include <vector>

struct Choice {
    std::string Description;
    std::string StatRequirement; // e.g. "STR", "DEX", "NONE"
    int DifficultyClass;
    int SuccessNodeID;
    int FailureNodeID;
};

struct QuestNode {
    int ID;
    std::string NarrativeText;
    std::vector<Choice> Choices;
};
