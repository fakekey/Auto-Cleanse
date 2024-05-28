#include "Game.h"
#include "GameData.h"
#include "Updater.h"
#include "Utils.h"
#include <conio.h>
#include <stdexcept>

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType) {
    case CTRL_CLOSE_EVENT:
        // Free memory
        delete Baker::GetInstance();
        return TRUE;
    default:
        return FALSE;
    }
}

void MainLoop(Game& game, Updater& updater, std::string& dataFolder)
{
    bool rehook = true;
    bool firstIter = true;
    printf("[i] Waiting for League process...\n");
    while (true) {
        bool isLeagueWindowActive = game.IsLeagueWindowActive();
        try {
            if (rehook) {
                game.HookToProcess();
                rehook = false;
                firstIter = true;
                printf("[i] Found League process. Auto cleanse activated!\n");
            } else {
                game.MakeSnapshot();
                // Shit started
                if (game.gameTime > 2.0f) {
                    // Init shit here
                    if (firstIter) {
                        updater.FirstStart(dataFolder);
                        firstIter = false;
                    }

                    // Update shit here
                    if (isLeagueWindowActive) {
                        updater.Update(game);
                    }
                }
            }
        } catch (const std::runtime_error& ex) {
            printf("[E] %s\n\n", ex.what());
            printf("[i] Waiting for League process...\n");
            rehook = true;
        }

        Sleep(5);
    }
}

int main()
{
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        printf("Could not set control handler\n");
        return _getch();
    }

    if (!Baker::ChangePrivilege("SeDebugPrivilege")) {
        printf("Can't change privilege\n");
        return _getch();
    }

    if (!Baker::GetInstance()->InitializeDevice()) {
        // Free memory
        delete Baker::GetInstance();
        return _getch();
    }

    // Load static data
    std::string dataPath("data");
    GameData::Load(dataPath);
    printf("Loaded static data!\n");

    Game game = Game();
    Updater updater = Updater();
    updater.Init();

    MainLoop(game, updater, dataPath);

    // Free memory
    delete Baker::GetInstance();
    return _getch();
}