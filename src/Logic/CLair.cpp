#include "CLair.h"
#include "CEnemy.h"
#include "CDetector.h"
#include "CPhysicalManager.h"
#include "Factory/CEnemyTemplate.h"
#include "AI/CActorAI.h"
#include "AI/CAISchemeManager.h"
#include "../Utils/CRand.h"
#include "../Utils/Maths.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Rendering/Animations/SAnimationState.h"
#include "../Rendering/Animations/SAnimation.h"
#include "../Rendering/Animations/CAnimSet.h"
#include "../Rendering/CDisplayable.h"
#include "../Rendering/ZIndexVals.h"
#include "../Audio/CAudioManager.h"
#include "../Map/SceneManager/CSceneNode.h"
#include "../Map/CMapManager.h"
#include <cstdio>
#include <SFML/System/Vector2.hpp>

CLair::CLair(const std::wstring& uniqueId): CActor(uniqueId), mCurrentGroup(0){
    mTimeSinceSpawn = 0.0f;
	mSpawnLimit = 15;
	mSpawnRate = 0.2f;
	mSpawnCount = 0;
	mSpawnPosition = sf::Vector2f(0.0f,0.0f);
	mMaxEnemyRadius = 0.0f;
	SetZIndex(Z_LAIR);
    mGroupingHint = 0;
    mLocked = false;

	// brakujace inicjalizacje?
    mMonsterCount = -1;
    mCurrentGroup = 0;
	
	mCollisionDetector = gPhysicalManager.CreateDetector();
	mCollisionDetector->SetCategory( PHYSICAL_DETECTOR );
	mCollisionDetector->SetCollidingPhysicals( PHYSICAL_PLAYER | PHYSICAL_MONSTER );
	mCollisionDetector->SetBehaviour( SET_COLLISION_FLAG );
    mCollisionDetector->GetCallbackDispatcher()->RegisterEventDeath(DetectorDestroyed,this,NULL);
}

CLair::~CLair(){
	std::vector<CEnemy*>::iterator it;
	for (it = mSpawned.begin(); it != mSpawned.end(); it++) {
        (*it)->GetCallbackDispatcher()->UnregisterCallee(this);
		(*it)->SetLair( NULL );
	}
    if (mCollisionDetector != NULL){
        mCollisionDetector->MarkForDelete();
        mCollisionDetector->GetCallbackDispatcher()->UnregisterCallee(this);
    }
}

void CLair::AddEnemyType( std::string enemy )
{
	CEnemyTemplate* enemyTemplate = (CEnemyTemplate*)gResourceManager.GetPhysicalTemplate( enemy );
	if ( enemyTemplate )
	{
		mToSpawn.push_back( enemyTemplate );
		if ( enemyTemplate->GetMaxRadius() > mMaxEnemyRadius )
			mMaxEnemyRadius = enemyTemplate->GetMaxRadius();
	}
}

void CLair::Update(float dt){
	mOldPosition = mPosition;
    
    if (mCurrentGroup == 0){
        if ((mSpawnCount==mSpawnLimit) && (mTimeSinceSpawn > mGroupingHint / mSpawnRate - 0.5)){
            mLocked = true;
        } else
            mLocked = false;
        if (!mLocked)
            mTimeSinceSpawn+=dt;
    }
    if ((mCollisionDetector!=NULL)&&(mSpawnState == ssAlive)){
        mCollisionDetector->SetPosition( mPosition + Maths::Rotate(mSpawnPosition,GetRotation()) );
        mCollisionDetector->SetBoundingCircle( mMaxEnemyRadius );

        if (!mCollisionDetector->GetCollisionFlag()){
            if ((mTimeSinceSpawn > mGroupingHint / mSpawnRate) && (mCurrentGroup == 0)){
                mCurrentGroup = mGroupingHint;
                mLocked = false;
            }
            if ((mCurrentGroup > 0) && (mMonsterCount != 0) && (mSpawnCount<mSpawnLimit) && (gScene.GetPhysicalCount(PHYSICAL_MONSTER) < gMapManager.GetCurrent()->GetMaxLivingMonsters())){
                mSpawnCount++;

                CEnemy *enemy = Spawn();
                if (enemy) {
                    mSpawned.push_back(enemy);
                    enemy->GetCallbackDispatcher()->RegisterEventDeath(SpawneeDeath,this,NULL);
                }
                if (mCurrentGroup == 0)
                    Ready();
            }
        }
    }

    CActor::Update(dt);
}

void CLair::SpawneeDeath(CPhysical *sender, void *callee, void *data){
	((CLair*)callee)->RemoveSpawnee((CEnemy*)sender);
}

void CLair::DetectorDestroyed(CPhysical *sender, void *callee, void *data){
    ((CLair*)callee)->mCollisionDetector = NULL;
}

CEnemy *CLair::Spawn(){
	if ( mToSpawn.size() )
	{
        if (mMonsterCount > 0)
            mMonsterCount--;
        if (mCurrentGroup > 0)
            mCurrentGroup--;
		int enemyNo = gRand.Rnd( 0, static_cast<unsigned int>(mToSpawn.size()) );
		CEnemy *enemy = mToSpawn[ enemyNo ]->Create();
        enemy->SetPosition( mPosition + Maths::Rotate(mSpawnPosition,GetRotation()) ,true);
		enemy->SetLair( this );

        if (mAnimSet->GetAnim(CAnimSet::acAttack) != NULL){
            this->SetAnimation(mAnimSet->GetAttackAnim());
            mAnimationTimeLeft = this->GetDisplayable()->GetAnimationState()->animation->TotalLength();
            //mAbilityStateDelayLeft = this->GetDisplayable()->GetAnimationState()->animation->TotalLength();
            //this->GetDisplayable()->GetAnimationState()->Stop();
        }

        // rAum, 29.08 - komentuje bo denerwuje...
        gAudioManager.GetSoundPlayer().Play(gResourceManager.GetSound(mSpawnSound), mPosition);
        
        // potwory tez moga miec cos do powiedzenia:
        if (unsigned sndCount = enemy->GetSounds("spawn").size())
            // przyjmijmy, ze pozycja spawnowanego potwora = pozycja gniazda
            gAudioManager.GetSoundPlayer().Play(enemy->GetSounds("spawn")[rand() % sndCount], mPosition);

		return enemy;
	}

	return NULL;
}

void CLair::RemoveSpawnee(CEnemy *spawnee){
	for (std::vector<CEnemy*>::iterator it = mSpawned.begin(); it != mSpawned.end(); it++){
		if (*it == spawnee){
			mSpawned.erase(it);
			break;
		}
	}
	if(mSpawnCount==mSpawnLimit)
		mTimeSinceSpawn=0.0f;
	mSpawnCount--;
    mLocked = false;
}

void CLair::Ready(){
    mTimeSinceSpawn = (0.5f/mSpawnRate) - gRand.Rndf(0.0f,1.0f/mSpawnRate);
}

