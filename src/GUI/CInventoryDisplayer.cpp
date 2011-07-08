#include "CInventoryDisplayer.h"
#include "CWindow.h"
#include "CRoot.h"
#include "CItemSlotsBar.h"
#include "CImageBox.h"
#include "CAbilityTreeDisplayer.h"
#include "CTextArea.h"
#include "CButton.h"
#include "CGameScreens.h"
#include "Localization/CLocalizator.h"
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
    : CGUIObject(name, GUI_INVENTORY_DISPLAYER, parent, zindex),
    mPlayer(player),
    mForceReload(true),
    mBackground(NULL),
    mRightPanel(NULL),
    mBar(NULL),
    mAbiTreeDisplayer(NULL)
{
    SetPosition(0.f, 0.f, 100.f, 100.f);
    
    mBackground = CreateWindow("inventory-displayer" + StringUtils::ToString(player));
    mBackground->SetBackgroundImage("data/GUI/skills-bkg.jpg");
    mBackground->SetPosition(0.f, 0.f, 100.f, 100.f, UNIT_PERCENT);
    
    mBar = new CItemSlotsBar();
    // dobrze, jesli te ustawienia sa przed Init, bo wtedy nie trzeba recznie wywolywac UpdatePosition()
    mBar->SetBarPosPercent(sf::Vector2f(10.f, 50.f));
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
    
    mRightPanel = mBackground->CreateWindow("right-panel");
    mRightPanel->SetPosition(50.f, 8.f, 50.f, 92.f);
    
    mAbiTreeDisplayer = mRightPanel->CreateAbilityTreeDisplayer("tree", player, "adom");
    mAbiTreeDisplayer->SetPosition(5.f,8.f,90.0f,90.0f);
    mAbiTreeDisplayer->SetHolderImage("data/GUI/abilities_scroll_empty.png",0.0f);
    mAbiTreeDisplayer->SetActiveHolderImage("data/GUI/abilities_scroll_bought.png");
    mAbiTreeDisplayer->SetPotentialHolderImage("data/GUI/abilities_scroll_potential.png");
    mAbiTreeDisplayer->SetDependencyLineImage("data/GUI/abilities-dependency-line.png");
    mAbiTreeDisplayer->SetLvlTextFont(gLocalizator.GetFont(GUI::FONT_MESSAGE),20);
    mAbiTreeDisplayer->ForceReload();
    
    CTextArea * xp = mAbiTreeDisplayer->CreateTextArea("xptext");
    xp->SetPosition(5.0f,87.0f,25.0f,15.0f);
    xp->SetFont(gLocalizator.GetFont(GUI::FONT_DIALOG), 14 );
    xp->SetText(L"XP: ?");

    CTextArea * sp = mAbiTreeDisplayer->CreateTextArea("skill-points");
    sp->SetPosition(5.0f,90.0f,25.0f,10.0f);
    sp->SetFont(gLocalizator.GetFont(GUI::FONT_DIALOG), 14 );
    sp->SetText(L"SP: ?");

    CTextArea * lvl = mAbiTreeDisplayer->CreateTextArea("player-level");
    lvl->SetPosition(5.0f,93.0f,25.0f,10.0f);
    lvl->SetFont(gLocalizator.GetFont(GUI::FONT_DIALOG), 14 );
    lvl->SetText(L"LVL: ?");

    CImageBox* avatar = mAbiTreeDisplayer->CreateImageBox("avatar");
    avatar->SetPosition(95.0f,5.0f,0.0f,0.0f);
    avatar->SetPosition(-120.0f,0.0f,120.0f,120.0f,UNIT_PIXEL);
    avatar->SetViewPosition( sf::Vector2f(50.0f,50.0f) );
    avatar->SetViewSize( 50.0f );
    switch(player){
        case 0: avatar->AddImageToSequence("data/GUI/mag0.png"); break;
        default: avatar->AddImageToSequence("data/GUI/mag20.png"); break;
    }
    
    CButton* close = mBackground->CreateButton("close-button", true, Z_GUI2); // zeby nie bylo pod abi-tree-displayerem
    close->SetPosition(77.f, 3.5f, 15.f, 5.f);// * (float)gGameOptions.GetWidth() / (float)gGameOptions.GetHeight());
    close->SetImage("data/GUI/bbtn-up.png", "data/GUI/bbtn-hover.png", "data/GUI/bbtn-down.png");
    close->GetClickCallback()->bind(gLogic.GetGameScreens(), &CGameScreens::ReturnToGame);
    // temp (wywalic po zmianie grafiki):
    close->SetFont(gLocalizator.GetFont(FONT_DIALOG), 14.f);
    close->SetText(gLocalizator.GetText("ABITREE_CLOSE"));
    close->SetCenter(true);
    
    // zapisz umiejki
    UpdateSprites(0.f);

	gGUI.UnregisterObject(mBackground);
}

CInventoryDisplayer::~CInventoryDisplayer(){

}

void CInventoryDisplayer::UpdateSprites( float secondsPassed ){
    if (mForceReload) {
        mForceReload = false;
        mBar->UpdatePosition();

        CPlayer* player = gPlayerManager.GetPlayerByNumber(mPlayer);
        if (player) {
            for (size_t i = 0; i < CItemSlotsBar::ITEM_SLOTS_COUNT; ++i)
                mBar->SetSlotItem(i, player->GetItem(i), false);
        }

        if (mAbiTreeDisplayer) {
            mAbiTreeDisplayer->ForceReload();

            if (player) {
                for (int i = 0; i < 4; ++i) {
                    GUI::CTextArea* xp = (GUI::CTextArea*) mAbiTreeDisplayer->FindObject("xptext");
                    std::wstringstream s;
                    s << gLocalizator.GetText("UI_XP").c_str() << (int)(player->GetTotalXP()) << "/" << (int)(player->XPRequired());
                    xp->SetText(s.str());

                    GUI::CTextArea* sp = (GUI::CTextArea*) mAbiTreeDisplayer->FindObject("skill-points");
                    s.str(L"");
                    s << gLocalizator.GetText("UI_POINTS").c_str() << (int)(player->GetSkillPoints());
                    sp->SetText(s.str());

                    GUI::CTextArea* lvl = (GUI::CTextArea*) mAbiTreeDisplayer->FindObject("player-level");
                    s.str(L"");
                    s << gLocalizator.GetText("UI_LEVEL").c_str() << player->GetLevel();
                    lvl->SetText(s.str());
                }
            }
        }
    }
}

