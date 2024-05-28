#pragma once
#include "GameObject.h"
#include <memory>

class Game {
public:
    Game();
    void MakeSnapshot();
    void ReadPlayer();
    void HookToProcess();
    bool IsHookedToProcess();
    bool IsLeagueWindowActive();

public:
    bool isChatOpen;
    float gameTime;
    std::unique_ptr<GameObject> player;

private:
    DWORD64 moduleBaseAddr;
};