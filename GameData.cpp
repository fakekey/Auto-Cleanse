#include "GameData.h"
#include "Utils.h"
#include "json.hpp"
#include <filesystem>
#include <fstream>

using json = nlohmann::json;

std::map<std::string, SpellInfo*> GameData::Spells = {};
std::map<std::string, SpellInfo*> GameData::SpellAlts = {};

SpellInfo* GameData::GetSpellInfoByName(std::string& name)
{
    auto it = Spells.find(name);
    if (it != Spells.end()) {
        return it->second;
    }
    it = SpellAlts.find(name);
    if (it != SpellAlts.end()) {
        return it->second;
    }
    return nullptr;
}

void GameData::Load(std::string& dataFolder)
{
    std::string spellData = dataFolder + "/SpellData.json";
    LoadSpellData(spellData);
}

void GameData::LoadSpellData(std::string& path)
{
    std::ifstream f(path);
    json data = json::parse(f);

    for (auto& spell : data) {
        SpellInfo* info = new SpellInfo();

        info->name = Character::ToLower(spell["name"]);
        info->nameAlt = Character::ToLower(spell["nameAlt"]);
        info->icon = Character::ToLower(spell["icon"]);
        info->flags = spell["flags"];
        info->delay = spell["delay"];
        info->castRange = spell["castRange"];
        info->castRadius = spell["castRadius"];
        info->width = spell["width"];
        info->height = spell["height"];
        info->speed = spell["speed"];
        info->travelTime = spell["travelTime"];
        info->type = spell["type"];
        info->projectDestination = spell["projectDestination"];

        Spells[info->name] = info;
        SpellAlts[info->nameAlt] = info;
    }
}