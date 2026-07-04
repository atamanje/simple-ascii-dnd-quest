#include "QuestManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

QuestManager::QuestManager() : m_CurrentNodeID(1) {}

bool QuestManager::LoadStory(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    json j;
    try {
        file >> j;
        
        for (const auto& nodeJson : j["nodes"]) {
            QuestNode node;
            node.ID = nodeJson["id"].get<int>();
            node.NarrativeText = nodeJson["narrative"].get<std::string>();
            node.ArtAssetPath = nodeJson.value("art", "");
            
            if (nodeJson.contains("choices")) {
                for (const auto& choiceJson : nodeJson["choices"]) {
                    Choice choice;
                    choice.Description = choiceJson["description"].get<std::string>();
                    choice.StatRequirement = choiceJson.value("stat", "NONE");
                    choice.DifficultyClass = choiceJson.value("dc", 0);
                    choice.SuccessNodeID = choiceJson["success_node"].get<int>();
                    choice.FailureNodeID = choiceJson.value("failure_node", choice.SuccessNodeID);
                    
                    node.Choices.push_back(choice);
                }
            }
            
            m_Nodes[node.ID] = node;
        }
        
    } catch (const std::exception& e) {
        // TODO: Handle/Log JSON parsing errors
        return false;
    }
    
    if (!m_Nodes.empty() && m_Nodes.find(1) != m_Nodes.end()) {
        m_CurrentNodeID = 1;
    }
    
    return true;
}

void QuestManager::SetCurrentNode(int id) {
    if (m_Nodes.find(id) != m_Nodes.end()) {
        m_CurrentNodeID = id;
    }
}

const QuestNode* QuestManager::GetCurrentNode() const {
    auto it = m_Nodes.find(m_CurrentNodeID);
    if (it != m_Nodes.end()) {
        return &it->second;
    }
    return nullptr;
}
