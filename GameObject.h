#pragma once
#include "Spell.h"
#include <Windows.h>
#include <map>
#include <string>
#include <vector>

enum BuffType {
    Internal = 0,
    Aura = 1,
    CombatEnchancer = 2,
    CombatDehancer = 3,
    SpellShield = 4,
    Stun = 5,
    Invisibility = 6,
    Silence = 7,
    Taunt = 8,
    Berserk = 9,
    Polymorph = 10,
    Slow = 11,
    Snare = 12,
    Damage = 13,
    Heal = 14,
    Haste = 15,
    SpellImmunity = 16,
    PhysicalImmunity = 17,
    Invulnerability = 18,
    AttackSpeedSlow = 19,
    NearSight = 20,
    Currency = 21,
    Fear = 22,
    Charm = 23,
    Poison = 24,
    Suppression = 25,
    Blind = 26,
    Counter = 27,
    Shred = 28,
    Flee = 29,
    Knockup = 30,
    Knockback = 31,
    Disarm = 32,
    Grounded = 33,
    Drowsy = 34,
    Asleep = 35,
    Obscured = 36,
    ClickProofToEnemies = 37,
    Unkillable = 38
};

struct BuffInfo {
public:
    BuffInfo() {};
    BuffInfo(std::string name, UINT8 count, UINT8 countAlt, UINT8 countAlt2, UINT8 type, float startTime, float endTime, bool isAlive)
    {
        this->name = name;
        this->count = count;
        this->countAlt = countAlt;
        this->countAlt2 = countAlt2;
        this->type = type;
        this->startTime = startTime;
        this->endTime = endTime;
        this->isAlive = isAlive;
    }
    std::string name;
    int count = 0;
    int countAlt = 0;
    int countAlt2 = 0;
    int type = -1;
    float startTime = 0.f;
    float endTime = 0.f;
    bool isAlive = false;
};

class GameObject {
public:
    void LoadFromMem(DWORD64 base, bool deepLoad = true);
    void LoadChampFromMem(DWORD64 base);
    void LoadBuffFromMem(DWORD64 base);
    BuffInfo* GetBuffByName(std::string buffName);

public:
    DWORD64 address;
    short objIndex;
    std::string name;
    DWORD32 networkId;
    short team;
    float health;
    bool visible;
    bool targetable;
    bool isAlive;

    /// players
    short level;
    Spell Q = Spell(SpellSlot::Q);
    Spell W = Spell(SpellSlot::W);
    Spell E = Spell(SpellSlot::E);
    Spell R = Spell(SpellSlot::R);
    Spell D = Spell(SpellSlot::D);
    Spell F = Spell(SpellSlot::F);
    Spell N_1 = Spell(SpellSlot::N_1);
    Spell N_2 = Spell(SpellSlot::N_2);
    Spell N_3 = Spell(SpellSlot::N_3);
    Spell N_5 = Spell(SpellSlot::N_5);
    Spell N_6 = Spell(SpellSlot::N_6);
    Spell N_7 = Spell(SpellSlot::N_7);
    Spell N_4 = Spell(SpellSlot::N_4);
    std::vector<BuffInfo> buffs;

private:
    static DWORD64 spellSlotPointerBuffer[14];
    static BYTE buffListBuffer[0x90];

protected:
    static const SIZE_T sizeBuff = 0x4FFF;
    static BYTE buff[sizeBuff];

public:
    static const char* ZOMBIES[3];
    static float gameTime;
};