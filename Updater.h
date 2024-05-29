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
    std::vector<BuffType> cleansable {};
    bool InArray(const BuffType& value, const std::vector<BuffType>& array)
    {
        return std::find(array.begin(), array.end(), value) != array.end();
    }
};