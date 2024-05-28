#pragma once
#include "SpellInfo.h"
#include <map>

class GameData {
public:
    static void Load(std::string& dataFolder);
    static SpellInfo* GetSpellInfoByName(std::string& name);

private:
    static void LoadSpellData(std::string& path);

public:
    static std::map<std::string, SpellInfo*> Spells;
    static std::map<std::string, SpellInfo*> SpellAlts;
};