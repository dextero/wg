#ifndef __CCHEST_OPTION_HANDLER_H__
#define __CCHEST_OPTION_HANDLER_H__

#include "IOptionChooserHandler.h"
#include "../../GUI/Messages/CMessageSystem.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Audio/CAudioManager.h"
#include "../../Utils/Maths.h"
#include "../../Logic/Loots/CLoot.h"
#include "../../Map/CRandomMapGenerator.h"

class CChestOptionHandler : public IOptionChooserHandler
{
    private:
        bool mIsOpened; // rownie dobrze mozna by stan trzymac w obiekcie Obstacle... a nie handlerze
        bool mHasLoot; 
        CObstacle * mChest;

    public:
        CChestOptionHandler(CObstacle * chest): IOptionChooserHandler() {
            mIsOpened = false;
            mHasLoot = true;
            mChest = chest;
        }

        virtual void Update(float secondsPassed) {
            if (Maths::Length(mPlayer->GetPosition() - mChest->GetPosition()) > 1.5f) {
                mChooser->Hide();
            }
        }

    	virtual void OptionSelected(size_t option) {
            switch(option) {
                case 0:
                    gGraphicalEffects.ShowEffect("bullet-explosion", mChest->GetPosition());
                    gAudioManager.PlaySound("data/sounds/Lugaro/break.ogg", mChest->GetPosition());
                    CLoot * loot = gRandomMapGenerator.GenerateNextLoot(0.10f, mChest->GetPosition());
                    if (loot) {
                        loot->SetPosition(mChest->GetPosition());
                    }
                    mChest->Kill();
                    break;
            }
        };
};


#endif//__CCHEST_OPTION_HANDLER_H__
