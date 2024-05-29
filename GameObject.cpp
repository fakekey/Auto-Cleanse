#include "GameObject.h"
#include "GameData.h"
#include "Offsets.h"
#include "Utils.h"

BYTE GameObject::buff[GameObject::sizeBuff] = {};
BYTE GameObject::buffListBuffer[0x90] = {};
const char* GameObject::ZOMBIES[3] = { "Sion", "KogMaw", "Karthus" };
DWORD64 GameObject::spellSlotPointerBuffer[7] = {};
float GameObject::gameTime = 0.f;

bool IsZombie(const std::string& name)
{
    for (const char* zombie : GameObject::ZOMBIES) {
        if (name == zombie) {
            return true;
        }
    }
    return false;
}

BuffInfo* GameObject::GetBuffByName(std::string buffName)
{
    float buffEndTime = 0.f;
    BuffInfo* rightBuff = nullptr;
    for (auto cbuff = buffs.begin(); cbuff != buffs.end(); ++cbuff) {
        if (cbuff->name.compare(buffName) == 0 && cbuff->endTime > buffEndTime) {
            buffEndTime = cbuff->endTime;
            rightBuff = cbuff._Ptr;
        }
    }
    return rightBuff;
}

void GameObject::LoadFromMem(DWORD64 base, bool deepLoad)
{
    address = base;
    Mem::Read(base, buff, sizeBuff);

    memcpy(&objIndex, &buff[Offsets::ObjIndex], sizeof(short));
    memcpy(&networkId, &buff[Offsets::ObjNetworkID], sizeof(DWORD32));
    memcpy(&health, &buff[Offsets::ObjHealth], sizeof(float));
    memcpy(&visible, &buff[Offsets::ObjVisible], sizeof(bool));
    memcpy(&targetable, &buff[Offsets::ObjTargetable], sizeof(bool));

    if (deepLoad) {
        char nameBuff[50];
        memset(nameBuff, 0, sizeof(nameBuff));
        Mem::Read(Mem::ReadDWORDFromBuffer(buff, Offsets::ObjName), nameBuff, 50);

        name = std::string("");
        if (Character::ContainsOnlyASCII(nameBuff, 50)) {
            name = Character::ToLower(std::string(nameBuff));
        } else {
            name = std::string("");
        }

        if (name == "") {
            char nameBuff2[50];
            memset(nameBuff2, 0, sizeof(nameBuff2));
            Mem::Read(address + Offsets::ObjName, nameBuff2, 50);

            if (Character::ContainsOnlyASCII(nameBuff2, 50)) {
                name = Character::ToLower(std::string(nameBuff2));
            } else {
                name = std::string("");
            }
        }
    }

    // Check if alive
    DWORD32 spawnCount;
    memcpy(&spawnCount, &buff[Offsets::ObjSpawnCount], sizeof(int));
    isAlive = IsZombie(name) ? health > 0 : spawnCount % 2 == 0;

    LoadChampFromMem(base);
    LoadBuffFromMem(base);
}

void GameObject::LoadChampFromMem(DWORD64 base)
{
    memcpy(&spellSlotPointerBuffer, &buff[Offsets::ObjSpellBook], sizeof(DWORD64) * 6);

    Q.LoadFromMem(spellSlotPointerBuffer[0]);
    W.LoadFromMem(spellSlotPointerBuffer[1]);
    E.LoadFromMem(spellSlotPointerBuffer[2]);
    R.LoadFromMem(spellSlotPointerBuffer[3]);
    D.LoadFromMem(spellSlotPointerBuffer[4]);
    F.LoadFromMem(spellSlotPointerBuffer[5]);

    memcpy(&level, &buff[Offsets::ObjLevel], sizeof(short));
}

void GameObject::LoadBuffFromMem(DWORD64 base)
{
    DWORD64 buffArrayBgn, buffArrayEnd;
    Mem::Read(address + Offsets::ObjBuffManager + Offsets::BuffArrayBegin, &buffArrayBgn, sizeof(DWORD64));
    Mem::Read(address + Offsets::ObjBuffManager + Offsets::BuffArrayEnd, &buffArrayEnd, sizeof(DWORD64));

    int i = 0;
    buffs.clear();
    buffs.reserve(i);
    char buffnamebuffer[240];

    for (DWORD64 pBuffPtr = buffArrayBgn; pBuffPtr != buffArrayEnd; pBuffPtr += 0x8) {
        i++;
        DWORD64 buffInstance = Mem::ReadDWORD(pBuffPtr);
        Mem::Read(buffInstance, buffListBuffer, 0x90);

        DWORD64 buffInfo, buffNamePtr;
        memcpy(&buffInfo, &buffListBuffer[Offsets::BuffEntryBuff], sizeof(DWORD64));
        if (buffInfo == NULL || (DWORD64)buffInfo <= 0x1000) {
            continue;
        }
        Mem::Read(buffInfo + Offsets::BuffName, &buffNamePtr, sizeof(DWORD64));
        memset(buffnamebuffer, 0, sizeof(buffnamebuffer));

        Mem::Read(buffNamePtr, buffnamebuffer, 240);
        if (std::string(buffnamebuffer) == "" || !Character::ContainsOnlyASCII(buffnamebuffer, 240)) {
            continue;
        }

        bool isAlive = false;
        float buffStartTime;
        float buffEndTime;
        UINT8 buffCount;
        UINT8 buffCountAlt;
        UINT8 buffCountAlt2;
        UINT8 bufftype;

        memcpy(&buffStartTime, &buffListBuffer[Offsets::BuffEntryBuffStartTime], sizeof(float));
        memcpy(&buffEndTime, &buffListBuffer[Offsets::BuffEntryBuffEndTime], sizeof(float));
        memcpy(&buffCount, &buffListBuffer[Offsets::BuffEntryBuffCount], sizeof(UINT8));
        memcpy(&buffCountAlt, &buffListBuffer[Offsets::BuffEntryBuffCountAlt], sizeof(UINT8));
        memcpy(&buffCountAlt2, &buffListBuffer[Offsets::BuffEntryBuffCountAlt2], sizeof(UINT8));
        memcpy(&bufftype, &buffListBuffer[Offsets::BuffType], sizeof(UINT8));

        if (buffEndTime > gameTime) {
            isAlive = true;
        }

        buffs.push_back(BuffInfo(buffnamebuffer, buffCount, buffCountAlt, buffCountAlt2, bufftype, buffStartTime, buffEndTime, isAlive));
    }
}