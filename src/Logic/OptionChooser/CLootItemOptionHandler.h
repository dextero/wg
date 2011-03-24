#ifndef __CLOOT_ITEM_OPTION_HANDLER_H__
#define __CLOOT_ITEM_OPTION_HANDLER_H__

#include "IOptionChooserHandler.h"
#include "../../GUI/Messages/CMessageSystem.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Audio/CAudioManager.h"
#include "../../Utils/Maths.h"

class CLootItemOptionHandler : public IOptionChooserHandler
{
    private:
        bool mIsOpened; // rownie dobrze mozna by stan trzymac w obiekcie Obstacle... a nie handlerze
        bool mHasLoot; 
        CLoot * mLoot;

    public:
        CLootItemOptionHandler(CLoot * loot): IOptionChooserHandler() {
            mIsOpened = false;
            mHasLoot = true;
            mLoot = loot;
        }

        virtual void Update(float secondsPassed) {
            if (Maths::Length(mPlayer->GetPosition() - mLoot->GetPosition()) > 1.5f) {
                mChooser->Hide();
            }
        }

    	virtual void OptionSelected(size_t option) {
            switch(option) {
                case 0:
                    if (mIsOpened) {
                        gMessageSystem.AddMessage(L"Already opened!");
                    } else {
                        if (mHasLoot) {
                            gMessageSystem.AddMessage(L"Found some gold!");
                            mPlayer->NextLevel();
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
                    gGraphicalEffects.ShowEffect("bullet-explosion", mLoot->GetPosition());
                    gAudioManager.PlaySound("data/sounds/Lugaro/break.ogg", mLoot->GetPosition());
                    mLoot->Kill();
                    break;
                default:
                    break;
            }
        };
};
#endif//__CLOOT_ITEM_OPTION_HANDLER_H__
