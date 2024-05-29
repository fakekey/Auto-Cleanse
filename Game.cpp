#include "Game.h"
#include "Offsets.h"
#include "Utils.h"
#include <stdexcept>

Game::Game()
{
    isChatOpen = false;
    gameTime = 0.0f;
    player = std::make_unique<GameObject>();
    moduleBaseAddr = 0;
}

void Game::ReadPlayer()
{
    DWORD64 localPlayer = Mem::ReadDWORD(moduleBaseAddr + Offsets::LocalPlayer);
    player->LoadFromMem(localPlayer, true);
}

void Game::HookToProcess()
{
    // Get the process ID
    if (!Baker::GetInstance()->GetProcessId()) {
        throw std::runtime_error("Please start game!");
    }

    moduleBaseAddr = Baker::GetInstance()->GetModuleBase();
}

bool Game::IsHookedToProcess()
{
    return gameTime > 2.0f;
}

bool Game::IsLeagueWindowActive()
{
    return Baker::GetInstance()->IsWindowActive();
}

void Game::MakeSnapshot()
{
    Mem::Read(moduleBaseAddr + Offsets::GameTime, &gameTime, sizeof(float));
    // Checking chat
    DWORD64 chatInstance = Mem::ReadDWORD(moduleBaseAddr + Offsets::ChatClient);
    Mem::Read(chatInstance + Offsets::ChatIsOpen, &isChatOpen, sizeof(bool));

    if (gameTime > 2.0f) {
        GameObject::gameTime = gameTime;
        ReadPlayer();
    }
}