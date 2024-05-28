#pragma once
#include "GameData.h"
#include "Input.h"
#include "SpellInfo.h"
#include <Windows.h>
#include <map>
#include <string>

enum class SpellSlot {
    Q = 0,
    W,
    E,
    R,
    D,
    F
};

enum SummonerSpellType {
    NONE,
    GHOST,
    HEAL,
    BARRIER,
    EXHAUST,
    CLARITY,
    SNOWBALL,
    FLASH,
    TELEPORT,
    CLEANSE,
    IGNITE,
    SMITE,
    RECALL,
    WARD
};

class Spell {
public:
    Spell(SpellSlot slot)
    {
        this->name = "";
        this->slot = slot;
        this->level = 0;
        this->readyAt = 0.f;
        this->info = nullptr;
    }

    float GetRemainingCooldown(float gameTime);
    const char* GetTypeStr();
    void LoadFromMem(DWORD64 base, bool deepLoad = true);
    void Trigger();
    bool IsReady(float gameTime);

public:
    std::string name;
    SpellSlot slot;
    int level;
    float readyAt;
    SpellInfo* info;
    SummonerSpellType summonerSpellType;

private:
    static BYTE buffer[0x150];
    static const char* spellTypeName[6];
    static const HKey spellSlotKey[6];
    static std::map<std::string, SummonerSpellType> summonerSpellTypeDict;
};