#include "CConditionManager.h"
#include "CCondition.h"
#include "../../CGame.h"

template<> CConditionManager* CSingleton<CConditionManager>::msSingleton = 0;

CConditionManager::CConditionManager()
{
    fprintf( stderr, "CConditionManager::CConditionManager()\n" );

    gGame.AddFrameListener( this );
}

CConditionManager::~CConditionManager()
{
    fprintf( stderr, "CConditionManager::~CConditionManager()\n" );

    for (unsigned int i = 0; i < mConditions.size(); i++)
        mConditions[i]->SwitchIndex(-1);
}

void CConditionManager::FrameStarted( float secondsPassed )
{
    for (unsigned int i = 0; i < mConditions.size(); i++)
        mConditions[i]->TryCheck(secondsPassed);
}

int CConditionManager::RegisterCondition(CCondition *cond){
    mConditions.push_back(cond);
    return (int)(mConditions.size()-1);
}

void CConditionManager::UnregisterCondition(int i){
    if ((i < 0) || (i >= (int)(mConditions.size())))
        return;
    mConditions[i]->SwitchIndex(-1);
    if (i < (int)(mConditions.size()-1)){
        mConditions[i] = mConditions[mConditions.size()-1];
        mConditions[i]->SwitchIndex(i);
    }
    mConditions.pop_back();
}

void CConditionManager::Clear()
{
    for (std::vector<CCondition*>::iterator it = mConditions.begin(); it != mConditions.end(); ++it)
        (*it)->ClearTriggered();
}

