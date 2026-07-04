#pragma once
#include "Player.h"

enum class StoryProgress {
    Prologue,
    Act1,
    Act2,
    Act3,
    Epilogue
};

class GameSession {
public:
    GameSession() 
        : m_Player(nullptr), m_Progress(StoryProgress::Prologue), 
          innocents_saved(false), ward_purified(false), elara_rescued(false) {}
          
    ~GameSession() {
        if (m_Player) delete m_Player;
    }

    void SetPlayer(Player* player) {
        if (m_Player) delete m_Player;
        m_Player = player;
    }
    
    Player* GetPlayer() const { return m_Player; }

    StoryProgress GetProgress() const { return m_Progress; }
    void SetProgress(StoryProgress progress) { m_Progress = progress; }

    // Flags
    bool innocents_saved;
    bool ward_purified;
    bool elara_rescued;

private:
    Player* m_Player;
    StoryProgress m_Progress;
};
