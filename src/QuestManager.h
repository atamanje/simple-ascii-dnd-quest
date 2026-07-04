#pragma once
#include "QuestNode.h"
#include <map>
#include <string>

class QuestManager {
public:
    QuestManager();
    
    bool LoadStory(const std::string& filepath);
    
    void SetCurrentNode(int id);
    const QuestNode* GetCurrentNode() const;

private:
    std::map<int, QuestNode> m_Nodes;
    int m_CurrentNodeID;
};
