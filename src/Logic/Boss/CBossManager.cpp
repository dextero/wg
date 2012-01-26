#include "CBossManager.h"
#include "../../CGame.h"
#include "../CEnemy.h"
#include "../CPlayer.h"
#include "../CPhysicalManager.h"
#include "../CPlayerManager.h"
#include "../../Utils/Maths.h"
#include "../../Commands/CCommands.h"
#include "../../Utils/StringUtils.h"

#include <sstream>

template<> CBossManager* CSingleton<CBossManager>::msSingleton = 0;


CBossManager::CBossManager():
    mBoss(NULL)
{
    gGame.AddFrameListener(this);
}

void CBossManager::StartBossFight( CEnemy * enemy )
{
    mBoss = enemy;
}

void CBossManager::StopBossFight()
{
    mBoss = NULL;
}

void CBossManager::EnemyDied( CEnemy * enemy )
{
    if (enemy == mBoss)
    {
        StopBossFight();

        gCommands.ParseCommand(L"open-doors");
        gCommands.ParseCommand(L"load-playlist data/music/testpl.xml");


        std::wstring id = enemy->GetUniqueId();
        for (std::vector<SBossOnMap>::iterator it = mBossesOnMap.begin(); it != mBossesOnMap.end(); ++it)
            if (it->id == id)
            {
                if( it->isFinal) {
                    gCommands.ParseCommand(L"exec data/console/final-boss-dead.console");
                }
                mBossesOnMap.erase(it);
                break;
            }
    }
}

CEnemy * CBossManager::GetBoss() const
{
    return mBoss;
}

void CBossManager::Trace(const std::wstring& enemyId, const std::string& aiScheme, float triggerRadius, const std::string playlist, bool final)
{
    for (std::vector<SBossOnMap>::iterator it = mBossesOnMap.begin(); it != mBossesOnMap.end(); ++it) {
        if (it->id == enemyId) {
            return;
        }
    }
    mBossesOnMap.push_back(SBossOnMap(enemyId, aiScheme, playlist, triggerRadius, final));
}

const std::string CBossManager::SerializeLivingBosses()
{
    std::stringstream ss;

    for (std::vector<SBossOnMap>::iterator it = mBossesOnMap.begin(); it != mBossesOnMap.end(); ++it)
        ss << "set-boss " << StringUtils::ConvertToString(it->id) << " " << it->ai << " " << it->radius << " " << it->playlist << " " << it->isFinal << "\n";

    return ss.str();
}


void CBossManager::FrameStarted(float secondsPassed)
{
    // czy ktorys z graczy zblizyl sie do ktoregos z bossow na tyle, zeby odpalic boss-fight?

    // jesli juz z jakism walczymy, olac
    if (!mBoss)
        for (std::vector<SBossOnMap>::iterator it = mBossesOnMap.begin(); it != mBossesOnMap.end(); ++it)
        {
            CEnemy* e = dynamic_cast<CEnemy*>(gPhysicalManager.GetPhysicalById(it->id));
            if (e)
                for (size_t i = 0; i < gPlayerManager.GetPlayerCount(); ++i)
                {
                    if (Maths::Length(e->GetPosition() - gPlayerManager.GetPlayerByNumber(i)->GetPosition()) < it->radius)
                    {
                        // playlista
                        if (it->playlist != "")
                            gCommands.ParseCommand(StringUtils::ConvertToWString("load-playlist " + it->playlist));
                        // ozywienie bossa
                        if (it->ai != "")
                            gCommands.ParseCommand(L"set-ai-scheme " + it->id + L" " + StringUtils::ConvertToWString(it->ai));

                        StartBossFight(e);
                        return;
                    }
                }
            else
                fprintf(stderr, "error @ CBossManager::FrameStarted: Bad boss ID: %ls!\n", it->id.c_str());
        }
}
