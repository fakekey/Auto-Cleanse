#include "Spell.h"
#include "GameData.h"
#include "Offsets.h"
#include "Utils.h"

BYTE Spell::buffer[0x150];
const char* Spell::spellTypeName[13] = { "Q", "W", "E", "R", "D", "F", "1", "2", "3", "5", "6", "7", "4" };
const HKey Spell::spellSlotKey[13] = { HKey::Q, HKey::W, HKey::E, HKey::R, HKey::D, HKey::F, HKey::N_1, HKey::N_2, HKey::N_3, HKey::N_5, HKey::N_6, HKey::N_7, HKey::N_4 };

std::map<std::string, SummonerSpellType> Spell::summonerSpellTypeDict = {
    { std::string("summonerhaste"), SummonerSpellType::GHOST },
    { std::string("summonerheal"), SummonerSpellType::HEAL },
    { std::string("summonerbarrier"), SummonerSpellType::BARRIER },
    { std::string("summonerexhaust"), SummonerSpellType::EXHAUST },
    { std::string("summonermana"), SummonerSpellType::CLARITY },
    { std::string("summonermark"), SummonerSpellType::SNOWBALL },
    { std::string("summonerflash"), SummonerSpellType::FLASH },
    { std::string("summonerboost"), SummonerSpellType::CLEANSE },
    { std::string("summonerdot"), SummonerSpellType::IGNITE },
    { std::string("summonerrecall"), SummonerSpellType::RECALL },
    { std::string("quicksilversash"), SummonerSpellType::QSS },
    { std::string("itemmercurial"), SummonerSpellType::QSS },
};

float Spell::GetRemainingCooldown(float gameTime)
{
    return (readyAt > gameTime ? readyAt - gameTime : 0.f);
}

const char* Spell::GetTypeStr()
{
    return spellTypeName[(int)slot];
}

void Spell::Trigger()
{
    return Input::PressKey(spellSlotKey[(int)slot]);
}

bool Spell::IsReady(float gameTime)
{
    return level > 0 && GetRemainingCooldown(gameTime) == 0.f;
}

void Spell::LoadFromMem(DWORD64 base, bool deepLoad)
{
    Mem::Read(base, buffer, 0x150);

    memcpy(&readyAt, buffer + Offsets::SpellSlotTime, sizeof(float));
    memcpy(&level, buffer + Offsets::SpellSlotLevel, sizeof(int));

    DWORD64 spellInfoPtr;
    memcpy(&spellInfoPtr, buffer + Offsets::SpellSlotSpellInfo, sizeof(DWORD64));

    DWORD64 spellDataPtr = Mem::ReadDWORD(spellInfoPtr + Offsets::SpellInfoSpellData);
    DWORD64 spellNamePtr = Mem::ReadDWORD(spellDataPtr + Offsets::SpellDataSpellName);

    char buff[50];
    memset(buff, 0, sizeof(buff));
    Mem::Read(spellNamePtr, buff, 50);
    if (Character::ContainsOnlyASCII(buff, 50)) {
        name = Character::ToLower(std::string(buff));
    } else {
        name = std::string("");
    }

    auto it = summonerSpellTypeDict.find(name.c_str());
    if (it != summonerSpellTypeDict.end())
        summonerSpellType = it->second;

    info = GameData::GetSpellInfoByName(name);
}