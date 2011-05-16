#include "CEnemy.h"
#include "AI/CActorAI.h"
#include "../Logic/Boss/CBossManager.h"
#include "../Logic/CBestiary.h"
#include "../ResourceManager/CResourceManager.h"
#include "Loots/CLootManager.h"

CEnemy::CEnemy(const std::wstring &uniqueId) 
:	CActor(uniqueId)	
{
    mController = new CActorAI(this);
}

CEnemy::~CEnemy() 
{
}

void CEnemy::Update(float dt)
{
    mController->Update(dt);
    CActor::Update(dt);
}

void CEnemy::Kill() 
{
    if ( mSpawnState != ssDying )
        CLootManager::DropLootAt( mPosition );

    gBossManager.EnemyDied(this);
    gBestiary.EnableMonster(this);

    CActor::Kill();
}

