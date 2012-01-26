#ifndef __SIGN_INTERACTION_H__
#define __SIGN_INTERACTION_H__

#include "InteractionHandler.h"
#include "../../GUI/CRoot.h"
#include "../../GUI/CTextArea.h"
#include "../../GUI/CItemSlot.h"
#include "../../GUI/CButton.h"
#include "../../GUI/CImageBox.h"
#include "../../GUI/CWindow.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/Maths.h"
#include "../CPlayer.h"
#include "../CNpc.h"
#include "../Loots/CLoot.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Audio/CAudioManager.h"
#include "../Abilities/CAbility.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../Factory/CPhysicalTemplate.h"
#include "../../Input/CBindManager.h"
#include "../../Utils/KeyStrings.h"


class ShopInteraction : public InteractionHandler
{
    protected:
        CPlayer * mPlayer;
        CNpc * mNpc;
        CInteractionTooltip * mTooltip;
        GUI::CTextArea * mDescription;
        GUI::CTextArea * mPlayerGold;
        bool tookAction;

    public:        
        ShopInteraction(CInteractionTooltip * tooltip, CPlayer * player, CNpc * npc) :
                mPlayer(player),
                mNpc(npc),
                mTooltip(tooltip),
                mDescription(NULL),
                mPlayerGold(NULL),
                tookAction(false)
        {
            fprintf(stderr, "SI creating %p\n", this);
            tooltip->Clear();

            GUI::CTextArea * title = tooltip->GetCanvas()->CreateTextArea("title");
            title->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"), 2.f);
            title->SetPosition(26.f, 2.5f, 30.f, 10.f);
            gGUI.UnregisterObject(title);
            title->SetText(L"Buy spell");


            mDescription = tooltip->GetCanvas()->CreateTextArea("description");
            mDescription->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"), 1.5f);
            mDescription->SetPosition(10.f, 15.f, 85.f, 80.f);
            gGUI.UnregisterObject(mDescription);
            mDescription->SetVisible(true);

            mPlayerGold = tooltip->GetCanvas()->CreateTextArea("playerGold");
            mPlayerGold->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"), 2.f);
            mPlayerGold->SetPosition(65.f, 2.5f, 30.f, 10.f);
            gGUI.UnregisterObject(mPlayerGold);
            mPlayerGold->SetVisible(true);
            mPlayerGold->SetText(L"Your gold: " + GUI::CTextArea::GetNextColorString(sf::Color(231, 216, 46)) //Gold
                   + StringUtils::ToWString(mPlayer->GetGold()));

//          description->SetText(StringUtils::ConvertToWString(title));
            if (mNpc->GetSellingItem().empty()) {
                mDescription->SetText(L"Hello, I've already sold you what I had.");
            } else {
                GUI::CWindow* canvas = tooltip->GetCanvas();

                CAbility * ability = gResourceManager.GetAbility(mNpc->GetSellingItem());
                int cost = mNpc->GetSellingPrice();

                mDescription->SetText(L"Hello wanderer, Would you like to buy " + ability->name + L" spell for " + StringUtils::ToWString(cost) + L"gp?");

                GUI::CImageBox* weaponIcon = tooltip->GetCanvas()->CreateImageBox("weaponIcon");
                weaponIcon->SetPosition(3.f, 45.f, 9.f, 9.f, GUI::UNIT_PERCENT, true);
                weaponIcon->AddImageToSequence(ability->icon);
                weaponIcon->SetSequenceState(0);

                GUI::CTextArea* weaponDescription = tooltip->GetCanvas()->CreateTextArea("weaponDescription");
                weaponDescription->SetPosition(10.f, 45.f, 38.f, 20.f);
                weaponDescription->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
                weaponDescription->SetText(GUI::CItemSlot::CreateWeaponDescription(ability, NULL, mPlayer,
                        GUI::CItemSlot::MID_VERBOSITY));

                GUI::CButton * buttonYes = canvas->CreateButton("yes");
                buttonYes->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
                buttonYes->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
                buttonYes->SetPosition(21.0f, 87.0f, 30.0f, 6.0f);
                buttonYes->SetCenter(true);
                buttonYes->GetClickCallback()->bind(this, &ShopInteraction::OptionYes);

                std::string keyKey = "Abi-0";
                std::string keyXKey = "AbiX-0";
                std::string keyLabel;
                const std::map<std::string, int> & keys = gBindManagerByPlayer(mPlayer->GetNumber())->GetKeyBindings();
                if (keys.count(keyKey) > 0) {
                    keyLabel = KeyStrings::KeyToString(keys.find(keyKey)->second);
                } else if (keys.count(keyXKey) > 0) {
                    keyLabel = KeyStrings::KeyToString(keys.find(keyXKey)->second);
                }
                buttonYes->SetText(L"Yes (press " + StringUtils::ConvertToWString(keyLabel) + L")");

                GUI::CButton * buttonNo = canvas->CreateButton("no");
                buttonNo->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
                buttonNo->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
                buttonNo->SetPosition(51.0f, 87.0f, 30.0f, 6.0f);
                buttonNo->SetCenter(true);
                buttonNo->GetClickCallback()->bind(this, &ShopInteraction::OptionNo);
                
                keyKey = "Abi-1";
                keyXKey = "AbiX-1";
                if (keys.count(keyKey) > 0) {
                    keyLabel = KeyStrings::KeyToString(keys.find(keyKey)->second);
                } else if (keys.count(keyXKey) > 0) {
                    keyLabel = KeyStrings::KeyToString(keys.find(keyXKey)->second);
                }
                buttonNo->SetText(L"No (press " + StringUtils::ConvertToWString(keyLabel) + L")");
            }

            tooltip->SetHandler(this);
            tooltip->Show();
            mTooltip = tooltip;
            gGUI.RegisterInteractionHandler(this);
        }

        ~ShopInteraction() {
            fprintf(stderr, "SI destroying %p\n", this);
            gGUI.UnregisterInteractionHandler(this);
        }

        void Update(float secondsPassed) {
            if (Maths::Length(mPlayer->GetPosition() - mNpc->GetPosition()) > 1.5f) {
                mTooltip->Clear();
            } else {
                mPlayerGold->SetText(L"Your gold: " + GUI::CTextArea::GetNextColorString(sf::Color(231, 216, 46)) //Gold
                       + StringUtils::ToWString(mPlayer->GetGold()));
            }
        }

        void OptionYes() {
            if (mNpc->GetSellingItem().empty())
            {
                mTooltip->Hide();
                return;
            }
            int price = mNpc->GetSellingPrice();
            if (mPlayer->GetGold() <= price) {
                if (!tookAction) {
                    mDescription->SetText(mDescription->GetText() + L"\n\nNot enough gold! Come back when you have more. You have only " + StringUtils::ToWString(mPlayer->GetGold()) + L"gp.");
                    tookAction = true;
                }
            } else {
                mPlayer->SetGold(mPlayer->GetGold() - price);
                mPlayerGold->SetText(L"Your gold: " + GUI::CTextArea::GetNextColorString(sf::Color(231, 216, 46)) //Gold
                       + StringUtils::ToWString(mPlayer->GetGold()));


                CLoot * loot = (CLoot*)(gResourceManager.GetPhysicalTemplate("data/loots/weapon.xml")->Create());
                loot->SetPosition(mPlayer->GetPosition());
                loot->SetAbility(mNpc->GetSellingItem());

                mNpc->SetSellingItem("");
                mTooltip->Hide();
                loot->HandleCollision(mPlayer);
            }
        }
        void OptionNo() {
            mTooltip->Hide();
            mTooltip->SetPriority(51);
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