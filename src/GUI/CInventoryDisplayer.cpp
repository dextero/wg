#include "CInventoryDisplayer.h"
#include "CWindow.h"
#include "CRoot.h"
#include "CItemSlotsBar.h"
#include "CImageBox.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Utils/StringUtils.h"
#include "../Utils/KeyStrings.h"
#include "../Utils/Maths.h"
#include "../Logic/Abilities/CAbility.h"
#include "../Logic/CPlayer.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/Items/CItem.h"
#include "../Logic/CLogic.h"

#undef CreateWindow

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <cassert>
#include <cstdio>

using namespace GUI;

void CInventoryDisplayer::UpdatePlayerData()
{
    if (mBar)
        mBar->UpdatePlayerData();
}

CInventoryDisplayer::CInventoryDisplayer( const std::string& name, CGUIObject* parent, 
                                             unsigned int zindex, int player )
    : CGUIObject(name, GUI_ABILITYTREEDISPLAYER, parent, zindex),
    mPlayer(player),
    mForceReload(true)
{
    SetPosition(0.f, 0.f, 100.f, 100.f);
    
    mBackground = CreateWindow("inventory-displayer" + StringUtils::ToString(player));
    mBackground->SetBackgroundImage("data/GUI/skills-bkg.jpg");
    mBackground->SetPosition(0.f, 0.f, 100.f, 100.f, UNIT_PERCENT);
    
    mBar = new CItemSlotsBar();
    // dobrze, jesli te ustawienia sa przed Init, bo wtedy nie trzeba recznie wywolywac UpdatePosition()
    mBar->SetBarPosPix(sf::Vector2f(100.f, 300.f));
    mBar->SetBarSizePix(sf::Vector2f(150.f, 150.f));
            
    sf::Vector2f slotPos[CItemSlotsBar::ITEM_SLOTS_COUNT] = {
        sf::Vector2f(33.33f, 0.f),      // (0) => gora
        sf::Vector2f(66.67f, 33.33f),   // (1) => prawo 
        sf::Vector2f(33.33f, 66.67f),   // (2) => dol
        sf::Vector2f(0.f, 33.33f)       // (3) => lewo
    };
    for (size_t i = 0; i < CItemSlotsBar::ITEM_SLOTS_COUNT; ++i)
        mBar->SetSlotPosPercent(i, slotPos[i]);
    mBar->SetSlotSizePercent(sf::Vector2f(33.33f, 33.33f));
    
    mBar->Init(this, player);
}

CInventoryDisplayer::~CInventoryDisplayer(){

}

void CInventoryDisplayer::UpdateSprites( float secondsPassed ){
    if (mForceReload) {
        mForceReload = false;
        CPlayer* player = gPlayerManager.GetPlayerByNumber(mPlayer);
        if (player) {
            for (size_t i = 0; i < CItemSlotsBar::ITEM_SLOTS_COUNT; ++i)
            {
                mBar->SetSlotItem(i, player->GetItem(i), false);
                printf("Slot %d: item %p\n", i, player->GetItem(i));
            }
        }
    }
}

