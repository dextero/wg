#ifndef __WEAPON_INTERACTION_H__
#define __WEAPON_INTERACTION_H__

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
#include "../Loots/CLoot.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Audio/CAudioManager.h"
#include "../Abilities/CAbility.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../Factory/CPhysicalTemplate.h"
#include "../../Input/CBindManager.h"
#include "../../Utils/KeyStrings.h"


class WeaponInteraction : public InteractionHandler
{
    protected:
        CPlayer * mPlayer;
        CLoot * mLoot;
        CInteractionTooltip * mTooltip;
        GUI::CTextArea * mDescription;

    public:        
        WeaponInteraction(CInteractionTooltip * tooltip, CPlayer * player, CLoot * loot) :
                mPlayer(player),
                mLoot(loot),
                mTooltip(tooltip),
                mDescription(NULL)
        {
            fprintf(stderr, "WI creating %p\n", this);
            tooltip->Clear();

            GUI::CTextArea * title = tooltip->GetCanvas()->CreateTextArea("title");
            title->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"), 2.f);
            title->SetPosition(26.f, 2.5f, 30.f, 10.f);
            gGUI.UnregisterObject(title);
            title->SetText(L"Pickup spell");

            CAbility * ability = gResourceManager.GetAbility(loot->GetItem()->GetAbility());

            GUI::CImageBox* weaponIcon = tooltip->GetCanvas()->CreateImageBox("weaponIcon");
            weaponIcon->SetPosition(3.f, 35.f, 9.f, 9.f, GUI::UNIT_PERCENT, true);
            weaponIcon->AddImageToSequence(ability->icon);
            weaponIcon->SetSequenceState(0);

            GUI::CTextArea* weaponDescription = tooltip->GetCanvas()->CreateTextArea("weaponDescription");
            weaponDescription->SetPosition(10.f, 35.f, 38.f, 20.f);
            weaponDescription->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            weaponDescription->SetText(GUI::CItemSlot::CreateWeaponDescription(ability, mLoot->GetItem(), mPlayer,
                    GUI::CItemSlot::MID_VERBOSITY));


            for (int i = 0 ; i < 4 ; i++) {
                CItem * playerItem = mPlayer->GetItem(i);

                float step = 24.10f;

                std::wstring buttonText = (playerItem == NULL) ? L"Take" : L"Replace";
                CAbility * playerAbility = (playerItem == NULL) ? NULL : gResourceManager.GetAbility(playerItem->GetAbility());

                GUI::CImageBox* playerWeaponBackground = tooltip->GetCanvas()->CreateImageBox("playerWeaponBackground" + StringUtils::ToString(i));
                playerWeaponBackground->SetPosition(49.f, 4.f + (i * step), 50.f, 23.f, GUI::UNIT_PERCENT, false);
                playerWeaponBackground->AddImageToSequence("data/GUI/weapon-highlight.png");
                playerWeaponBackground->SetSequenceState(0);

                GUI::CImageBox* playerWeaponIcon = tooltip->GetCanvas()->CreateImageBox("playerWeaponIcon" + StringUtils::ToString(i));
                playerWeaponIcon->SetPosition(50.f, 6.f + (i * step), 9.f, 9.f, GUI::UNIT_PERCENT, true);
                if (playerItem != NULL) {
                    playerWeaponIcon->AddImageToSequence(playerAbility->icon);
                } else {
                    playerWeaponIcon->AddImageToSequence("data/GUI/bestiary/background.png");
                }
                playerWeaponIcon->SetSequenceState(0);

                GUI::CTextArea* playerWeaponDescription = tooltip->GetCanvas()->CreateTextArea("playerWeaponDescription" + StringUtils::ToString(i));
                playerWeaponDescription->SetPosition(57.f, 6.0f + (i * step) , 38.f, 20.f);
                playerWeaponDescription->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"), 0.9f);
                if (playerItem != NULL) {
                    playerWeaponDescription->SetText(GUI::CItemSlot::CreateWeaponDescription(playerAbility, playerItem, mPlayer,
                            GUI::CItemSlot::LOW_VERBOSITY));
                } else {
                    playerWeaponDescription->SetText(L"Empty slot");
                }

                GUI::CButton * buttonReplace = tooltip->GetCanvas()->CreateButton("replace" + StringUtils::ToString(i));
                buttonReplace->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
                buttonReplace->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
                buttonReplace->SetText(buttonText, mPlayer->GetNumber(), i);
                buttonReplace->SetPosition(73.0f, 21.0f + (i * step), 27.0f, 6.0f);
                buttonReplace->SetCenter(true);
                buttonReplace->GetClickIntCallback()->bind(this, &WeaponInteraction::ReplaceButtonClicked);
                buttonReplace->SetClickIntCallbackParam(i);
            }
            tooltip->SetHandler(this);
            tooltip->Show();
            mTooltip = tooltip;
            gGUI.RegisterInteractionHandler(this);
        }

        ~WeaponInteraction() {
            fprintf(stderr, "WI destroying %p\n", this);
            gGUI.UnregisterInteractionHandler(this);
        }

        void Update(float secondsPassed) {
            if (Maths::Length(mPlayer->GetPosition() - mLoot->GetPosition()) > 1.5f) {
                mTooltip->Clear();
            }
        }

        void ReplaceButtonClicked(int selected)
        {
            this->OptionSelected((size_t)selected);
        }

        virtual void OptionSelected(size_t selected) {
            mPlayer->AddItem(mLoot->GetItem(), selected);
            gGraphicalEffects.ShowEffect("magic-circle-4", mPlayer);
            gAudioManager.PlaySound("data/sounds/reload.wav", mPlayer->GetPosition());
            mLoot->Kill();
        };
};

#endif//__WEAPON_INTERACTION_H__
