#ifndef __CLOOT_ITEM_OPTION_HANDLER_H__
#define __CLOOT_ITEM_OPTION_HANDLER_H__

#include "IOptionChooserHandler.h"
#include "../../GUI/Messages/CMessageSystem.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Audio/CAudioManager.h"
#include "../../Utils/Maths.h"
#include "../Items/CItem.h"

inline std::string CutToName(const std::string & ability) {
    std::string ret = ability;
    ret = ret.substr(ret.rfind("/") + 1);
    ret = ret.substr(0, ret.find("."));
    return ret;
}

class CLootItemOptionHandler : public IOptionChooserHandler
{
    private:
        bool mIsOpened; // rownie dobrze mozna by stan trzymac w obiekcie Obstacle... a nie handlerze
        bool mHasLoot; 
        CLoot * mLoot;
        std::string mTitle;


    public:
        CLootItemOptionHandler(CLoot * loot): IOptionChooserHandler() {
            mIsOpened = false;
            mHasLoot = true;
            mLoot = loot;
            mTitle = "Pickup a " + CutToName(loot->GetItem()->GetAbility()) + "?";
        }

        virtual void Update(float secondsPassed) {
            if (Maths::Length(mPlayer->GetPosition() - mLoot->GetPosition()) > 0.72f) {
                mChooser->Hide();
            } else {
                mChooser->SetTitle(mTitle); // pomyslec., czy nie zrobic w IOptionHandlerze jakiegos void onInit()

                std::vector<std::string> options;
                for (size_t i = 0 ; i < 4 ; i++) {
                    if (mPlayer->GetItem(i)) {
                        options.push_back("swap with " + CutToName(mPlayer->GetItem(i)->GetAbility()));
                    } else {
                        std::string option = "take as ";
                        switch (i) {
                            case 0 : option += "A weapon"; break;
                            case 1 : option += "B weapon"; break;
                            case 2 : option += "C weapon"; break;
                            case 3 : option += "D weapon"; break;
                        }
                        options.push_back(option);
                    }
                }
                mChooser->SetOptions(options);
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
