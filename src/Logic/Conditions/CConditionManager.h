#ifndef __CCONDITION_MANAGER_H__
#define __CCONDITION_MANAGER_H__

#include "../../Utils/CSingleton.h"
#include "../../IFrameListener.h"

#include <vector>

class CCondition;

#define gConditionManager CConditionManager::GetSingleton()

class CConditionManager : public CSingleton< CConditionManager >, public IFrameListener
{
public:
    CConditionManager();
    virtual ~CConditionManager();
 
    virtual void FrameStarted( float secondsPassed );

    int RegisterCondition(CCondition *cond);
    void UnregisterCondition(int i);

    void Clear();
private:
    std::vector<CCondition*> mConditions;
};

#endif//__CCONDITION_MANAGER_H__
