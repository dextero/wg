#include "CGameScreens.h"
#include "Editor/CEditorScreens.h"
#include "CWindow.h"
#include "CButton.h"
#include "CTextArea.h"
#include "CProgressBar.h"
#include "CImageBox.h"
#include "CRoot.h"
#include "CAbilityTreeDisplayer.h"
#include "CMultipanel.h"
#include "CSaveScreen.h"
#include "CScrollPanel.h"
#include "CScrollBar.h"
#include "CHud.h"
#include "CCompass.h"
#include "CBossHud.h"
#include "CAbiSlotsBar.h"
#include "CInventoryDisplayer.h"
#include "../Logic/CLogic.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/CPlayer.h"
#include "../Logic/Abilities/CAbility.h"
#include "../Logic/Abilities/CAbilityPerformer.h"
#include "../Utils/StringUtils.h"
#include "../Utils/KeyStrings.h"
#include "../Utils/Directions.h"
#include "../Input/CBindManager.h"
#include "CInteractionTooltip.h"
#include "Notepad/CNotepad.h"
#include "Localization/CLocalizator.h"
#include "Localization/GeneralKeys.h"
#include "../Map/CWorldGraph.h"
#include "../Map/CMapManager.h"
#include "../CGameOptions.h"
#include <algorithm>
#undef CreateWindow

using namespace GUI;

// rAum, 29.08 - taka funkcja coby brzydko ale skutecznie wlaczyc pokazywanie kursora przy sterowaniu
// myszkowym. Trzeba to kiedyś poprawić, na razie to z powodu deadline na jutro.
// tox, 30.07.11 - ciekawe w ktorym roku powyzszy komentarz byl pisany
void ShowCursor(bool show=true)
{
    if (show)								gGUI.ShowCursor(true);
    else if (
		!gBindManager0->GetMouseLook() &&
		!gBindManager1->GetMouseLook() )	gGUI.ShowCursor(false);
}

void CGameScreens::Show(const std::wstring &menu)
{
	if ( menu == L"hud" )
	{
		mHud[0]->Show();
		mHud[1]->Show();
		mCompass->Show();
        mBossHud->Show();
		ShowCursor(false);
	}
	else if ( menu == L"abilities0" )
	{
        //mAbiSlotsBar[0]->Show();
		//mAbilities[0]->SetVisible( true )
		Hide(L"hud");
        UpdateInventory(0);
        mInventory[0]->SetVisible(true);
		//mAbiHelp[0]->SetVisible( true );
		//UpdateAbilities( 0 );
        ShowCursor();
        //SetActiveAbilityScreen(mActiveAbilitiesTab[0]);
	}
	else if ( menu == L"abilities1" )
	{
        //mAbiSlotsBar[1]->Show();
		//mAbilities[1]->SetVisible( true );
        Hide(L"hud");
        UpdateInventory(1);
        mInventory[1]->SetVisible(true);
		//mAbiHelp[1]->SetVisible( true );
		//UpdateAbilities( 1 );
        ShowCursor();
        //SetActiveAbilityScreen(mActiveAbilitiesTab[1]);
	}
	else if ( menu == L"controls0" )
	{
		mControls[0]->SetVisible(true);
		UpdateControlListing(0);
		ShowCursor();
	}
	else if ( menu == L"controls1" )
	{
		mControls[1]->SetVisible(true);
		UpdateControlListing(1);
		ShowCursor();
	}
    else if ( menu == L"load-game" )    { mSaveScreen->UpdateSlots(false); ShowCursor(); }
    else if ( menu == L"game-over" )	{ mGameOver->SetVisible( true ); ShowCursor(); }

#ifndef __EDITOR__
	else if ( menu == L"editor" )       { mEditorScreens->Show(); ShowCursor(); }
#endif

	else if ( menu == L"notepad" )		{ gNotepad.Show(); ShowCursor(); }
    else if ( menu == L"map" ) {
		Hide(L"hud");
        UpdateMap();
        mMap->SetVisible(true);
        ShowCursor();
    }

    mSaveScreen->SetVisible(menu == L"load-game");
    mGameOver->SetVisible(menu == L"game-over");
}

void GUI::CGameScreens::ShowPrevious()
{
    Show(L"game-over");
}

void CGameScreens::Hide(const std::wstring &menu)
{
    if ( menu == L"hud" )
	{
		mHud[0]->Hide();
		mHud[1]->Hide();
		mCompass->Hide();
        mBossHud->Hide();
        for (int i = 0; i < 2; i++) {
            CPlayer * player = gPlayerManager.GetPlayerByNumber(i);
            if (player) {
                player->GetController()->GetInteractionTooltip()->Hide();
            }
        }
	}
    else if ( menu == L"abilities0" || menu == L"abilities1" )
	{
        mAbiSlotsBar[0]->Hide();
        mAbiSlotsBar[0]->UpdatePlayerData();
        mAbiSlotsBar[1]->Hide();
        mAbiSlotsBar[1]->UpdatePlayerData();
		//mAbilities[0]->SetVisible( false );
		//mAbilities[1]->SetVisible( false );
        mInventory[0]->SetVisible(false);
		if (menu == L"abilities0")
			mInventory[0]->UpdatePlayerData();
        mInventory[1]->SetVisible(false);
		if (menu == L"abilities1")
			mInventory[1]->UpdatePlayerData();
		//mAbiHelp[0]->SetVisible( false );
		//mAbiHelp[1]->SetVisible( false );
		ShowCursor(false);

		this->Show(L"hud");
	}
	else if ( menu == L"controls0" || menu == L"controls1" )
	{
		mControls[0]->SetVisible(false);
		mControls[1]->SetVisible(false);
		ShowCursor(false);
	}
    else if ( menu == L"load-game" )    { mSaveScreen->SetVisible( false ); ShowCursor(false); }
    else if ( menu == L"game-over" )	{ mGameOver->SetVisible( false ); ShowCursor(false); }
    else if ( menu == L"editor" )       { mEditorScreens->Hide(); ShowCursor(false); }
	else if ( menu == L"notepad" )		{ gNotepad.Hide(); ShowCursor(false); }
    else if ( menu == L"map")           { mMap->SetVisible(false); this->Show(L"hud"); ShowCursor(false); }
}

void CGameScreens::HideAll()
{
	mHud[0]->Hide();
	mHud[1]->Hide();
    mAbiSlotsBar[0]->Hide();
    mAbiSlotsBar[0]->UpdatePlayerData();
    mAbiSlotsBar[1]->Hide();
    mAbiSlotsBar[1]->UpdatePlayerData();
	mCompass->Hide();
    mBossHud->Hide();
	mAbilities[0]->SetVisible( false );
	mAbilities[1]->SetVisible( false );
	mControls[0]->SetVisible(false);
	mControls[1]->SetVisible(false);
	mGameOver->SetVisible( false );
    mEditorScreens->Hide();
    mSaveScreen->SetVisible(false);
    mInventory[0]->SetVisible(false);
    mInventory[1]->SetVisible(false);
	gNotepad.Hide();
    mMap->SetVisible(false);
    ShowCursor(false);
}

void CGameScreens::InitHud()
{
	mHud[0] = new CHud();
	mHud[0]->Init(0);
	mHud[1] = new CHud();
	mHud[1]->Init(1);
	mCompass = new CCompass();
	mCompass->Init();
    mBossHud = new CBossHud();
    mBossHud->Init();
}

static float markerState = 0.0f;

void CGameScreens::UpdateHud(float dt)
{
	mHud[0]->Update(dt);
	mHud[1]->Update(dt);
	mCompass->Update(dt);
    mBossHud->Update(dt);
    CImageBox* marker = (CImageBox*)(mMap->FindObject("marker"));
    markerState += dt;
    if (markerState >= 2.0f) markerState = 0.0f;
    marker->SetSequenceState(markerState >= 1.0f ? 2.0f - markerState : markerState);
}

void CGameScreens::InitMap()
{
    CWindow* wnd = gGUI.CreateWindow("map", true, Z_GUI3);
    wnd->SetPosition(0.f, 0.f, 100.f, 100.f);
	wnd->SetBackgroundImage( "data/GUI/paper-bkg.jpg" );
    CButton* close = wnd->CreateButton("close-button", true, Z_GUI3 - 2);
    close->SetPosition(91.5f, 2.5f, 5.f, 5.f * (float)gGameOptions.GetWidth() / (float)gGameOptions.GetHeight());
    close->SetImage("data/GUI/bbtn-up.png", "data/GUI/bbtn-hover.png", "data/GUI/bbtn-down.png");
    close->GetClickCallback()->bind(this, &CGameScreens::ReturnToGame);
    // temp (wywalic po zmianie grafiki):
    close->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
    close->SetText(L"X");
    close->SetCenter(true);

    CImageBox* content = wnd->CreateImageBox("content");
    content->SetPosition(0.0f,9.0f,100.0f,83.0f);
    content->AddImageToSequence("data/maps/world-map.png");
    CImageBox* marker = content->CreateImageBox("marker");
    marker->AddImageToSequence("data/GUI/transparent.png");
    marker->AddImageToSequence("data/maps/world-map-mark.png");

    mMap = wnd;
}

void CGameScreens::UpdateMap()
{
    CImageBox* marker = (CImageBox*)(mMap->FindObject("marker"));
    const CWorldGraph & worldGraph = gMapManager.GetWorldGraph();
    const std::string & mapId = gMapManager.GetCurrentMapId();
    if (worldGraph.maps.find(mapId) == worldGraph.maps.end()) {
        return;
    }
    const CWorldGraphMap & mapDef = worldGraph.maps.find(mapId)->second;
    const sf::Vector2f & mapPos = mapDef.mapPos;
    marker->SetPosition(mapPos.x - 2.5f, mapPos.y - 2.5f, 5.0f, 5.0f, GUI::UNIT_PERCENT, false);
}

void CGameScreens::InitAbilities(unsigned playerNumber)
{
    mAbiSlotsBar[0] = new CAbiSlotsBar();
    mAbiSlotsBar[0]->Init(0);
    mAbiSlotsBar[1] = new CAbiSlotsBar();
    mAbiSlotsBar[1]->Init(1);

    if (!mAbilities[playerNumber])
    {
        CWindow* wnd = gGUI.CreateWindow("abilities" + StringUtils::ToString(playerNumber));
        wnd->SetPosition(0.f, 0.f, 100.f, 100.f);
        std::string treesNames[] = { "abilities-fire", "abilities-electricity", "abilities-support", "abilities-special" };
        std::string treesKeys[] = { "ABITREE_FIRE", "ABITREE_ELECTRICITY", "ABITREE_SUPPORT", "ABITREE_SPECIAL" };
        CWindow* trees[4];

        for (int i = 0; i < 4; ++i)
        {
            trees[i] = wnd->CreateWindow(treesNames[i]);
            trees[i]->SetPosition(0.f, 0.f, 100.f, 100.f);

            CButton* btn[4];
            for (int j = 0; j < 4; ++j)
            {
                btn[j] = trees[i]->CreateButton("tree-switcher" + StringUtils::ToString(j));
                btn[j]->SetPosition(8.f + j * 16.f, 3.5f, 15.f, 5.f);
                btn[j]->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
                btn[j]->SetText(gLocalizator.GetText(treesKeys[j].c_str()));
                btn[j]->GetClickParamCallback()->bind(this, &CGameScreens::SetActiveAbilityScreen);
                btn[j]->SetClickCallbackParams(StringUtils::ConvertToWString(treesNames[j]));
                btn[j]->SetImage((i == j ? "data/GUI/bbtn-hover.png" : "data/GUI/bbtn-up.png"), "data/GUI/bbtn-hover.png", "data/GUI/bbtn-down.png");
                btn[j]->SetCenter(true);
            }

            CButton* close = trees[i]->CreateButton("close-button", true, Z_GUI2); // zeby nie bylo pod abi-tree-displayerem
            close->SetPosition(77.f, 3.5f, 15.f, 5.f);// * (float)gGameOptions.GetWidth() / (float)gGameOptions.GetHeight());
            close->SetImage("data/GUI/bbtn-up.png", "data/GUI/bbtn-hover.png", "data/GUI/bbtn-down.png");
            close->GetClickCallback()->bind(this, &CGameScreens::ReturnToGame);
            // temp (wywalic po zmianie grafiki):
            close->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            close->SetText(gLocalizator.GetText("ABITREE_CLOSE"));
            close->SetCenter(true);
        }

        CAbilityTreeDisplayer* atd[4];

        atd[0] = trees[0]->CreateAbilityTreeDisplayer("abilities", playerNumber, "fire");
        atd[1] = trees[1]->CreateAbilityTreeDisplayer("abilities", playerNumber, "electricity");
        atd[2] = trees[2]->CreateAbilityTreeDisplayer("abilities", playerNumber, "support");
        atd[3] = trees[3]->CreateAbilityTreeDisplayer("abilities", playerNumber, "special");

        for (int i = 0; i < 4; ++i)
        {
            atd[i]->SetPosition(5.f,8.f,90.0f,90.0f);
            atd[i]->SetHolderImage("data/GUI/abilities_scroll_empty.png",0.0f);
			atd[i]->SetActiveHolderImage("data/GUI/abilities_scroll_bought.png");
			atd[i]->SetPotentialHolderImage("data/GUI/abilities_scroll_potential.png");
            atd[i]->SetDependencyLineImage("data/GUI/abilities-dependency-line.png");
            const GUI::FontSetting & fs = gGUI.GetFontSetting("FONT_IN_GAME_TEXT");
            atd[i]->SetLvlTextFont(fs.name, fs.size);

			CTextArea * xp = atd[i]->CreateTextArea("xptext");
			xp->SetPosition(5.0f,86.0f,25.0f,10.0f);
			xp->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
			xp->SetText(L"XP: ?");

			CTextArea * gold = atd[i]->CreateTextArea("gold");
			gold->SetPosition(5.0f,89.0f,25.0f,15.0f);
			gold->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
			gold->SetText(L"GOLD: ?");

			CTextArea * sp = atd[i]->CreateTextArea("skill-points");
			sp->SetPosition(5.0f,92.0f,25.0f,10.0f);
			sp->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
			sp->SetText(L"SP: ?");

			CTextArea * lvl = atd[i]->CreateTextArea("player-level");
			lvl->SetPosition(5.0f,95.0f,25.0f,10.0f);
			lvl->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
			lvl->SetText(L"LVL: ?");

			CImageBox* avatar = atd[i]->CreateImageBox("avatar");
			avatar->SetPosition(95.0f,5.0f,0.0f,0.0f);
			avatar->SetPosition(-120.0f,0.0f,120.0f,120.0f,UNIT_PIXEL);
			avatar->SetViewPosition( sf::Vector2f(50.0f,50.0f) );
			avatar->SetViewSize( 50.0f );
			switch(playerNumber){
				case 0:	avatar->AddImageToSequence("data/GUI/mag0.png"); break;
				default: avatar->AddImageToSequence("data/GUI/mag20.png"); break;
			}
        }

        mAbilities[playerNumber] = wnd;

        SetActiveAbilityScreen(StringUtils::ConvertToWString(treesNames[0]));
    }
}

void CGameScreens::UpdateAbilities(unsigned playerNumber)
{
    CWindow* wnd = (CWindow*) mAbilities[playerNumber];
	//CMultipanel * mp = (CMultipanel*) mAbilities[playerNumber];
	CPlayer * player = gPlayerManager.GetPlayerByNumber(playerNumber);
	if (wnd && player){
        std::string treesNames[] = { "abilities-fire", "abilities-electricity", "abilities-support", "abilities-special" };

		Hide(L"hud");

		for (int i = 0; i < 4; ++i)
		{
			((CAbilityTreeDisplayer*)(wnd->FindObject(treesNames[i])->FindObject("abilities")))->ForceReload();
			UpdateAbilitiesTextboxes(playerNumber);
		}
	}
}

void CGameScreens::UpdateAbilitiesTextboxes(unsigned playerNumber){
    CWindow* wnd = (CWindow*) mAbilities[playerNumber];
	//CMultipanel * mp = (CMultipanel*) mAbilities[playerNumber];
	CPlayer * player = gPlayerManager.GetPlayerByNumber(playerNumber);
	if (wnd && player){
        std::string treesNames[] = { "abilities-fire", "abilities-electricity", "abilities-support", "abilities-special" };

		for (int i = 0; i < 4; ++i){
            CWindow* tree = (CWindow*)wnd->FindObject(treesNames[i]);
			std::wstringstream s;
			GUI::CTextArea* xp = (GUI::CTextArea*) tree->FindObject("xptext");
			s << gLocalizator.GetText("UI_XP").c_str() << (int)(player->GetTotalXP()) << "/" << (int)(player->XPRequired());
			xp->SetText(s.str());

            GUI::CTextArea* gold = (GUI::CTextArea*) tree->FindObject("gold");
			s.str(L"");
			s << gLocalizator.GetText("UI_GOLD").c_str() << player->GetGold();
			gold->SetText(s.str());

			GUI::CTextArea* sp = (GUI::CTextArea*) tree->FindObject("skill-points");
			s.str(L"");
			s << gLocalizator.GetText("UI_POINTS").c_str() << (int)(player->GetSkillPoints());
			sp->SetText(s.str());

			GUI::CTextArea* lvl = (GUI::CTextArea*) tree->FindObject("player-level");
			s.str(L"");
			s << gLocalizator.GetText("UI_LEVEL").c_str() << player->GetLevel();
			lvl->SetText(s.str());
		}
	}
}

void CGameScreens::SetActiveAbilityScreen( const std::wstring& name )
{
    int i = -1;
    if (mAbilities[0] && mAbilities[0]->IsVisible())
        i = 0;
    else if (mAbilities[1] && mAbilities[1]->IsVisible())
        i = 1;
    else
        return;

    if (!mAbilities[i])
        return;

    CWindow* parent = dynamic_cast<CWindow*>(mAbilities[i]);
    if (!parent)
        return;
    CWindow* tree = dynamic_cast<CWindow*>(parent->FindObject(StringUtils::ConvertToString(name)));
    if (!tree)
        return;

    mActiveAbilitiesTab[i] = name;

    if (parent->IsVisible())
    {
        // ukryj dzieci
        parent->SetVisible(false);
        parent->SetVisible(true, false);

        // pokaz co trzeba
        tree->SetVisible(true);
    }

    CAbilityTreeDisplayer* atd = dynamic_cast<CAbilityTreeDisplayer*>(tree->FindObject("abilities"));
    if (atd)
        atd->ForceReload();
}

void CGameScreens::InitInventory(unsigned int playerNumber)
{
    if (!mInventory[playerNumber])
        mInventory[playerNumber] = gGUI.CreateInventoryDisplayer("inventory" + StringUtils::ToString(playerNumber), playerNumber);
}

void CGameScreens::UpdateInventory(unsigned int playerNumber)
{
    if (!mInventory[playerNumber])
        InitInventory(playerNumber);
    mInventory[playerNumber]->ForceReload();
}

void CGameScreens::InitControlListing(unsigned playerNumber)
{
    if ( !mControls[playerNumber] )
	{
		CWindow * controls = gGUI.CreateWindow( "controls", true, Z_GUI3 );
		controls->SetBackgroundImage( "data/GUI/paper-bkg.jpg" );
		//controls->SetPosition(00.0f,00.0f,0.0f,0.0f,UNIT_PERCENT);
		controls->SetPosition(0.0f, 0.0f, 100.0f, 100.0f,UNIT_PERCENT);

        CButton* close = controls->CreateButton("close-button", true, Z_GUI3 - 2); // zeby nie bylo pod reszta
        close->SetPosition(91.5f, 2.5f, 5.f, 5.f * (float)gGameOptions.GetWidth() / (float)gGameOptions.GetHeight());
        close->SetImage("data/GUI/bbtn-up.png", "data/GUI/bbtn-hover.png", "data/GUI/bbtn-down.png");
        close->GetClickCallback()->bind(this, &CGameScreens::ReturnToGame);
        // temp (wywalic po zmianie grafiki):
        close->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
        close->SetText(L"X");
        close->SetCenter(true);

		CScrollBar * sbY = controls->CreateScrollBar( "sy" );
		sbY->SetPosition(82.0f, 12.0f, 2.5f, 75.0f);
		sbY->SetBackgroundImage("data/GUI/scrollbar-v.png");
		sbY->SetHandleImage("data/GUI/scrollbar-handle.png");
		sbY->SetOrientation( ORIENTATION_Y );
		sbY->SetHandleSize(4.0f);

		CScrollPanel * panel = controls->CreateScrollPanel( "controls-scroll" );
		panel->SetPosition(14.0f,12.0f,68.0f,75.0f);
		panel->SetScrollBarY(sbY);

		CImageBox* avatar = panel->CreateImageBox("avatar");
		avatar->SetPosition(100.0f,0.0f,0.0f,0.0f);
		avatar->SetPosition(-120.0f,0.0f,120.0f,120.0f,UNIT_PIXEL);
		avatar->SetViewPosition( sf::Vector2f(50.0f,50.0f) );
		avatar->SetViewSize( 50.0f );
		switch(playerNumber){
			case 0:	avatar->AddImageToSequence("data/GUI/mag0.png"); break;
			default: avatar->AddImageToSequence("data/GUI/mag20.png"); break;
			}

		CTextArea *txt = panel->CreateTextArea("help");
		txt->SetFont(gGUI.GetFontSetting("FONT_IN_GAME_TEXT"));
		txt->SetPosition(0.0f,0.0f,100.0f,0.0f);
		txt->SetAutoHeight(true);

        // aktualne klawisze:
        System::Input::CBindManager* bindMgr0 = System::Input::CBindManager::GetActualBindManager(0);
        System::Input::CBindManager* bindMgr1 = System::Input::CBindManager::GetActualBindManager(1);
        std::wstring bindKeys[4] = {
            StringUtils::ConvertToWString(KeyStrings::KeyToString(bindMgr0->GetKeyBindings().find("Help")->second)),
            StringUtils::ConvertToWString(KeyStrings::KeyToString(bindMgr1->GetKeyBindings().find("Help")->second)),
            StringUtils::ConvertToWString(KeyStrings::KeyToString(bindMgr0->GetKeyBindings().find("Abilities")->second)),
            StringUtils::ConvertToWString(KeyStrings::KeyToString(bindMgr1->GetKeyBindings().find("Abilities")->second))
        };

		txt->SetText(gLocalizator.GetTextf("HELP_SCREENS_CONTROLS", bindKeys[0].c_str(), bindKeys[1].c_str(), bindKeys[2].c_str(), bindKeys[3].c_str()));
		txt->SetColor(sf::Color::Black);

		txt = txt->CreateTextArea("controls-txt");
		txt->SetFont(gGUI.GetFontSetting("FONT_IN_GAME_TEXT"));
		txt->SetPosition(0.0f,100.0f,100.0f,0.0f);
		txt->SetAutoHeight(true);
		txt->SetColor(sf::Color::Black);

		CTextArea *txt2 = txt->CreateTextArea("abilities-txt-left");
		txt2->SetFont(gGUI.GetFontSetting("FONT_IN_GAME_TEXT", 1.2f));
		txt2->SetPosition(0.0f,100.0f,40.0f,0.0f);
		txt2->SetAutoHeight(true);
		txt2->SetColor(sf::Color::Black);

        txt2 = txt->CreateTextArea("abilities-txt-right");
		txt2->SetFont(gGUI.GetFontSetting("FONT_IN_GAME_TEXT", 1.2f));
		txt2->SetPosition(40.0f,100.0f,60.0f,0.0f);
		txt2->SetAutoHeight(true);
		txt2->SetColor(sf::Color::Black);

		mControls[playerNumber] = (CGUIObject*) controls;
	}
}

void CGameScreens::UpdateControlListing(unsigned playerNumber)
{
    if ( CPlayer * player = gPlayerManager.GetPlayerByNumber(playerNumber) )
	{
        CTextArea *txt = (CTextArea*) mControls[playerNumber]->FindObject("help");

        // aktualne klawisze:
        System::Input::CBindManager* bindMgr0 = System::Input::CBindManager::GetActualBindManager(0);
        System::Input::CBindManager* bindMgr1 = System::Input::CBindManager::GetActualBindManager(1);
        std::wstring bindKeys[4] = {
            StringUtils::ConvertToWString(KeyStrings::KeyToString(bindMgr0->GetKeyBindings().find("Help")->second)),
            StringUtils::ConvertToWString(KeyStrings::KeyToString(bindMgr1->GetKeyBindings().find("Help")->second)),
            StringUtils::ConvertToWString(KeyStrings::KeyToString(bindMgr0->GetKeyBindings().find("Abilities")->second)),
            StringUtils::ConvertToWString(KeyStrings::KeyToString(bindMgr1->GetKeyBindings().find("Abilities")->second))
        };

		txt->SetText(gLocalizator.GetTextf("HELP_SCREENS_CONTROLS", bindKeys[0].c_str(), bindKeys[1].c_str(), bindKeys[2].c_str(), bindKeys[3].c_str()));


		std::wostringstream sLeft, sRight;
		std::vector<SAbilityInstance> *abilities = player->GetAbilityPerformer().GetAbilities();

        CTextArea * controlsTxt = (CTextArea*) mControls[playerNumber]->FindObject( "controls-txt" );
        controlsTxt->SetText( gBindManagerByPlayer(playerNumber)->GetControlsDescription() );

		CTextArea * left = (CTextArea*) mControls[playerNumber]->FindObject( "abilities-txt-left" );
		CTextArea * right = (CTextArea*) mControls[playerNumber]->FindObject( "abilities-txt-right" );

		for (unsigned int i = 0; i < ABI_SLOTS_COUNT; ++i)
		{
			if (mAbiSlotsBar[playerNumber]->GetSelectedAbility(i) != NULL)
			{
				sLeft << mAbiSlotsBar[playerNumber]->GetSelectedAbility(i)->name << L"\n";
				
				if ( gBindManagerByPlayer(playerNumber) != NULL )
				{
					const std::map<std::string,int>& keys = gBindManagerByPlayer(playerNumber)->GetKeyBindings();
					switch(i)
					{
						case 0:
							if (keys.count("Abi-0") > 0)
								sRight << StringUtils::ConvertToWString(KeyStrings::KeyToString( keys.find("Abi-0")->second )) << L"\n";
                            else if (keys.count("Slot-0") > 0)
								sRight << StringUtils::ConvertToWString(KeyStrings::KeyToString( keys.find("Slot-0")->second )) << L"\n";
							break;
						case 1:
							if (keys.count("Abi-1") > 0)
								sRight << StringUtils::ConvertToWString(KeyStrings::KeyToString( keys.find("Abi-1")->second )) << L"\n";
                            else if (keys.count("Slot-1") > 0)
								sRight << StringUtils::ConvertToWString(KeyStrings::KeyToString( keys.find("Slot-1")->second )) << L"\n";
							break;
						case 2:
							if (keys.count("Abi-2") > 0)
								sRight << StringUtils::ConvertToWString(KeyStrings::KeyToString( keys.find("Abi-2")->second )) << L"\n";
                            else if (keys.count("Slot-2") > 0)
								sRight << StringUtils::ConvertToWString(KeyStrings::KeyToString( keys.find("Slot-2")->second )) << L"\n";
							break;
					}
				}

			}
		}

        sLeft << L"\n";
        sRight << L"\n";

        for ( unsigned i = 0; i < abilities->size(); i++ )
        {
            if ( (abilities->at(i).ability->abilityType!=atPassive )
				&& (abilities->at(i).ability->abilityType!=atExport )
				&& (abilities->at(i).ability->trigger.size() > 1))
			{
                std::string t = abilities->at(i).ability->trigger;
				std::string t_;
				std::string	aReplace;
				std::string	bReplace;
				std::string	cReplace;
				std::string	dReplace;

				if ( gBindManagerByPlayer(playerNumber) != NULL )
				{
					const std::map<std::string,int>& keys = gBindManagerByPlayer(playerNumber)->GetKeyBindings();

					if ( keys.count("Abi-0") > 0 )	aReplace = KeyStrings::KeyToString( keys.find("Abi-0")->second );
                    else if ( keys.count("AbiX-0") > 0 ) aReplace = KeyStrings::KeyToString( keys.find("AbiX-0")->second );
					if ( keys.count("Abi-1") > 0 )	bReplace = KeyStrings::KeyToString( keys.find("Abi-1")->second );
                    else if ( keys.count("AbiX-1") > 0 ) bReplace = KeyStrings::KeyToString( keys.find("AbiX-1")->second );
					if ( keys.count("Abi-2") > 0 )	cReplace = KeyStrings::KeyToString( keys.find("Abi-2")->second );
                    else if ( keys.count("AbiX-2") > 0 ) cReplace = KeyStrings::KeyToString( keys.find("AbiX-2")->second );
					if ( keys.count("Abi-3") > 0 )	dReplace = KeyStrings::KeyToString( keys.find("Abi-3")->second );
                    else if ( keys.count("AbiX-3") > 0 ) dReplace = KeyStrings::KeyToString( keys.find("AbiX-3")->second );
				}

				for ( unsigned j = 0; j < t.size(); j++ )
				{
					switch ( t[j] )
					{
					case 'a':	t_ += aReplace; break;
					case 'b':	t_ += bReplace; break;
					case 'c':	t_ += cReplace; break;
					default:	t_ += dReplace; break;
					}

					if ( j < t.size() - 1 )
						t_ += ", ";
				}

				sLeft << abilities->at(i).ability->name << L"\n";
				sRight << StringUtils::ConvertToWString(t_) << L"\n";
            }
        }

		left->SetText( sLeft.str() );
		right->SetText( sRight.str() );
	}
}

void CGameScreens::InitGameOver()
{
	if ( !mGameOver )
	{
		CWindow * gameOver = gGUI.CreateWindow( "game-over", true, Z_GUI3 );
        //TODO: tymczasowe, zmienic, gdy pojawi sie rAum
        gameOver->SetBackgroundImage( StringUtils::ConvertToString(gLocalizator.GetText("GUI_POSTMORTEM_BG")) );
		gameOver->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

		CButton * load = gameOver->CreateButton( "load-game-button" );
		load->SetImage( "data/GUI/btn-blank.png", "data/GUI/btn-blankd.png" );
        load->SetText(gLocalizator.GetText("MENU_LOAD_GAME"));
        load->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        load->SetCenter(true);
		load->SetPosition( 20.0f, 90.0f, 20.0f, 6.0f );
//		load->GetClickCallback()->bind( &gLogic, &CLogic::LoadGame );
 		load->GetClickParamCallback()->bind( this, &CGameScreens::Show );
 		load->SetClickCallbackParams( L"load-game" );

        CButton * exit = gameOver->CreateButton( "exit-button" );
		exit->SetImage( "data/GUI/btn-blank.png", "data/GUI/btn-blankd.png" );
        exit->SetText(gLocalizator.GetText("MENU_EXIT"));
        exit->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        exit->SetCenter(true);
		exit->SetPosition( 60.0f, 90.0f, 20.0f, 6.0f );
		exit->GetClickCallback()->bind( &gLogic, &CLogic::CheckRestart );

		mGameOver = (CGUIObject*) gameOver;
	}
}

CHud* CGameScreens::GetHud(unsigned int player)
{
    return mHud[player];
}

CAbiSlotsBar* CGameScreens::GetAbiBar(unsigned int player)
{
    return mAbiSlotsBar[player];
}

CGUIObject* CGameScreens::GetAbilities(unsigned int player)
{
    return mAbilities[player];
}

CInventoryDisplayer* CGameScreens::GetInventoryDisplayer(unsigned int player)
{
    return mInventory[player];
}

void CGameScreens::ReturnToGame()
{
    // dex: nie mam lepszego pomysłu, jak to zrobić
    // bez grzebania w kodzie CLogic
    sf::Event::KeyEvent e;
    e.Code = sf::Key::Escape;
    gLogic.KeyPressed(e);
}

void CGameScreens::ResetCompass()
{
    mCompass->ClearExit();
}

void CGameScreens::SetSlotAbility(unsigned int player, unsigned int slot, CAbility* abi)
{
    mAbiSlotsBar[player]->SetSlotAbility(slot, abi);
}

CGameScreens::CGameScreens()
:	mGameOver( NULL ),
    mSaveScreen( NULL ),
    mEditorScreens( NULL ),
    mMap(NULL)
{
	mHud[0] = mHud[1] = NULL;
	mCompass = NULL;
    mBossHud = NULL;
	mAbilities[0] = mAbilities[1] = NULL;
    mActiveAbilitiesTab[0] = mActiveAbilitiesTab[1] = L"abilities-fire";
	mAbiHelp[0] = mAbiHelp[1] = NULL;
	mControls[0] = mControls[1] = NULL;
    mInventory[0] = mInventory[1] = NULL;

	InitHud();
	InitAbilities(0);
	InitAbilities(1);
	InitControlListing(0);
	InitControlListing(1);
	InitGameOver();
    InitInventory(0);
    InitInventory(1);
    InitMap();
    mSaveScreen = new CSaveScreen(this);
    mEditorScreens = new CEditorScreens();
	HideAll();
}

CGameScreens::~CGameScreens()
{
}

