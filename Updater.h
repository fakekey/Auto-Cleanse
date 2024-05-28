#pragma once
#include "Game.h"
#include "json.hpp"

using json = nlohmann::json;

class Updater {
public:
    void Init();
    void FirstStart(std::string& dataFolder);
    void Update(Game& game);

private:
    json settings;
};