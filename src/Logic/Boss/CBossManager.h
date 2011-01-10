#ifndef __CBOSSMANAGER_H__
#define __CBOSSMANAGER_H__

#include "../../Utils/CSingleton.h"
#include "../../IFrameListener.h"
#include "../../Audio/CPlayList.h"

#include <vector>

class CEnemy;

#define gBossManager CBossManager::GetSingleton()

class CBossManager : public CSingleton<CBossManager>, public IFrameListener
{
public:
    CBossManager();

    void StartBossFight(CEnemy * enemy);
    void StopBossFight();

    void EnemyDied(CEnemy * enemy);
    CEnemy * GetBoss() const;

    // sprawdzaj odleglosc gracza od tege przeciwnika co klatke
    // i w razie czego odpal boss-fighta
    void Trace(const std::wstring& enemyId, const std::string& aiScheme, float triggerRadius, const std::string playlist = "");
    const std::string SerializeLivingBosses();

    // zerowanie bossa, czyszczenie wektora sledzonych bossow
    void ClearData() { mBoss = NULL; mBossesOnMap.clear(); }

    bool FramesDuringPause(EPauseVariant pv) { return pv == pvLogicOnly; }
    void FrameStarted(float secondsPassed);

private:
    CEnemy * mBoss;

    // nie wiem, jak nazwac sensownie ta strukture :x
    struct SBossOnMap {
        std::wstring id;
        std::string ai;
        std::string playlist;
        float radius;
        
        SBossOnMap(): id(L""), ai(""), playlist(""), radius(0.f) {}
        SBossOnMap(const std::wstring& id, const std::string& ai, const std::string& pl, float r): id(id), ai(ai), playlist(pl), radius(r) {}
    };
    // bossy obecne na mapie
    std::vector<SBossOnMap> mBossesOnMap;
};

#endif //  __CBOSSMANAGER_H__
