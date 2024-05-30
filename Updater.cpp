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
    qssable.clear();

    if (settings["CleanseCuongLoan"]) {
        cleansable.push_back(BuffType::Berserk);
    }
    if (settings["CleanseMeHoac"]) {
        cleansable.push_back(BuffType::Charm);
    }
    if (settings["CleanseBoChay"]) {
        cleansable.push_back(BuffType::Flee);
    }
    if (settings["CleanseHoangSo"]) {
        cleansable.push_back(BuffType::Fear);
    }
    if (settings["CleanseKhieuKhich"]) {
        cleansable.push_back(BuffType::Taunt);
    }
    if (settings["CleanseSaLay"]) {
        cleansable.push_back(BuffType::Grounded);
    }
    if (settings["CleanseTroi"]) {
        cleansable.push_back(BuffType::Snare);
    }
    if (settings["CleanseCamLang"]) {
        cleansable.push_back(BuffType::Silence);
    }
    if (settings["CleanseBienHinh"]) {
        cleansable.push_back(BuffType::Polymorph);
    }
    if (settings["CleanseBuonNgu"]) {
        cleansable.push_back(BuffType::Drowsy);
    }
    if (settings["CleanseNgu"]) {
        cleansable.push_back(BuffType::Asleep);
    }
    if (settings["CleanseChoang"]) {
        cleansable.push_back(BuffType::Stun);
    }
    if (settings["CleanseMu"]) {
        cleansable.push_back(BuffType::Blind);
    }

    // QSS
    if (settings["QSSCuongLoan"]) {
        qssable.push_back(BuffType::Berserk);
    }
    if (settings["QSSMeHoac"]) {
        qssable.push_back(BuffType::Charm);
    }
    if (settings["QSSBoChay"]) {
        qssable.push_back(BuffType::Flee);
    }
    if (settings["QSSHoangSo"]) {
        qssable.push_back(BuffType::Fear);
    }
    if (settings["QSSKhieuKhich"]) {
        qssable.push_back(BuffType::Taunt);
    }
    if (settings["QSSSaLay"]) {
        qssable.push_back(BuffType::Grounded);
    }
    if (settings["QSSTroi"]) {
        qssable.push_back(BuffType::Snare);
    }
    if (settings["QSSCamLang"]) {
        qssable.push_back(BuffType::Silence);
    }
    if (settings["QSSBienHinh"]) {
        qssable.push_back(BuffType::Polymorph);
    }
    if (settings["QSSBuonNgu"]) {
        qssable.push_back(BuffType::Drowsy);
    }
    if (settings["QSSNgu"]) {
        qssable.push_back(BuffType::Asleep);
    }
    if (settings["QSSChoang"]) {
        qssable.push_back(BuffType::Stun);
    }
    if (settings["QSSMu"]) {
        qssable.push_back(BuffType::Blind);
    }
    if (settings["QSSCanThi"]) {
        qssable.push_back(BuffType::NearSight);
    }
    if (settings["QSSApChe"]) {
        qssable.push_back(BuffType::Suppression);
    }
}

void Updater::Update(Game& game)
{
    GameObject& me = *(game.player.get());
    if (me.isAlive && !game.isChatOpen) {
        if (settings["Cleanse"]) {
            bool isShouldCleanse = false;
            for (auto& buff : me.buffs) {
                if (InArray((BuffType)buff.type, cleansable) && buff.isAlive) {
                    isShouldCleanse = true;
                    break;
                }
            }

            if (settings["CleanseKietSuc"]) {
                BuffInfo* exhaust = me.GetBuffByName("SummonerExhaust");
                if (exhaust != nullptr && exhaust->isAlive) {
                    isShouldCleanse = true;
                }
            }

            if (settings["CleanseThieuDot"]) {
                BuffInfo* ignite = me.GetBuffByName("SummonerDot");
                if (ignite != nullptr && ignite->isAlive) {
                    isShouldCleanse = true;
                }
            }

            if (isShouldCleanse) {
                // Look for Airborne
                if (!settings["CleanseChoangLucHatTung"]) {
                    for (auto& cbuff : me.buffs) {
                        if (cbuff.type == BuffType::Knockup && cbuff.isAlive) {
                            Sleep(100);
                            return;
                        }
                        if (cbuff.type == BuffType::Knockback && cbuff.isAlive) {
                            Sleep(100);
                            return;
                        }
                    }
                }

                if (me.D.summonerSpellType == SummonerSpellType::CLEANSE && me.D.IsReady(game.gameTime)) {
                    me.D.Trigger();
                    Sleep(1000);
                    return;
                } else if (me.F.summonerSpellType == SummonerSpellType::CLEANSE && me.F.IsReady(game.gameTime)) {
                    me.F.Trigger();
                    Sleep(1000);
                    return;
                }
            }
        }

        if (settings["QSS"]) {
            bool isShouldQss = false;
            for (auto& buff : me.buffs) {
                if (InArray((BuffType)buff.type, qssable) && buff.isAlive) {
                    isShouldQss = true;
                    break;
                }
            }

            if (isShouldQss) {
                // Look for Airborne
                if (!settings["QSSChoangLucHatTung"]) {
                    for (auto& cbuff : me.buffs) {
                        if (cbuff.type == BuffType::Knockup && cbuff.isAlive) {
                            Sleep(100);
                            return;
                        }
                        if (cbuff.type == BuffType::Knockback && cbuff.isAlive) {
                            Sleep(100);
                            return;
                        }
                    }
                }

                if (me.N_1.summonerSpellType == SummonerSpellType::QSS && me.N_1.IsReady(game.gameTime)) {
                    me.N_1.Trigger();
                    Sleep(1000);
                    return;
                } else if (me.N_2.summonerSpellType == SummonerSpellType::QSS && me.N_2.IsReady(game.gameTime)) {
                    me.N_2.Trigger();
                    Sleep(1000);
                    return;
                } else if (me.N_3.summonerSpellType == SummonerSpellType::QSS && me.N_3.IsReady(game.gameTime)) {
                    me.N_3.Trigger();
                    Sleep(1000);
                    return;
                } else if (me.N_5.summonerSpellType == SummonerSpellType::QSS && me.N_5.IsReady(game.gameTime)) {
                    me.N_5.Trigger();
                    Sleep(1000);
                    return;
                } else if (me.N_6.summonerSpellType == SummonerSpellType::QSS && me.N_6.IsReady(game.gameTime)) {
                    me.N_6.Trigger();
                    Sleep(1000);
                    return;
                } else if (me.N_7.summonerSpellType == SummonerSpellType::QSS && me.N_7.IsReady(game.gameTime)) {
                    me.N_7.Trigger();
                    Sleep(1000);
                    return;
                }
            }
        }
    }
}
