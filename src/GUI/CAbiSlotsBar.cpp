#include "CAbiSlotsBar.h"
#include "CWindow.h"
#include "CAbilitySlot.h"
#include "CRoot.h"
#include "CTextArea.h"
#include "Localization/CLocalizator.h"

#include "../Input/CBindManager.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/CPlayer.h"
#include "../CGameOptions.h"
#include "../Utils/KeyStrings.h"

#include <string.h>

#undef CreateWindow

using namespace GUI;

CAbiSlotsBar::CAbiSlotsBar():
    mBar(NULL)
{
    memset(mSlotBg, 0, ABI_SLOTS_COUNT * sizeof(CWindow*));
    memset(mSlot, 0, ABI_SLOTS_COUNT * sizeof(CAbilitySlot*));
}

CAbiSlotsBar::~CAbiSlotsBar()
{
}


void CAbiSlotsBar::Init(unsigned playerNumber)
{
    std::string prefix = std::string("pl") + StringUtils::ToString(playerNumber) + "-";

    mPlayerNumber = playerNumber;
    
    mBar = gGUI.CreateWindow(prefix + "abi-bar", true, Z_GUI2);
    mBar->SetBackgroundImage("data/GUI/abilities-holder.png");

    UpdatePosition();

    float abiSlotDelta = 90.f / ABI_SLOTS_COUNT;

    for (unsigned int i = 0; i < ABI_SLOTS_COUNT; ++i)
    {
        mSlotBg[i] = mBar->CreateWindow(std::string("slot-bg-") + StringUtils::ToString(i));
        mSlotBg[i]->SetPosition(6.f + abiSlotDelta * i, 10.f, abiSlotDelta - 2.f, 80.f);
        // zmieniajac ten obrazek, zrob to samo w CRoot::SendMouseEvent (i wstaw tam podswietlona wersje)
        mSlotBg[i]->SetBackgroundImage("data/GUI/abi-bar/abi-bar-scroll.png");

        mBindKey[i] = mSlotBg[i]->CreateTextArea("key");
        mBindKey[i]->SetPosition(3.f, 67.f, 94.f, 27.f);
        mBindKey[i]->SetFont(gLocalizator.GetFont(FONT_DIALOG), 15.f);
        mBindKey[i]->SetColor(sf::Color(255, 255, 255, 150));
        mSlot[i] = mSlotBg[i]->CreateAbilitySlot("slot", true, Z_GUI2 - 1);
        mSlot[i]->SetPosition(0.f, 0.f, 100.f, 100.f);
    }

    UpdateKeyStrings();
    UpdatePlayerData();

    mBar->SetVisible(false);
}

void CAbiSlotsBar::UpdatePlayerData()
{
    if (CPlayer* player = gPlayerManager.GetPlayerByNumber(mPlayerNumber))
    {
        int* abis = player->GetController()->GetSelectedAbilities();
        std::vector<SAbilityInstance>* instances = player->GetAbilityPerformer().GetAbilities();

        for (unsigned i = 0; i < ABI_SLOTS_COUNT; ++i)
        {
            abis[i] = -1; // -1 == CAbilityKeyMap::ABILITY_NOT_FOUND
            for (size_t it = 0; it < instances->size(); ++it)
                if (instances->at(it).ability == mSlot[i]->GetSelectedAbility())
                {
                    abis[i] = it;
                    break;
                }
        }
    }
}

void CAbiSlotsBar::UpdateKeyStrings()
{
    // podpisy na slotach
    const std::map<std::string, int>& map = System::Input::CBindManager::GetActualBindManager(mPlayerNumber)->GetKeyBindings();

    for (unsigned i = 0; i < ABI_SLOTS_COUNT; ++i)
    {
        if (map.find(std::string("Abi-") + StringUtils::ToString(i)) != map.end())
        {
            int key = map.find(std::string("Abi-") + StringUtils::ToString(i))->second;
            mBindKey[i]->SetText(StringUtils::ConvertToWString(KeyStrings::KeyToString(key)));
        }
    }
}

void CAbiSlotsBar::Show()
{
    UpdatePosition();
    UpdateKeyStrings();
    mBar->SetVisible(true);
}

void CAbiSlotsBar::Hide()
{
    mBar->SetVisible(false);
}

void CAbiSlotsBar::UpdatePosition()
{
    if ( mPlayerNumber == 0 )
        mBar->SetPosition( 180.0f, gGameOptions.GetHeight() - 60.f, 180.0f, 60.f, UNIT_PIXEL, false );
	else
        mBar->SetPosition( gGameOptions.GetWidth() - 360.0f, gGameOptions.GetHeight() - 60.f, 180.0f, 60.0f, UNIT_PIXEL, false );
}


CAbility* CAbiSlotsBar::GetSelectedAbility(unsigned int num)
{
    return (num < ABI_SLOTS_COUNT && mSlot[num] ? mSlot[num]->GetSelectedAbility() : NULL);
}


void CAbiSlotsBar::SetBarBackground(const std::string& bg)
{
    if (mBar)
        mBar->SetBackgroundImage(bg);
}

void CAbiSlotsBar::SetSlotBackground(const std::string& bg)
{
    for (unsigned int i = 0; i < ABI_SLOTS_COUNT; ++i)
    {
        if (!mSlotBg[i])
            continue;

        mSlotBg[i]->SetBackgroundImage(bg);
    }
}

void CAbiSlotsBar::SetSlotAbility(unsigned int slot, CAbility* abi)
{
    if (slot < ABI_SLOTS_COUNT && mSlot[slot])
        mSlot[slot]->SetSelectedAbility(abi);

    UpdatePlayerData();
}
