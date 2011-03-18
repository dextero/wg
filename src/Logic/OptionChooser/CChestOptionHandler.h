#ifndef __CCHEST_OPTION_HANDLER_H__
#define __CCHEST_OPTION_HANDLER_H__

#include "IOptionChooserHandler.h"
#include "../../GUI/Messages/CMessageSystem.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Audio/CAudioManager.h"

class CChestOptionHandler : public IOptionChooserHandler
{
    private:
        bool mIsOpened; // rownie dobrze mozna by stan trzymac w obiekcie Obstacle... a nie handlerze
        bool mHasLoot; 
        CObstacle * mChest;

    public:
        CChestOptionHandler(CObstacle * chest) : IOptionChooserHandler() {
            mIsOpened = false;
            mHasLoot = true;
            mChest = chest;
        }

    	virtual void OptionSelected(size_t option) {
            switch(option) {
                case 0:
                    if (mIsOpened) {
                        gMessageSystem.AddMessage(L"Already opened!");
                    } else {
                        if (mHasLoot) {
                            gMessageSystem.AddMessage(L"Found some gold!");
                            mHasLoot = false;
                        } else {
                            gMessageSystem.AddMessage(L"You open the chest...");
                        }
                        mIsOpened = true;
                    }
                    break;
                case 1:
                    if (!mIsOpened) {
                        gMessageSystem.AddMessage(L"Already closed!");
                    } else {
                        gMessageSystem.AddMessage(L"You close the chest...");
                        mIsOpened = false;

                    }
                    break;
                case 2:
                    gGraphicalEffects.ShowEffect("bullet-explosion", mChest->GetPosition());
                    gAudioManager.PlaySound("data/sounds/Lugaro/break.ogg", mChest->GetPosition());
                    mChest->Kill();
                    break;
                default:
                    break;
            }
        };
};


#endif//__CCHEST_OPTION_HANDLER_H__
