#include "CItemSlotsBar.h"
#include "CWindow.h"
#include "CItemSlot.h"
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

CItemSlotsBar::CItemSlotsBar():
    mBar(NULL),
    mBarPosPercent(10.f, 50.f),
    mBarSizePix(150.f, 150.f),
    mSlotSizePercent(33.33f, 33.33f)
{
    memset(mSlotBg, 0, ITEM_SLOTS_COUNT * sizeof(CWindow*));
    memset(mSlot, 0, ITEM_SLOTS_COUNT * sizeof(CItemSlot*));
    
    mSlotPosPercent[0] = sf::Vector2f(33.33f, 0.f);     // gora
    mSlotPosPercent[1] = sf::Vector2f(66.67f, 33.33f);  // prawo
    mSlotPosPercent[2] = sf::Vector2f(33.33f, 66.67f);  // dol
    mSlotPosPercent[3] = sf::Vector2f(0.f, 33.33f);     // lewo
}

CItemSlotsBar::~CItemSlotsBar()
{
}


void CItemSlotsBar::Init(CGUIObject* parent, unsigned playerNumber)
{
    std::string prefix = std::string("pl") + StringUtils::ToString(playerNumber) + "-";

    mPlayerNumber = playerNumber;
    
    mBar = parent->CreateWindow(prefix + "abi-bar", true, Z_GUI2);
    mBar->SetBackgroundImage("data/GUI/abilities-holder.png");

    for (unsigned int i = 0; i < ITEM_SLOTS_COUNT; ++i)
    {
        mSlotBg[i] = mBar->CreateWindow(std::string("slot-bg-") + StringUtils::ToString(i));
        // zmieniajac ten obrazek, zrob to samo w CRoot::SendMouseEvent (i wstaw tam podswietlona wersje)
        mSlotBg[i]->SetBackgroundImage("data/GUI/abi-bar/abi-bar-scroll.png");

        mBindKey[i] = mSlotBg[i]->CreateTextArea("key");
        mBindKey[i]->SetPosition(3.f, 67.f, 94.f, 27.f);
        mBindKey[i]->SetFont(gGUI.GetFontSetting("FONT_DEFAULT")); // a bylo 15.f
        mBindKey[i]->SetColor(sf::Color(255, 255, 255, 150));
        mSlot[i] = mSlotBg[i]->CreateItemSlot("slot", playerNumber);
        mSlot[i]->SetPosition(0.f, 0.f, 100.f, 100.f);
    }

    UpdatePosition();
    UpdateKeyStrings();

    mBar->SetVisible(false);
}

void CItemSlotsBar::UpdatePlayerData()
{
    if (CPlayer* player = gPlayerManager.GetPlayerByNumber(mPlayerNumber))
    {
        for (unsigned i = 0; i < ITEM_SLOTS_COUNT; ++i)
        {
            CItem* item = mSlot[i]->GetSelectedItem();
            if (player->GetItem(i) != item)
            {
                if (item)
                {
                    player->RemoveItem(item);
                    player->AddItem(item, i);
                }
                else
                    player->RemoveItem(player->GetItem(i));
            }
        }
    }
}

void CItemSlotsBar::UpdateKeyStrings()
{
    // podpisy na slotach
    const std::map<std::string, int>& map = System::Input::CBindManager::GetActualBindManager(mPlayerNumber)->GetKeyBindings();

    for (unsigned i = 0; i < ITEM_SLOTS_COUNT; ++i)
    {
        if (map.find(std::string("Abi-") + StringUtils::ToString(i)) != map.end())
        {
            int key = map.find(std::string("Abi-") + StringUtils::ToString(i))->second;
            mBindKey[i]->SetText(StringUtils::ConvertToWString(KeyStrings::KeyToString(key)));
        }
    }
}

void CItemSlotsBar::Show()
{
    UpdatePosition();
    UpdateKeyStrings();
    mBar->SetVisible(true);
}

void CItemSlotsBar::Hide()
{
    mBar->SetVisible(false);
}

void CItemSlotsBar::UpdatePosition()
{
    sf::Vector2f screenSize = gGUI.ConvertToGlobalPosition(sf::Vector2f(1.f, 1.f));
    mBarSizePix.x = mBarSizePix.y = std::min(screenSize.x, screenSize.y) * 0.3f;    // hardkodowany rozmiar - 30% mniejszej wspolrzednej ekranu

    mBar->SetPosition(mBarPosPercent.x / 100.f * screenSize.x, mBarPosPercent.y / 100.f * screenSize.y, mBarSizePix.x, mBarSizePix.y, UNIT_PIXEL);
    for (unsigned int i = 0; i < ITEM_SLOTS_COUNT; ++i)
        if (mSlotBg[i])
            mSlotBg[i]->SetPosition(mSlotPosPercent[i].x, mSlotPosPercent[i].y, mSlotSizePercent.x, mSlotSizePercent.y, UNIT_PERCENT);
}


CItem* CItemSlotsBar::GetSelectedItem(unsigned int num)
{
    return (num < ITEM_SLOTS_COUNT && mSlot[num] ? mSlot[num]->GetSelectedItem() : NULL);
}


void CItemSlotsBar::SetBarBackground(const std::string& bg)
{
    if (mBar)
        mBar->SetBackgroundImage(bg);
}

void CItemSlotsBar::SetSlotBackground(const std::string& bg)
{
    for (unsigned int i = 0; i < ITEM_SLOTS_COUNT; ++i)
    {
        if (!mSlotBg[i])
            continue;

        mSlotBg[i]->SetBackgroundImage(bg);
    }
}

void CItemSlotsBar::SetSlotItem(size_t slot, CItem* item, bool updatePlayer)
{
    if (slot < ITEM_SLOTS_COUNT && mSlot[slot])
        mSlot[slot]->SetSelectedItem(item);

    if (updatePlayer)
        UpdatePlayerData();
}
