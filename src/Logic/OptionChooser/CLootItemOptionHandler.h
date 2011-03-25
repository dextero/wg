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
            if (option > 3) return;

            size_t invPos = option;

            mPlayer->AddItem(mLoot->GetItem(), invPos);
            gGraphicalEffects.ShowEffect("magic-circle-4", mPlayer);
            gAudioManager.PlaySound("data/sounds/reload.wav", mPlayer->GetPosition());
            mLoot->Kill();
        };
};
#endif//__CLOOT_ITEM_OPTION_HANDLER_H__
