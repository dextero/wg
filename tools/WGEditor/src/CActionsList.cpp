#include "CActionsList.h"

template<> CActionsList* CSingleton<CActionsList>::msSingleton = NULL;


SAction SAction::Reverse(SAction action)
{
    switch (action.type)
    {
    case Actions::deleteDoodah:
        action.type = Actions::placeDoodah;
        break;
    case Actions::placeDoodah:
        action.type = Actions::deleteDoodah;
        break;
    case Actions::deletePhysical:
        action.type = Actions::placePhysical;
        break;
    case Actions::placePhysical:
        action.type = Actions::deletePhysical;
        break;
    case Actions::deleteRegion:
        action.type = Actions::placeRegion;
        break;
    case Actions::placeRegion:
        action.type = Actions::deleteRegion;
        break;
    case Actions::changeTile:
        {
            std::string tmp = action.code;
            action.code = action.codeNew;
            action.codeNew = tmp;
        }
        break;
    }
    
    return action;
}

// -----------------------------------

void CActionsList::Push(SAction action)
{
    mUndone.clear();
    mDone.push_back(action);
    if (mDone.size() > UNDO_MAX)
        mDone.pop_front();
}

SAction CActionsList::Undo()
{
    if (!mDone.size())
        return SAction::None();

    SAction top = mDone.back();
    mDone.pop_back();
    mUndone.push_back(SAction::Reverse(top));
    return top;
}

SAction CActionsList::Redo()
{
    if (!mUndone.size())
        return SAction::None();

    SAction top = mUndone.back();
    mUndone.pop_back();
    mDone.push_back(SAction::Reverse(top));
    return top;
}