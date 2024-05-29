#include "Updater.h"
#include "Utils.h"
#include <fstream>

void Updater::Init()
{
    std::string windowName = Character::RandomString(10);
    SetConsoleTitleA(windowName.c_str());
}

void Updater::FirstStart(std::string& dataFolder)
{
    std::string settingsData = dataFolder + "/Settings.json";
    std::ifstream f(settingsData);
    settings = json::parse(f);
    cleansable.clear();

    if (settings["Berserk"]) {
        cleansable.push_back(BuffType::Berserk);
    }
    if (settings["Charm"]) {
        cleansable.push_back(BuffType::Charm);
    }
    if (settings["Flee"]) {
        cleansable.push_back(BuffType::Flee);
    }
    if (settings["Fear"]) {
        cleansable.push_back(BuffType::Fear);
    }
    if (settings["Taunt"]) {
        cleansable.push_back(BuffType::Taunt);
    }
    if (settings["Grounded"]) {
        cleansable.push_back(BuffType::Grounded);
    }
    if (settings["Snare"]) {
        cleansable.push_back(BuffType::Snare);
    }
    if (settings["Silence"]) {
        cleansable.push_back(BuffType::Silence);
    }
    if (settings["Polymorph"]) {
        cleansable.push_back(BuffType::Polymorph);
    }
    if (settings["Drowsy"]) {
        cleansable.push_back(BuffType::Drowsy);
    }
    if (settings["Asleep"]) {
        cleansable.push_back(BuffType::Asleep);
    }
    if (settings["Stun"]) {
        cleansable.push_back(BuffType::Stun);
    }
    if (settings["Blind"]) {
        cleansable.push_back(BuffType::Blind);
    }
    if (settings["NearSight"]) {
        cleansable.push_back(BuffType::NearSight);
    }
}

void Updater::Update(Game& game)
{
    if (!settings["Enabled"]) {
        return;
    }

    GameObject& me = *(game.player.get());
    if (me.isAlive && !game.isChatOpen) {
        bool isShouldCleanse = false;
        for (auto& buff : me.buffs) {
            if (InArray((BuffType)buff.type, cleansable) && buff.isAlive) {
                isShouldCleanse = true;
                break;
            }
        }

        if (settings["SummonerExhaust"]) {
            BuffInfo* exhaust = me.GetBuffByName("SummonerExhaust");
            if (exhaust != nullptr && exhaust->isAlive) {
                isShouldCleanse = true;
            }
        }

        if (settings["SummonerDot"]) {
            BuffInfo* sdot = me.GetBuffByName("SummonerDot");
            if (sdot != nullptr && sdot->isAlive) {
                isShouldCleanse = true;
            }
        }

        if (isShouldCleanse) {
            if (me.D.summonerSpellType == SummonerSpellType::CLEANSE && me.D.IsReady(game.gameTime)) {
                me.D.Trigger();
            } else if (me.F.summonerSpellType == SummonerSpellType::CLEANSE && me.F.IsReady(game.gameTime)) {
                me.F.Trigger();
            }
        }
    }
}
