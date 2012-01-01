#ifndef __SIGN_INTERACTION_H__
#define __SIGN_INTERACTION_H__

#include "InteractionHandler.h"
#include "../../GUI/CRoot.h"
#include "../../GUI/CTextArea.h"
#include "../../GUI/CButton.h"
#include "../../GUI/CImageBox.h"
#include "../../GUI/CWindow.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/Maths.h"
#include "../CPlayer.h"
#include "../CNpc.h"
#include "../Items/CItem.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Audio/CAudioManager.h"
#include "../Abilities/CAbility.h"
#include "../../ResourceManager/CResourceManager.h"


class ShopInteraction : public InteractionHandler
{
    protected:
        CPlayer * mPlayer;
        CNpc * mNpc;
        CInteractionTooltip * mTooltip;
        GUI::CTextArea * mDescription;

    public:        
        ShopInteraction(CInteractionTooltip * tooltip, const std::string & title, CPlayer * player, CNpc * npc) :
                mPlayer(player),
                mNpc(npc),
                mTooltip(tooltip),
                mDescription(NULL)
        {
            tooltip->Clear();
            mDescription = tooltip->GetCanvas()->CreateTextArea("description");
            mDescription->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            mDescription->SetPosition(5.f, 5.f, 90.f, 90.f);
            gGUI.UnregisterObject(mDescription);
            mDescription->SetVisible(true);

//          description->SetText(StringUtils::ConvertToWString(title));
            if (mNpc->GetSellingItem().empty()) {
                mDescription->SetText(L"Hello, I've already sold you what I had.");
            } else {
                GUI::CWindow* canvas = tooltip->GetCanvas();

                CAbility * ability = gResourceManager.GetAbility(mNpc->GetSellingItem());
                int cost = mNpc->GetSellingPrice();

                mDescription->SetText(L"Hello wanderer, I'm Griswold, Griswold the Angry. \
Why angry you ask? Cause no where in Anthkaldia you will be able to buy \
better spells than I can sell. Would you like to buy " + ability->name + L" spell for " + StringUtils::ToWString(cost) + L"gp?");

                GUI::CImageBox* itemIcon = canvas->CreateImageBox("icon");
                itemIcon->SetPosition(5.f, 20.f, 0.f, 0.f);
                itemIcon->SetPosition(0.f, 0.f, 50.f, 50.f, GUI::UNIT_PIXEL);
                itemIcon->AddImageToSequence(ability->icon);
                itemIcon->SetSequenceState(0);

                GUI::CTextArea* itemName = canvas->CreateTextArea("item-name");
                itemName->SetPosition(15.f, 20.f, 80.f, 5.f);
                itemName->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
                itemName->SetText(ability->name);

                GUI::CTextArea* itemDescription = canvas->CreateTextArea("item-desc");
                itemDescription->SetPosition(15.f, 20.f, 80.f, 20.f);
                itemDescription->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
                itemDescription->SetText(ability->description);
                
                GUI::CButton * buttonYes = canvas->CreateButton("yes");
                buttonYes->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
                buttonYes->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
                buttonYes->SetText(L"Yeah, sure (press {btn0})");
                buttonYes->SetPosition(21.0f, 80.0f, 30.0f, 6.0f);
                buttonYes->SetCenter(true);
                buttonYes->GetClickCallback()->bind(this, &ShopInteraction::OptionYes);

                GUI::CButton * buttonNo = canvas->CreateButton("no");
                buttonNo->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
                buttonNo->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
                buttonNo->SetText(L"No thanks. (press {btn1})");
                buttonNo->SetPosition(51.0f, 80.0f, 30.0f, 6.0f);
                buttonNo->SetCenter(true);
                buttonNo->GetClickCallback()->bind(this, &ShopInteraction::OptionNo);
            }

            tooltip->SetHandler(this);
            tooltip->Show();
            mTooltip = tooltip;
            gGUI.RegisterInteractionHandler(this);
        }

        ~ShopInteraction() {
            gGUI.UnregisterInteractionHandler(this);
        }

        void Update(float secondsPassed) {
            if (Maths::Length(mPlayer->GetPosition() - mNpc->GetPosition()) > 1.5f) {
                mTooltip->Clear();
            }
        }

        void OptionYes() {
            if (mNpc->GetSellingItem().empty())
            {
                mTooltip->Clear();
                return;
            }
            int price = mNpc->GetSellingPrice();
            if (mPlayer->GetGold() <= price) {
                mDescription->SetText(mDescription->GetText() + L"\n\nNot enough gold! Come back when you have more. You have only " + StringUtils::ToWString(mPlayer->GetGold()) + L"gp.");
            } else {
                mPlayer->SetGold(mPlayer->GetGold() - price);
                CItem * item = new CItem();
                item->SetAbility(mNpc->GetSellingItem());
                item->mLevel = 1;
                mPlayer->AddItem(item, 1);
                gGraphicalEffects.ShowEffect("magic-circle-4", mPlayer);
                gAudioManager.PlaySound("data/sounds/reload.wav", mPlayer->GetPosition());
                mNpc->SetSellingItem("");
                mTooltip->Clear();
            }
        }
        void OptionNo() {
            mTooltip->Clear();
        }

        virtual void OptionSelected(size_t selected) {
            if (selected == 0) {
                OptionYes();
            } else if (selected == 1) {
                OptionNo();
            }
        };
};

#endif//__SIGN_INTERACTION_H__
