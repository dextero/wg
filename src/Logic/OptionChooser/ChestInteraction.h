#ifndef __CHEST_INTERACTION_H__
#define __CHEST_INTERACTION_H__

#include "InteractionHandler.h"
#include "../../GUI/CRoot.h"
#include "../../GUI/CTextArea.h"
#include "../../GUI/CButton.h"
#include "../../GUI/CWindow.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/Maths.h"
#include "../CPlayer.h"
#include "../MapObjects/CObstacle.h"
#include "../Loots/CLoot.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Audio/CAudioManager.h"
#include "../../Map/CRandomMapGenerator.h"

class ChestInteraction : public InteractionHandler
{
    protected:
        CPlayer * mPlayer;
        CObstacle * mChest;
        CInteractionTooltip * mTooltip;
        GUI::CTextArea * mDescription;
        GUI::CTextArea * mPlayerGold;
        bool tookAction;

    public:        
        ChestInteraction(CInteractionTooltip * tooltip, CPlayer * player, CObstacle * chest) :
                mPlayer(player),
                mChest(chest),
                mTooltip(tooltip),
                mDescription(NULL),
                mPlayerGold(NULL),
                tookAction(false)
        {
            tooltip->Clear();

            tooltip->GetCanvas()->SetPosition(25.f, 30.f, 50.f, 40.f);

            GUI::CTextArea * title = tooltip->GetCanvas()->CreateTextArea("title");
            title->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"), 2.f);
            title->SetPosition(40.f, 2.5f, 30.f, 10.f);
            gGUI.UnregisterObject(title);
            title->SetText(L"A chest!");


            mDescription = tooltip->GetCanvas()->CreateTextArea("description");
            mDescription->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"), 1.5f);
            mDescription->SetPosition(10.f, 38.f, 85.f, 80.f);
            gGUI.UnregisterObject(mDescription);
            mDescription->SetVisible(true);
            mDescription->SetText(L"You have found a chest. Care to open it?");

            GUI::CButton * buttonYes = tooltip->GetCanvas()->CreateButton("yes");
            buttonYes->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
            buttonYes->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
            buttonYes->SetPosition(8.0f, 76.0f, 45.0f, 15.0f);
            buttonYes->SetCenter(true);
            buttonYes->GetClickCallback()->bind(this, &ChestInteraction::OptionYes);
            buttonYes->SetText(L"Yes", mPlayer->GetNumber(), 0);

            GUI::CButton * buttonNo = tooltip->GetCanvas()->CreateButton("no");
            buttonNo->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
            buttonNo->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
            buttonNo->SetPosition(52.0f, 76.0f, 45.0f, 15.0f);
            buttonNo->SetCenter(true);
            buttonNo->GetClickCallback()->bind(this, &ChestInteraction::OptionNo);
            buttonNo->SetText(L"No", mPlayer->GetNumber(), 1);

            tooltip->SetHandler(this);
            tooltip->Show();
            tooltip->SetPriority(75);
            mTooltip = tooltip;
            gGUI.RegisterInteractionHandler(this);
        }

        ~ChestInteraction() {
            gGUI.UnregisterInteractionHandler(this);
        }

        void Update(float secondsPassed) {
            if (Maths::Length(mPlayer->GetPosition() - mChest->GetPosition()) > 1.5f) {
                mTooltip->Clear();
            }
        }

        void OptionYes() {
            gGraphicalEffects.ShowEffect("bullet-explosion", mChest->GetPosition());
            gAudioManager.PlaySound("data/sounds/Lugaro/break.ogg", mChest->GetPosition());
            CLoot * loot = gRandomMapGenerator.GenerateNextLoot(0.10f, mChest->GetPosition());
            if (loot) {
                loot->SetPosition(mChest->GetPosition());
            }
            mChest->Kill();
        }

        void OptionNo() {
            mTooltip->Hide();
            mTooltip->SetPriority(76);
        }

        virtual void OptionSelected(size_t selected) {
            if (selected == 0) {
                OptionYes();
            } else if (selected == 1) {
                OptionNo();
            }
        };
};

#endif//__CHEST_INTERACTION_H__
