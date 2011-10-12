#include "CMenuScreens.h"
#include "CWindow.h"
#include "CButton.h"
#include "CTextArea.h"
#include "CImageBox.h"
#include "CAnimatedImage.h"
#include "CCheckBox.h"
#include "CDropList.h"
#include "CScrollBar.h"
#include "CScrollPanel.h"
#include "CKeyReader.h"
#include "CRoot.h"
#include "CSaveScreen.h"
#include "../Commands/CCommands.h"
#include "../Logic/Achievements/CAchievementManager.h"
#include "../Logic/Factory/CEnemyTemplate.h"
#include "../Logic/Quests/CQuestManager.h"
#include "../Logic/CBestiary.h"
#include "../Logic/CLogic.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Input/CBindManager.h"
#include "../Input/CMouseCaster.h"
#include "../Rendering/Animations/CAnimSet.h"
#include "../Rendering/Animations/SAnimation.h"
#include "../Rendering/Animations/STimedAnimationFrame.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Utils/FileUtils.h"
#include "../Utils/StringUtils.h"
#include "../Utils/KeyStrings.h"
#include "../Utils/FileUtils.h"
#include "../CTimeManipulator.h"
#include "Localization/CLocalizator.h"
#include "Localization/GeneralKeys.h"
#include "Localization/OptionsKeys.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <fstream>

#undef CreateWindow

using namespace GUI;

bool enteredOptionsFromChooseControlsScheme = false;

void CMenuScreens::Show(const std::wstring &menu)
{
    InitAll();

    if (menu == L"")
    {
        gGame.SetPause(false);
        gLogic.SetState(L"playing");
    }
    // NIE chcemy zapisywac loading-screena w prevMenus!
    else if (menu != L"loading")
    {
        mPrevMenus.push(menu);

#ifndef __EDITOR__
        gGame.SetPause(true);
#endif
    }

	mMainMenu->SetVisible( menu == L"main-menu" );
    mPauseMenu->SetVisible( menu == L"pause-menu" );
	mNewGameOptions->SetVisible( menu == L"new-game-options" );
    mChooseControlsMenu->SetVisible( menu == L"choose-controls-menu" );
	mOptions->SetVisible( menu == L"options" );
    if (menu == L"options-from-choose-controls") {
	    mOptions->SetVisible(true);
        enteredOptionsFromChooseControlsScheme = true;
    }
	mBindingOptions[0]->SetVisible( menu == L"binding-options0" );
	mBindingOptions[1]->SetVisible( menu == L"binding-options1" );
    mReadmeScreen->SetVisible( menu == L"readme-screen" );
	mSaveScreen->SetVisible( menu == L"save-screen" || menu == L"load-screen" );
	mLoadingScreen->SetVisible( menu == L"loading" );
    mAchievementsScreen->SetVisible( menu == L"achievements" );
    mBestiaryScreen->SetVisible( menu == L"bestiary" );
	gGUI.ShowCursor( menu != L"" );

    if ( menu == L"binding-options0" )	UpdateBindingOptions(0);
	else if ( menu == L"binding-options1" )	UpdateBindingOptions(1);
    else if ( menu == L"save-screen" || menu == L"load-screen" )
        UpdateSaveScreen( menu == L"save-screen" );
    else if ( menu == L"achievements" ) UpdateAchievementsScreen();
    else if ( menu == L"bestiary" ) UpdateBestiaryScreen();
}

void CMenuScreens::ShowPrevious()
{
    if (mPrevMenus.size())
        mPrevMenus.pop();

    if (mPrevMenus.size())
    {
        Show(mPrevMenus.top());
        RestoreOptions();
        mPrevMenus.pop();   // wywala push() wykonane w Show()
    }
    else if (gLogic.GetState() != L"main-menu")
    {
        Show(L"");
        gGame.SetPause(false);
    }
    else    // jakby cos sie zacielo, wychodz do menu glownego..
        Show(L"main-menu");
}

void CMenuScreens::HideAll()
{
    InitAll();
	mMainMenu->SetVisible( false );
    mPauseMenu->SetVisible( false );
	mNewGameOptions->SetVisible( false );
    mChooseControlsMenu->SetVisible( false );
	mOptions->SetVisible( false );
    mReadmeScreen->SetVisible( false );
	mSaveScreen->SetVisible( false );
	mLoadingScreen->SetVisible( false );
    mAchievementsScreen->SetVisible( false );
    mBestiaryScreen->SetVisible( false );
    gGUI.ShowCursor(false);
}

void CMenuScreens::InitAll()
{
	InitMainMenu();
    InitPauseMenu();
	InitNewGameOptions();
    InitChooseControlsMenu();
	InitOptions();
	UpdateOptions();
	InitBindingOptions();
    InitReadmeScreen();
	InitSaveScreen();
	InitLoadingScreen();
    InitAchievementsScreen();
    InitBestiaryScreen();
}

void CMenuScreens::InitMainMenu()
{
	if ( !mMainMenu )
	{
		CWindow * mainMenu = gGUI.CreateWindow( "main-menu" );
		mainMenu->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
		mainMenu->SetBackgroundImage( "data/GUI/bg-mainmenu.jpg" );

		CButton* newGameButton = mainMenu->CreateButton( "new-game-button" );
        newGameButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        newGameButton->SetText(gLocalizator.GetText("MENU_NEW_GAME"));
		newGameButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		newGameButton->SetPosition( 30.0f, 25.0f, 40.0f, 10.0f );
		newGameButton->SetCenter( true );
		newGameButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		newGameButton->SetClickCallbackParams( L"new-game-options" );

		CButton* loadGameButton = mainMenu->CreateButton( "load-game-button" );
        loadGameButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        loadGameButton->SetText(gLocalizator.GetText("MENU_LOAD_GAME"));
		loadGameButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		loadGameButton->SetPosition( 30.0f, 35.0f, 40.0f, 10.0f );
		loadGameButton->SetCenter( true );
//        loadGameButton->GetClickCallback()->bind( CLogic::GetSingletonPtr(), &CLogic::LoadGame );
 		loadGameButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
 		loadGameButton->SetClickCallbackParams( L"load-screen" );

		CButton* optionsButton = mainMenu->CreateButton( "options-button" );
        optionsButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        optionsButton->SetText(gLocalizator.GetText("MENU_OPTIONS"));
		optionsButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		optionsButton->SetPosition( 30.0f, 45.0f, 40.0f, 10.0f );
		optionsButton->SetCenter( true );
		optionsButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		optionsButton->SetClickCallbackParams( L"options" );

        CButton* creditsButton = mainMenu->CreateButton( "readme-button" );
        creditsButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        creditsButton->SetText(gLocalizator.GetText("MENU_README"));
		creditsButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		creditsButton->SetPosition( 30.0f, 55.0f, 40.0f, 10.0f );
		creditsButton->SetCenter( true );
		creditsButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		creditsButton->SetClickCallbackParams( L"readme-screen" );

        /*CButton* achievementsButton = mainMenu->CreateButton( "achievements-button" );
        achievementsButton->SetText( gLocalizator.Get("MENU_ACHIEVEMENTS"), gLocalizator.GetFont(GUI::FONT_MENU) );
		achievementsButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		achievementsButton->SetPosition( 30.0f, 59.0f, 40.0f, 10.0f );
		achievementsButton->SetCenter( true );
		achievementsButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		achievementsButton->SetClickCallbackParams( L"achievements" );*/
        
        CButton* bestiaryButton = mainMenu->CreateButton( "readme-button" );
        bestiaryButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        bestiaryButton->SetText(gLocalizator.GetText("MENU_BESTIARY"));
		bestiaryButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		bestiaryButton->SetPosition( 30.0f, 65.0f, 40.0f, 10.0f );
		bestiaryButton->SetCenter( true );
		bestiaryButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		bestiaryButton->SetClickCallbackParams( L"bestiary" );

        if (mFlags & MENUSCREEN_SHOW_EDITOR)
        {
		    CButton* editorButton = mainMenu->CreateButton( "editor-button" );
            editorButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
            editorButton->SetText(gLocalizator.GetText("MENU_EDITOR"));
		    editorButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		    editorButton->SetPosition( 30.0f, 75.0f, 40.0f, 10.0f );
		    editorButton->SetCenter( true );
		    editorButton->GetClickCallback()->bind( CLogic::GetSingletonPtr(), &CLogic::StartEditor );
        }

		CButton* exitGameButton = mainMenu->CreateButton( "exit-game-button" );
        exitGameButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        exitGameButton->SetText(gLocalizator.GetText("MENU_EXIT"));
		exitGameButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		exitGameButton->SetPosition( 30.0f, 85.0f, 40.0f, 10.0f );
		exitGameButton->SetCenter( true );
		exitGameButton->GetClickCallback()->bind( this, &CMenuScreens::ExitGame );

		mMainMenu = (CGUIObject*) mainMenu;
	}
}

void CMenuScreens::InitPauseMenu()
{
    if ( !mPauseMenu )
	{
		CWindow * pauseMenu = gGUI.CreateWindow( "pause-menu" );
		pauseMenu->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
		pauseMenu->SetBackgroundImage( "data/GUI/bg-mainmenu.jpg" );

        CButton* continueGameButton = pauseMenu->CreateButton( "continue-game-button" );
        continueGameButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        continueGameButton->SetText(gLocalizator.GetText("MENU_CONTINUE"));
		continueGameButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		continueGameButton->SetPosition( 30.0f, 30.0f, 40.0f, 10.0f );
		continueGameButton->SetCenter( true );
		continueGameButton->GetClickCallback()->bind( this, &CMenuScreens::ShowPrevious );

		CButton* newGameButton = pauseMenu->CreateButton( "new-game-button" );
        newGameButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        newGameButton->SetText(gLocalizator.GetText("MENU_NEW_GAME"));
		newGameButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		newGameButton->SetPosition( 30.0f, 39.0f, 40.0f, 10.0f );
		newGameButton->SetCenter( true );
		newGameButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		newGameButton->SetClickCallbackParams( L"new-game-options" );

		CButton* loadGameButton = pauseMenu->CreateButton( "load-game-button" );
        loadGameButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        loadGameButton->SetText(gLocalizator.GetText("MENU_LOAD_GAME"));
		loadGameButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		loadGameButton->SetPosition( 30.0f, 48.0f, 40.0f, 10.0f );
		loadGameButton->SetCenter( true );
		loadGameButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		loadGameButton->SetClickCallbackParams( L"load-screen" );

		CButton* saveGameButton = pauseMenu->CreateButton( "save-game-button" );
        saveGameButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        saveGameButton->SetText(gLocalizator.GetText("MENU_SAVE_GAME"));
		saveGameButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		saveGameButton->SetPosition( 30.0f, 57.0f, 40.0f, 10.0f );
		saveGameButton->SetCenter( true );
		saveGameButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		saveGameButton->SetClickCallbackParams( L"save-screen" );

		CButton* optionsButton = pauseMenu->CreateButton( "options-button" );
        optionsButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        optionsButton->SetText(gLocalizator.GetText("MENU_OPTIONS"));
		optionsButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		optionsButton->SetPosition( 30.0f, 66.0f, 40.0f, 10.0f );
		optionsButton->SetCenter( true );
		optionsButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		optionsButton->SetClickCallbackParams( L"options" );

        CButton* creditsButton = pauseMenu->CreateButton( "readme-button" );
        creditsButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        creditsButton->SetText(gLocalizator.GetText("MENU_README"));
		creditsButton->SetImage ( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		creditsButton->SetPosition( 30.0f, 75.0f, 40.0f, 10.0f );
		creditsButton->SetCenter( true );
		creditsButton->GetClickParamCallback()->bind( this, &CMenuScreens::Show );
		creditsButton->SetClickCallbackParams( L"readme-screen" );

        // wbrew nazwie, wychodzi do main-menu, nie wylacza gry
		CButton* exitGameButton = pauseMenu->CreateButton( "exit-game-button" );
        exitGameButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        exitGameButton->SetText(gLocalizator.GetText("MENU_EXIT"));
		exitGameButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		exitGameButton->SetPosition( 30.0f, 84.0f, 40.0f, 10.0f );
		exitGameButton->SetCenter( true );
        exitGameButton->GetClickParamCallback()->bind( gCommands.GetSingletonPtr(), &CCommands::ParseCommand );
        exitGameButton->SetClickCallbackParams(L"exit-to-main-menu");

		mPauseMenu = pauseMenu;
	}
}

void CMenuScreens::InitNewGameOptions()
{
	if ( !mNewGameOptions )
	{
		CWindow * options = gGUI.CreateWindow( "ng-options" );
		options->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
		options->SetBackgroundImage( "data/GUI/bg-options.jpg" );

//		CButton* tutorialButton = options->CreateButton( "single-player-button" );
//      tutorialButton->SetText( gLocalizator.Get("MENU_1_PLAYER_TUTORIAL"), gLocalizator.GetFont(GUI::FONT_MENU) );
//		tutorialButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
//		tutorialButton->SetPosition( 25.0f, 30.0f, 50.0f, 10.0f );
//		tutorialButton->SetCenter( true );
//		tutorialButton->GetClickParamCallback()->bind( this, &CMenuScreens::TryStartGame );
//		tutorialButton->SetClickCallbackParams( L"new-game-tutorial" );

		CButton* singleButton = options->CreateButton( "single-player-button" );
        singleButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        singleButton->SetText(gLocalizator.GetText("MENU_1_PLAYER"));
		singleButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		singleButton->SetPosition( 25.0f, 43.0f, 50.0f, 10.0f );
		singleButton->SetCenter( true );
        singleButton->GetClickParamCallback()->bind( this, &CMenuScreens::TryStartGame );
        singleButton->SetClickCallbackParams( L"new-game" );

		CButton* hotSeatButton = options->CreateButton( "hot-seat-button" );
        hotSeatButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        hotSeatButton->SetText(gLocalizator.GetText("MENU_2_PLAYERS"));
		hotSeatButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		hotSeatButton->SetPosition( 25.0f, 56.0f, 50.0f, 10.0f );
		hotSeatButton->SetCenter( true );
		hotSeatButton->GetClickParamCallback()->bind( CLogic::GetSingletonPtr(), &CLogic::StartNewGame );
		hotSeatButton->SetClickCallbackParams( L"new-game-hs" );

		CButton* returnButton = options->CreateButton( "return-button" );
        returnButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        returnButton->SetText(gLocalizator.GetText("MENU_RETURN"));
		returnButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		returnButton->SetPosition( 25.0f, 69.0f, 50.0f, 10.0f );
		returnButton->SetCenter( true );
		returnButton->GetClickCallback()->bind( this, &CMenuScreens::ShowPrevious );

        /*CTextArea* diffTxt = options->CreateTextArea("difficulty-string");
        diffTxt->SetFont(gLocalizator.GetFont(FONT_DIALOG), 14.f);
        diffTxt->SetText(gLocalizator.GetText("DIFFICULTY") + gLocalizator.GetText("DIFFICULTY_1"));
        diffTxt->SetPosition(15.f, 23.f, 30.f, 4.f);

        CScrollBar* diff = options->CreateScrollBar("difficulty-bar");
        diff->SetStatesCount(5);
        diff->SetHandleImage("data/GUI/scrollbar-handle.png");
        diff->SetBackgroundImage("data/GUI/scrollbar.png");
        diff->SetPosition(45.0f, 22.0f, 20.0f, 4.0f);
        diff->SetHandleSize(16.0f);
        diff->GetSlideParamCallback()->bind(this, &CMenuScreens::UpdateDifficultyString);
        diff->SetSlideCallbackParams(diffTxt);

        CCheckBox* arcade = options->CreateCheckBox("arcade");
        arcade->SetImage("data/GUI/checkbox-true.png", "data/GUI/checkbox-false.png");
        arcade->SetPosition(70.f, 22.f, 4.f, 5.f);
        arcade->GetEventCallback(MOUSE_RELEASED_LEFT)->bind(this, &CMenuScreens::UpdateArcadeMode);

        CTextArea* arcadeTxt = options->CreateTextArea("arcade-txt");
        arcadeTxt->SetFont(gLocalizator.GetFont(FONT_DIALOG), 14.f);
        arcadeTxt->SetText(gLocalizator.GetText("ARCADE_MODE"));
        arcadeTxt->SetPosition(75.f, 23.f, 15.f, 8.f);*/

		mNewGameOptions = options;
	}
}

void GUI::CMenuScreens::InitChooseControlsMenu()
{
    if (!mChooseControlsMenu)
    {
        CWindow* controls = gGUI.CreateWindow("choose-controls-menu");
        controls->SetPosition(0.f, 0.f, 100.f, 100.f);
        controls->SetBackgroundImage("data/GUI/bg-options.jpg");

        CTextArea* header = controls->CreateTextArea("header");
        header->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON", (30.f/38.f)));
        header->SetText(gLocalizator.GetText("CHOOSE_CONTROLS_SCHEME"));
        header->SetPosition(0.f, 10.f, 100.f, 10.f);
        header->SetCenter(true);

        // ile schematow juz dodanych (do pozycji buttonow)
        size_t bindsAdded = 0;

        for (size_t i = 0; i < System::Input::CBindManager::GetBindManagersCount(); ++i)
        {
            // czy pokazywac ten schemat sterowania w menu?
            if (!System::Input::CBindManager::GetBindManagerAt(i)->GetShowOnFirstGame())
                continue;

            CButton* btn = controls->CreateButton("controls" + StringUtils::ToString(i));
            btn->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON", (0.20f/0.38f)));
            btn->SetText(System::Input::CBindManager::GetBindManagerAt(i)->GetName());
		    btn->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
		    btn->SetPosition(10.f + (bindsAdded % 2) * 40.f, 30.f + (int)(bindsAdded / 2) * 15.f, 40.0f, 10.0f);
		    btn->SetCenter(true);
            btn->GetClickParamCallback()->bind(this, &CMenuScreens::ChooseControls);
            btn->SetClickCallbackParams(System::Input::CBindManager::GetBindManagerAt(i)->GetName());

            if (btn != NULL)
            {
                CTextArea* tooltip = gGUI.CreateTextArea("choose-controls" + StringUtils::ToString(i) + "_tooltip", true, Z_GUI4);
                tooltip->SetPosition(0.f, 0.f, 400.f, 0.f, UNIT_PIXEL);
                tooltip->SetBackgroundImage("data/GUI/transparent-black.png");
                tooltip->SetVisible(false);
                //tooltip->SetCenter(true); // �eeee�oooo�eeee�oooo bug alert, nie centrowac
                tooltip->SetAutoHeight(true);
                tooltip->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
                tooltip->SetText(System::Input::CBindManager::GetBindManagerAt(i)->GetControlsMenuDescription()
                        + L"\n" + System::Input::CBindManager::GetBindManagerAt(i)->GetControlsDescription());
                tooltip->SetPadding(20.f, 20.f, 20.f, 20.f, UNIT_PIXEL);
                if (bindsAdded % 2)
                    tooltip->SetOffset(sf::Vector2f(-400.f - btn->ConvertToGlobalPosition(btn->GetSize() / 100.f).x, 20.f));
                else
                    tooltip->SetOffset(sf::Vector2f(400.f + btn->ConvertToGlobalPosition(btn->GetSize() / 100.f).x, 20.f));

                btn->SetTooltip(tooltip);
            }

            ++bindsAdded;
        }

        CTextArea* moreSchemesTxt = controls->CreateTextArea("more-schemes-txt");
        moreSchemesTxt->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
        moreSchemesTxt->SetText(gLocalizator.GetText("MORE_CONTROL_SCHEMES"));
        moreSchemesTxt->SetPosition(15.f, 63.f, 55.f, 11.f);

        CButton* options = controls->CreateButton("options");
        options->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
        options->SetText(gLocalizator.GetText("MENU_OPTIONS"));
		options->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
        options->SetPosition(69.f, 63.f, 13.f, 6.f);
        options->SetCenter(true);
        options->GetClickParamCallback()->bind(this, &CMenuScreens::Show);
        options->SetClickCallbackParams( L"options-from-choose-controls" );

        CButton* ret = controls->CreateButton("return");
        ret->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        ret->SetText(gLocalizator.GetText("MENU_RETURN"));
		ret->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
        ret->SetPosition(30.f, 80.f, 40.f, 10.f);
        ret->SetCenter(true);
        ret->GetClickCallback()->bind(this, &CMenuScreens::ShowPrevious);

        mChooseControlsMenu = controls;
    }
}

void GUI::CMenuScreens::InitSaveScreen()
{
	if (!mSaveScreen)
        mSaveScreen = new CSaveScreen(this);
}

void GUI::CMenuScreens::UpdateSaveScreen(bool save)
{
    mSaveScreen->UpdateSlots(save);
}

void CMenuScreens::InitOptions()
{
	if ( !mOptions )
	{
		CWindow * options = gGUI.CreateWindow( "options" );
		options->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
		options->SetBackgroundImage( "data/GUI/bg-options.jpg" );

		CTextArea * resolutionTxt = options->CreateTextArea( "resolution-txt" );
		resolutionTxt->SetFont(gGUI.GetFontSetting("FONT_MENU_LABEL"));
		resolutionTxt->SetText( gLocalizator.GetText("OPT_RESOLUTION") );
		resolutionTxt->SetPosition( 19.0f, 15.0f, 30.0f, 5.0f );

		CDropList * resolution = options->CreateDropList( "resolution" );
		resolution->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
		resolution->SetImage( "data/GUI/droplist.png", "data/GUI/droplistd.png" );
		resolution->SetCenter( true );
		resolution->SetOptionFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
		resolution->SetOptionImage( "data/GUI/droplist-option.png", "data/GUI/droplist-optiond.png" );
		resolution->SetOptionCenter( true );
		resolution->SetPosition( 51.0f, 15.0f, 20.0f, 4.0f );

		/* listowanie rozdzielczosci */

		sf::VideoMode videoMode, desktopMode;

		for ( size_t i = 0; i < sf::VideoMode::GetModesCount(); i++ )
		{
			videoMode = sf::VideoMode::GetMode( i );
			desktopMode = sf::VideoMode::GetDesktopMode();
			if ( videoMode.BitsPerPixel == desktopMode.BitsPerPixel )
				resolution->AddOption( GenVideoMode( videoMode.Width, videoMode.Height, videoMode.BitsPerPixel ) );
		}


		/* ------------------------- */

		CTextArea * fullscreenTxt = options->CreateTextArea( "fullscreen-txt" );
		fullscreenTxt->SetFont(gGUI.GetFontSetting("FONT_MENU_LABEL"));
        fullscreenTxt->SetText( gLocalizator.GetText("OPT_FULLSCREEN"));
		fullscreenTxt->SetPosition( 19.0f, 23.0f, 30.0f, 5.0f );

		CCheckBox * fullscreen = options->CreateCheckBox( "fullscreen" );
		fullscreen->SetImage( "data/GUI/checkbox-true.png", "data/GUI/checkbox-false.png" );
		fullscreen->SetPosition( 51.0f, 23.0f, 3.25f, 4.0f );

		CTextArea * vsyncTxt = options->CreateTextArea( "vsync-txt" );
		vsyncTxt->SetFont(gGUI.GetFontSetting("FONT_MENU_LABEL"));
		vsyncTxt->SetText( gLocalizator.GetText("OPT_VSYNC") );
		vsyncTxt->SetPosition( 19.0f, 31.0f, 30.0f, 5.0f );

		CCheckBox * vsync = options->CreateCheckBox( "vsync" );
		vsync->SetImage( "data/GUI/checkbox-true.png", "data/GUI/checkbox-false.png" );
		vsync->SetPosition( 51.0f, 31.0f, 3.25f, 4.0f );

		CTextArea * soundTxt = options->CreateTextArea( "sound-txt" );
		soundTxt->SetFont(gGUI.GetFontSetting("FONT_MENU_LABEL"));
		soundTxt->SetText(  gLocalizator.GetText("OPT_SOUND_LEVEL") );
		soundTxt->SetPosition( 19.0f, 41.0f, 30.0f, 5.0f );

        CScrollBar *sound = options->CreateScrollBar( "sound-volume" );
        sound->SetHandleImage("data/GUI/scrollbar-handle.png");
        sound->SetBackgroundImage("data/GUI/scrollbar.png" );
        sound->SetPosition( 51.0f, 41.0f, 20.0f, 4.0f);
        sound->SetHandleSize(16.0f);

		CTextArea * musicTxt = options->CreateTextArea( "music-txt" );
		musicTxt->SetFont(gGUI.GetFontSetting("FONT_MENU_LABEL"));
		musicTxt->SetText(  gLocalizator.GetText("OPT_MUSIC_LEVEL") );
		musicTxt->SetPosition( 19.0f, 49.0f, 30.0f, 5.0f );

        CScrollBar * music = options->CreateScrollBar( "music-volume" );
        music->SetHandleImage("data/GUI/scrollbar-handle.png");
        music->SetBackgroundImage("data/GUI/scrollbar.png" );
        music->SetPosition( 51.0f, 50.0f, 20.0f, 4.0f);
        music->SetHandleSize(16.0f);
        
		CTextArea * stereoTxt = options->CreateTextArea( "3d-sound-txt" );
		stereoTxt->SetFont(gGUI.GetFontSetting("FONT_MENU_LABEL"));
        stereoTxt->SetText( gLocalizator.GetText("OPT_3D_SOUND"));
		stereoTxt->SetPosition( 19.0f, 57.0f, 30.0f, 5.0f );

		CCheckBox * stereo = options->CreateCheckBox( "3d-sound" );
		stereo->SetImage( "data/GUI/checkbox-true.png", "data/GUI/checkbox-false.png" );
		stereo->SetPosition( 51.0f, 57.0f, 3.25f, 4.0f );

        CTextArea * controlsTxt0 = options->CreateTextArea( "controls-txt0" );
        controlsTxt0->SetFont(gGUI.GetFontSetting("FONT_MENU_LABEL"));
        controlsTxt0->SetText(  gLocalizator.GetText("OPT_CONTROLS1") );
        controlsTxt0->SetPosition( 19.0f, 64.0f, 30.0f, 5.0f );

		CDropList * controls0 = options->CreateDropList( "controls0" );
		controls0->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
		controls0->SetImage( "data/GUI/droplist.png", "data/GUI/droplistd.png" );
		controls0->SetCenter( true );
		controls0->SetOptionFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
		controls0->SetOptionImage( "data/GUI/droplist-option.png", "data/GUI/droplist-optiond.png" );
		controls0->SetOptionCenter( true );
		controls0->SetPosition( 51.0f, 64.0f, 20.0f, 4.0f );
        size_t count = System::Input::CBindManager::GetBindManagersCount(0);
        for ( size_t i = 0 ; i < count ; i++ )
        {
            controls0->AddOption(
                System::Input::CBindManager::GetBindManagerAt( i,0 )->GetName()
            );

            CButton* opt = dynamic_cast<CButton*>(controls0->FindObject("controls0" + StringUtils::ToString(i)));
            if (opt != NULL)
            {
                CTextArea* tooltip = gGUI.CreateTextArea("controls0" + StringUtils::ToString(i) + "_tooltip", true, Z_GUI4);
                tooltip->SetPosition(0.f, 0.f, 400.f, 0.f, UNIT_PIXEL);
                tooltip->SetBackgroundImage("data/GUI/transparent-black.png");
                tooltip->SetVisible(false);
                //tooltip->SetCenter(true); // �eeee�oooo�eeee�oooo bug alert, nie centrowac
                tooltip->SetAutoHeight(true);
                tooltip->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
                tooltip->SetText(System::Input::CBindManager::GetBindManagerAt(i, 0)->GetControlsMenuDescription()
                        + L"\n" + System::Input::CBindManager::GetBindManagerAt(i, 0)->GetControlsDescription());
                tooltip->SetPadding(20.f, 20.f, 20.f, 20.f, UNIT_PIXEL);
                tooltip->SetOffset(sf::Vector2f(-400.f - opt->ConvertToGlobalPosition(opt->GetSize() / 100.f).x, 20.f));

                opt->SetTooltip(tooltip);
            }
        }

		CButton * binding0 = options->CreateButton( "binding-btn0" );
		binding0->SetPosition( 75.0f, 64.0f, 10.0f, 4.0f );
		binding0->SetCenter( true );
		binding0->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
        binding0->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
		binding0->SetText(gLocalizator.GetText("OPT_DEFINE"));
		binding0->GetClickParamCallback()->bind( this, &CMenuScreens::ShowBindingOptions );
		binding0->SetClickCallbackParams( L"binding-options0" );

		CTextArea * controlsTxt1 = options->CreateTextArea( "controls-txt1" );
        controlsTxt1->SetFont(gGUI.GetFontSetting("FONT_MENU_LABEL"));
        controlsTxt1->SetText(  gLocalizator.GetText("OPT_CONTROLS2") );
        controlsTxt1->SetPosition( 19.0f, 71.0f, 30.0f, 5.0f );

		CDropList * controls1 = options->CreateDropList( "controls1" );
        controls1->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
		controls1->SetImage( "data/GUI/droplist.png", "data/GUI/droplistd.png" );
		controls1->SetCenter( true );
		controls1->SetOptionFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
		controls1->SetOptionImage( "data/GUI/droplist-option.png", "data/GUI/droplist-optiond.png" );
		controls1->SetOptionCenter( true );
		controls1->SetPosition( 51.0f, 71.0f, 20.0f, 4.0f );
        count = System::Input::CBindManager::GetBindManagersCount(1);
        for ( size_t i = 0 ; i < count ; i++ )
        {
            controls1->AddOption(
                System::Input::CBindManager::GetBindManagerAt( i,1 )->GetName()
            );

            CButton* opt = dynamic_cast<CButton*>(controls1->FindObject("controls1" + StringUtils::ToString(i)));
            if (opt != NULL)
            {
                CTextArea* tooltip = gGUI.CreateTextArea("controls1" + StringUtils::ToString(i) + "_tooltip", true, Z_GUI4);
                tooltip->SetPosition(0.f, 0.f, 400.f, 0.f, UNIT_PIXEL);
                tooltip->SetBackgroundImage("data/GUI/transparent-black.png");
                tooltip->SetVisible(false);
                //tooltip->SetCenter(true); // �eeee�oooo�eeee�oooo bug alert, nie centrowac
                tooltip->SetAutoHeight(true);
                tooltip->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
                tooltip->SetText(System::Input::CBindManager::GetBindManagerAt(i, 1)->GetControlsMenuDescription()
                        + L"\n" + System::Input::CBindManager::GetBindManagerAt(i, 1)->GetControlsDescription());
                tooltip->SetPadding(20.f, 20.f, 20.f, 20.f, UNIT_PIXEL);
                tooltip->SetOffset(sf::Vector2f(-400.f - opt->ConvertToGlobalPosition(opt->GetSize() / 100.f).x, 20.f));

                opt->SetTooltip(tooltip);
            }
        }

		CButton * binding1 = options->CreateButton( "binding-btn1" );
		binding1->SetPosition( 75.0f, 71.0f, 10.0f, 4.0f );
		binding1->SetCenter( true );
		binding1->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
        binding1->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
		binding1->SetText(gLocalizator.GetText("OPT_DEFINE"));
		binding1->GetClickParamCallback()->bind( this, &CMenuScreens::ShowBindingOptions );
		binding1->SetClickCallbackParams( L"binding-options1" );

		CButton * saveOptions = options->CreateButton( "save-options-button" );
		saveOptions->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		saveOptions->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
		saveOptions->SetText( gLocalizator.GetText("OPT_SAVE") );
		saveOptions->SetPosition( 19.0f, 80.0f, 30.0f, 6.0f );
		saveOptions->SetCenter( true );
		saveOptions->GetClickCallback()->bind( this, &CMenuScreens::SaveOptions );

		CButton * exitOptions = options->CreateButton( "exit-options-button" );
		exitOptions->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		exitOptions->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
		exitOptions->SetText( gLocalizator.GetText("OPT_CANCEL") );
		exitOptions->SetPosition( 51.0f, 80.0f, 30.0f, 6.0f );
		exitOptions->SetCenter( true );
		exitOptions->GetClickCallback()->bind( this, &CMenuScreens::CancelOptions );

		mOptions = options;

        UpdateOptions();
        StoreOptions();
	}
}

void CMenuScreens::UpdateOptions()
{
	if ( mOptions )
	{
		CDropList * resolution = (CDropList*) mOptions->FindObject( "resolution" );
		resolution->SetSelectedOption( GenVideoMode( gGameOptions.GetWidth(), gGameOptions.GetHeight(), gGameOptions.GetBitsPerPixel() ) );

		CCheckBox * fullscreen = (CCheckBox*) mOptions->FindObject( "fullscreen" );
		fullscreen->SetState( gGameOptions.IsFullscreen() );

		CCheckBox * vsync = (CCheckBox*) mOptions->FindObject( "vsync" );
		vsync->SetState( gGameOptions.IsVSync() );

        CScrollBar * sound = (CScrollBar*) mOptions->FindObject( "sound-volume");
        sound->SetState( gGameOptions.GetSoundVolume());

        CScrollBar * music = (CScrollBar*) mOptions->FindObject( "music-volume");
        music->SetState( gGameOptions.GetMusicVolume());

        CCheckBox * stereo = (CCheckBox*) mOptions->FindObject( "3d-sound" );
        stereo->SetState( gGameOptions.Get3DSound() );

        CDropList * controls0 = (CDropList*) mOptions->FindObject( "controls0" );
        controls0->SetSelectedOption( gBindManager0->GetName() );

		CDropList * controls1 = (CDropList*) mOptions->FindObject( "controls1" );
        controls1->SetSelectedOption( gBindManager1->GetName() );
	}
}

void CMenuScreens::ShowBindingOptions(const std::wstring & menu)
{
    StoreOptions();
    Show(menu);
}

// przywroc tymczasowe opcje, jeszcze nie zapisane, ale juz ruszane przez uzytkownika
void CMenuScreens::RestoreOptions()
{
    if (!mOptions)
        return;

//    CDropList * resolution = (CDropList*) mOptions->FindObject( "resolution" );
//    resolution->SetSelectedOption( GenVideoMode( tmpWidth, tmpHeight, tmpBitsPerPixel ) );

    CCheckBox * fullscreen = (CCheckBox*) mOptions->FindObject( "fullscreen" );
    fullscreen->SetState( tmpIsFullscreen );

    CCheckBox * vsync = (CCheckBox*) mOptions->FindObject( "vsync" );
    vsync->SetState( tmpIsVSync );

    CScrollBar * sound = (CScrollBar*) mOptions->FindObject( "sound-volume");
    sound->SetState( tmpSoundVolume );

    CScrollBar * music = (CScrollBar*) mOptions->FindObject( "music-volume");
    music->SetState( tmpMusicVolume);

    CCheckBox * stereo = (CCheckBox*) mOptions->FindObject( "3d-sound" );
    stereo->SetState( tmp3DSound );

    CDropList * controls0 = (CDropList*) mOptions->FindObject( "controls0" );
    controls0->SetSelectedOption( tmpControls0 );

    CDropList * controls1 = (CDropList*) mOptions->FindObject( "controls1" );
    controls1->SetSelectedOption( tmpControls1 );

}

void CMenuScreens::StoreOptions()
{
	if ( !mOptions )
        return;

//    CDropList * resolution = (CDropList*) mOptions->FindObject( "resolution" );

    CCheckBox * fullscreen = (CCheckBox*) mOptions->FindObject( "fullscreen" );
    tmpIsFullscreen = fullscreen->GetState();

    CCheckBox * vsync = (CCheckBox*) mOptions->FindObject( "vsync" );
    tmpIsVSync = vsync->GetState();

    CScrollBar * sound = (CScrollBar*) mOptions->FindObject( "sound-volume");
    tmpSoundVolume = sound->GetState();

    CScrollBar * music = (CScrollBar*) mOptions->FindObject( "music-volume");
    tmpMusicVolume = music->GetState();

    CCheckBox * stereo = (CCheckBox*) mOptions->FindObject( "3d-sound" );
    tmp3DSound = stereo->GetState();

    CDropList * controls0 = (CDropList*) mOptions->FindObject( "controls0" );
    tmpControls0 = controls0->GetSelectedOption();

    CDropList * controls1 = (CDropList*) mOptions->FindObject( "controls1" );
    tmpControls1 = controls1->GetSelectedOption();
    
    for (unsigned int i = 0; i < 2; ++i)
    {
        if (!mBindingOptions[i])
            InitBindingOptions();

        CCheckBox* absolute = dynamic_cast<CCheckBox*>(mBindingOptions[i]->FindObject("absolute"));
        CCheckBox* mouseLook = dynamic_cast<CCheckBox*>(mBindingOptions[i]->FindObject("mouse-look"));
        CCheckBox* mouseCast = dynamic_cast<CCheckBox*>(mBindingOptions[i]->FindObject("mouse-cast"));
        CCheckBox* separateSeq = dynamic_cast<CCheckBox*>(mBindingOptions[i]->FindObject("separate-seq"));
        CCheckBox* pncMovement = dynamic_cast<CCheckBox*>(mBindingOptions[i]->FindObject("pnc-movement"));

        absolute->SetState(System::Input::CBindManager::GetBindManager((i == 0 ? tmpControls0 : tmpControls1))->GetIsAbsolute());
        mouseLook->SetState(System::Input::CBindManager::GetBindManager((i == 0 ? tmpControls0 : tmpControls1))->GetMouseLook());
        mouseCast->SetState(System::Input::CBindManager::GetBindManager((i == 0 ? tmpControls0 : tmpControls1))->GetMouseCaster() != NULL);
        pncMovement->SetState(System::Input::CBindManager::GetBindManager((i == 0 ? tmpControls0 : tmpControls1))->GetPointNClickMove());
        
        for (size_t b = 0; b < System::Input::CBindManager::GetBindManagersCount(i); ++b)
            if (System::Input::CBindManager::GetBindManagerAt(b, i)->GetName() == (i == 0 ? tmpControls0 : tmpControls1))
                separateSeq->SetState(!!(CGameOptions::cfSeparateSeq & gGameOptions.GetDefControlSchemes(i)[b].flags));
    }
}

#include "../Console/CConsole.h"

void CMenuScreens::SaveOptions()
{
	if ( mOptions )
	{
		CDropList * resolution = (CDropList*) mOptions->FindObject( "resolution" );
		CCheckBox * fullscreen = (CCheckBox*) mOptions->FindObject( "fullscreen" );
		CCheckBox * vsync = (CCheckBox*) mOptions->FindObject( "vsync" );
        CScrollBar * music = (CScrollBar*) mOptions->FindObject( "music-volume");
        CScrollBar * sound = (CScrollBar*) mOptions->FindObject( "sound-volume");
        CCheckBox * stereo = (CCheckBox*) mOptions->FindObject( "3d-sound" );
		CDropList * controls0 = (CDropList*) mOptions->FindObject( "controls0" );
		CDropList * controls1 = (CDropList*) mOptions->FindObject( "controls1" );


        gGameOptions.SetMusicVolume( music->GetState() );
        gGameOptions.SetSoundVolume( sound->GetState() );
        gGameOptions.Set3DSound( stereo->GetState() );

        // ify, zeby sie nie zrobilo kuku first_game'owi, jesli nie trzeba
        if (StringUtils::ConvertToString(controls0->GetSelectedOption()) != gGameOptions.GetControls(0))
		    gGameOptions.SetControls( StringUtils::ConvertToString( controls0->GetSelectedOption() ), 0 );
        if (StringUtils::ConvertToString(controls1->GetSelectedOption()) != gGameOptions.GetControls(1))
		    gGameOptions.SetControls( StringUtils::ConvertToString( controls1->GetSelectedOption() ), 1 );

		sf::VideoMode videoMode;
		for ( size_t i = 0; i < sf::VideoMode::GetModesCount(); i++ ) {
			videoMode = sf::VideoMode::GetMode( i );
			if ( GenVideoMode( videoMode.Width, videoMode.Height, videoMode.BitsPerPixel ) == resolution->GetSelectedOption() )
				break;
		}

        unsigned int w = gGameOptions.GetWidth();
        unsigned int h = gGameOptions.GetHeight();
        unsigned int bpp = gGameOptions.GetBitsPerPixel();
		bool		 fs = gGameOptions.IsFullscreen();

        if (videoMode.Width == 0) videoMode.Width = w;
        if (videoMode.Height == 0) videoMode.Height = h;
        if (videoMode.BitsPerPixel == 0) videoMode.BitsPerPixel = bpp;

  		gGameOptions.SetWidth( videoMode.Width );
   		gGameOptions.SetHeight( videoMode.Height );
   		gGameOptions.SetBitsPerPixel( videoMode.BitsPerPixel );
   		gGameOptions.SetFullscreen( fullscreen->GetState() );
   		gGameOptions.SetVSync( vsync->GetState() );
		gGameOptions.SaveOptions();

        if ( w != videoMode.Width || h != videoMode.Height || bpp != videoMode.BitsPerPixel || fs != fullscreen->GetState() ) {
  		    gGameOptions.UpdateWindow();
        }

        StoreOptions();
        ShowPrevious();
        if (enteredOptionsFromChooseControlsScheme) {
            enteredOptionsFromChooseControlsScheme = false;
            ShowPrevious();
        }
	}
}

void CMenuScreens::CancelOptions()
{
	UpdateOptions();
    enteredOptionsFromChooseControlsScheme = false;
    ShowPrevious();
}

void CMenuScreens::InitBindingOptions()
{
	for ( int i = 0; i < PLAYERS_CNT; i++ )
	{
		if ( !mBindingOptions[i] )
		{
			CWindow* w = gGUI.CreateWindow( std::string("binding-options") + StringUtils::ToString(i) );
			w->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
			w->SetBackgroundImage( "data/GUI/bg-options.jpg" );

			CButton * saveOptions = w->CreateButton( "save-options-button" );
			saveOptions->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
            saveOptions->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
            saveOptions->SetText(gLocalizator.GetText("OPT_SAVE"));
			saveOptions->SetPosition( 19.0f, 80.0f, 30.0f, 6.0f );
			saveOptions->SetCenter( true );
			saveOptions->GetClickParamCallback()->bind( this, &CMenuScreens::SaveBindingOptions );
			saveOptions->SetClickCallbackParams( StringUtils::ToWString(i) );

			CButton * exitOptions = w->CreateButton( "exit-options-button" );
			exitOptions->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
            exitOptions->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
			exitOptions->SetText(gLocalizator.GetText("OPT_CANCEL"));
			exitOptions->SetPosition( 51.0f, 80.0f, 30.0f, 6.0f );
			exitOptions->SetCenter( true );
			exitOptions->GetClickCallback()->bind( this, &CMenuScreens::ShowPrevious );

            // checkboxy
            CCheckBox* absolute = w->CreateCheckBox("absolute");
		    absolute->SetImage("data/GUI/checkbox-true.png", "data/GUI/checkbox-false.png");
            absolute->SetPosition(10.f, 13.f, 4.f, 5.f);
            absolute->GetEventIntCallback(MOUSE_RELEASED_LEFT)->bind(this, &CMenuScreens::UpdateBindingOptions);
            absolute->SetEventIntParam(MOUSE_RELEASED_LEFT, i);

            CTextArea* absoluteTxt = w->CreateTextArea("absolute-txt");
            absoluteTxt->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
            absoluteTxt->SetText(gLocalizator.GetText("CTRL_BIND_ABS"));
            absoluteTxt->SetPosition(15.f, 14.f, 11.f, 4.f);

            CTextArea* absoluteTooltip = gGUI.CreateTextArea("binding" + StringUtils::ToString(i) + "-absolute_tooltip", true, Z_GUI4);
            absoluteTooltip->SetPosition(0.f, 0.f, 400.f, 0.f, UNIT_PIXEL);
            absoluteTooltip->SetBackgroundImage("data/GUI/transparent-black.png");
            absoluteTooltip->SetVisible(false);
            absoluteTooltip->SetAutoHeight(true);
            absoluteTooltip->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            absoluteTooltip->SetText(gLocalizator.GetText("CTRL_BIND_ABS_TOOLTIP"));
            absoluteTooltip->SetPadding(20.f, 20.f, 20.f, 20.f, UNIT_PIXEL);
            absoluteTooltip->SetOffset(sf::Vector2f(absolute->ConvertToGlobalPosition(absolute->GetSize() / 100.f).x, 20.f));

            absolute->SetTooltip(absoluteTooltip);

            CCheckBox* mouseLook = w->CreateCheckBox("mouse-look");
            mouseLook->SetImage("data/GUI/checkbox-true.png", "data/GUI/checkbox-false.png");
            mouseLook->SetPosition(26.f, 13.f, 4.f, 5.f);
            mouseLook->GetEventIntCallback(MOUSE_RELEASED_LEFT)->bind(this, &CMenuScreens::UpdateBindingOptions);
            mouseLook->SetEventIntParam(MOUSE_RELEASED_LEFT, i);

            CTextArea* mouseLookTxt = w->CreateTextArea("mouse-look-txt");
            mouseLookTxt->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
            mouseLookTxt->SetText(gLocalizator.GetText("CTRL_BIND_MOUSE_LOOK"));
            mouseLookTxt->SetPosition(31.f, 14.f, 11.f, 4.f);

            CTextArea* mouseLookTooltip = gGUI.CreateTextArea("binding" + StringUtils::ToString(i) + "-mouse-look_tooltip", true, Z_GUI4);
            mouseLookTooltip->SetPosition(0.f, 0.f, 400.f, 0.f, UNIT_PIXEL);
            mouseLookTooltip->SetBackgroundImage("data/GUI/transparent-black.png");
            mouseLookTooltip->SetVisible(false);
            mouseLookTooltip->SetAutoHeight(true);
            mouseLookTooltip->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            mouseLookTooltip->SetText(gLocalizator.GetText("CTRL_BIND_MOUSE_LOOK_TOOLTIP"));
            mouseLookTooltip->SetPadding(20.f, 20.f, 20.f, 20.f, UNIT_PIXEL);
            mouseLookTooltip->SetOffset(sf::Vector2f(mouseLook->ConvertToGlobalPosition(mouseLook->GetSize() / 100.f).x, 20.f));

            mouseLook->SetTooltip(mouseLookTooltip);

            CCheckBox* mouseCast = w->CreateCheckBox("mouse-cast");
            mouseCast->SetImage("data/GUI/checkbox-true.png", "data/GUI/checkbox-false.png");
            mouseCast->SetPosition(42.f, 13.f, 4.f, 5.f);
            mouseCast->GetEventIntCallback(MOUSE_RELEASED_LEFT)->bind(this, &CMenuScreens::UpdateBindingOptions);
            mouseCast->SetEventIntParam(MOUSE_RELEASED_LEFT, i);

            CTextArea* mouseCastTxt = w->CreateTextArea("mouse-cast-txt");
            mouseCastTxt->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
            mouseCastTxt->SetText(gLocalizator.GetText("CTRL_BIND_MOUSECAST"));
            mouseCastTxt->SetPosition(47.f, 14.f, 11.f, 4.f);

            CTextArea* mouseCastTooltip = gGUI.CreateTextArea("binding" + StringUtils::ToString(i) + "-mouse-cast_tooltip", true, Z_GUI4);
            mouseCastTooltip->SetPosition(0.f, 0.f, 400.f, 0.f, UNIT_PIXEL);
            mouseCastTooltip->SetBackgroundImage("data/GUI/transparent-black.png");
            mouseCastTooltip->SetVisible(false);
            mouseCastTooltip->SetAutoHeight(true);
            mouseCastTooltip->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            mouseCastTooltip->SetText(gLocalizator.GetText("CTRL_BIND_MOUSECAST_TOOLTIP"));
            mouseCastTooltip->SetPadding(20.f, 20.f, 20.f, 20.f, UNIT_PIXEL);
            mouseCastTooltip->SetOffset(sf::Vector2f(mouseCast->ConvertToGlobalPosition(mouseCast->GetSize() / 100.f).x, 20.f));

            mouseCast->SetTooltip(mouseCastTooltip);

            CCheckBox* separateSeq = w->CreateCheckBox("separate-seq");
            separateSeq->SetImage("data/GUI/checkbox-true.png", "data/GUI/checkbox-false.png");
            separateSeq->SetPosition(58.f, 13.f, 4.f, 5.f);
            separateSeq->GetEventIntCallback(MOUSE_RELEASED_LEFT)->bind(this, &CMenuScreens::UpdateBindingOptions);
            separateSeq->SetEventIntParam(MOUSE_RELEASED_LEFT, i);

            CTextArea* separateSeqTxt = w->CreateTextArea("separate-seq-txt");
            separateSeqTxt->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
            separateSeqTxt->SetText(gLocalizator.GetText("CTRL_BIND_SEPARATE_SEQUENCES"));
            separateSeqTxt->SetPosition(63.f, 14.f, 11.f, 4.f);

            CTextArea* separateSeqTooltip = gGUI.CreateTextArea("binding" + StringUtils::ToString(i) + "-separate-seq_tooltip", true, Z_GUI4);
            separateSeqTooltip->SetPosition(0.f, 0.f, 400.f, 0.f, UNIT_PIXEL);
            separateSeqTooltip->SetBackgroundImage("data/GUI/transparent-black.png");
            separateSeqTooltip->SetVisible(false);
            separateSeqTooltip->SetAutoHeight(true);
            separateSeqTooltip->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            separateSeqTooltip->SetText(gLocalizator.GetText("CTRL_BIND_SEPARATE_SEQ_TOOLTIP"));
            separateSeqTooltip->SetPadding(20.f, 20.f, 20.f, 20.f, UNIT_PIXEL);
            separateSeqTooltip->SetOffset(sf::Vector2f(separateSeq->ConvertToGlobalPosition(separateSeq->GetSize() / 100.f).x, 20.f));

            separateSeq->SetTooltip(separateSeqTooltip);

            CCheckBox* pointNClickMovement = w->CreateCheckBox("pnc-movement");
            pointNClickMovement->SetImage("data/GUI/checkbox-true.png", "data/GUI/checkbox-false.png");
            pointNClickMovement->SetPosition(75.f, 13.f, 4.f, 5.f);
            pointNClickMovement->GetEventIntCallback(MOUSE_RELEASED_LEFT)->bind(this, &CMenuScreens::UpdateBindingOptions);
            pointNClickMovement->SetEventIntParam(MOUSE_RELEASED_LEFT, i);

            CTextArea* pointNClickMovementTxt = w->CreateTextArea("pnc-movement-txt");
            pointNClickMovementTxt->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL"));
            pointNClickMovementTxt->SetText(gLocalizator.GetText("CTRL_BIND_POINTNCLICK_MOVEMENT"));
            pointNClickMovementTxt->SetPosition(80.f, 14.f, 11.f, 4.f);

            CTextArea* pointNClickMovementTooltip = gGUI.CreateTextArea("binding" + StringUtils::ToString(i) + "-pnc-movement_tooltip", true, Z_GUI4);
            pointNClickMovementTooltip->SetPosition(0.f, 0.f, 400.f, 0.f, UNIT_PIXEL);
            pointNClickMovementTooltip->SetBackgroundImage("data/GUI/transparent-black.png");
            pointNClickMovementTooltip->SetVisible(false);
            pointNClickMovementTooltip->SetAutoHeight(true);
            pointNClickMovementTooltip->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            pointNClickMovementTooltip->SetText(gLocalizator.GetText("CTRL_BIND_POINTNCLICK_MOVEMENT_TOOLTIP"));
            pointNClickMovementTooltip->SetPadding(20.f, 20.f, 20.f, 20.f, UNIT_PIXEL);
            pointNClickMovementTooltip->SetOffset(sf::Vector2f(pointNClickMovement->ConvertToGlobalPosition(pointNClickMovement->GetSize() / 100.f).x, 20.f));

            pointNClickMovement->SetTooltip(pointNClickMovementTooltip);

			mBindingOptions[i] = w;
		}
	}
}

void CMenuScreens::UpdateBindingOptions(int playerNumber)
{
    RestoreOptions();

	std::string playerStr = StringUtils::ToString( playerNumber );

	if ( mBindingOptions[playerNumber] != NULL )
	{
		/* Wyczysc plansze */

        // zapisz tu poprzednie ustawienia, zeby nie czyscilo po zmianie checkboxow
        std::map<std::string, int> prevBindings;

		for ( unsigned i = 0; i < mKeyBindings[playerNumber].size(); i++ )
		{
            prevBindings.insert(std::pair<std::string, int>(mKeyBindings[playerNumber][i].name, mKeyBindings[playerNumber][i].keyReader->GetKey()));

			mKeyBindings[playerNumber][i].textArea->Remove();
			mKeyBindings[playerNumber][i].keyReader->Remove();
		}

		mKeyBindings[playerNumber].clear();
        
        /* checkboxy */
        CCheckBox* absolute = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("absolute"));
        CCheckBox* mouseLook = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("mouse-look"));
        CCheckBox* mouseCast = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("mouse-cast"));
        CCheckBox* separateSeq = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("separate-seq"));
        CCheckBox* pncMovement = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("pnc-movement"));
        
        // ktore bindingi pokazywac?
        // te uniwersalne
        unsigned int showMask = System::Input::CBindManager::agAll;
       
        // mousecast
        if (mouseCast)
        {
            if (mouseCast->GetState())
                showMask |= System::Input::CBindManager::agMousecast;
            else
            {
                if (separateSeq->GetState())
                    showMask |= System::Input::CBindManager::agSeparateSeq;
                else
                    showMask |= System::Input::CBindManager::agNormalCast;
            }
        }

        // sterowanie - absolutne czy zwykle? a moze point'n'click?
        if (absolute)
        {
            if (absolute->GetState())
                showMask |= System::Input::CBindManager::agAbsolute;
            else if (pncMovement && pncMovement->GetState())
            {
                showMask |= System::Input::CBindManager::agPointNClick;
                showMask &= ~System::Input::CBindManager::agNormalCast; // uzywamy tylko abi0
            }
            else
            {
                showMask |= System::Input::CBindManager::agNormal;
                
                // obracanie - mysz, czy klawisze?
                if (mouseLook && !mouseLook->GetState())
                    showMask |= System::Input::CBindManager::agKeyTurning;
            }

        }

		/* Dodaj bindingi zgodne z aktualnym trybem sterowania */

		CDropList* controls = (CDropList*) mOptions->FindObject( std::string("controls") + playerStr );

		if ( controls != NULL )
		{
			System::Input::CBindManager* bm = System::Input::CBindManager::GetBindManager( controls->GetSelectedOption(), playerNumber );

			if ( bm != NULL )
			{
				const std::map<std::string,int>& keys = bm->GetKeyBindings();
				std::map<std::string,int>::const_iterator it;

                const System::Input::CBindManager::ActionPair* actions = System::Input::CBindManager::availableActions;

				unsigned i, j;

				//for ( i=0, it=keys.begin(); it!=keys.end(); ++it )
                for (i = 0, j = 0; j < System::Input::CBindManager::availableActionsCount; ++j)
				{
					keyBinding kb;

                    // do jakiej grupy nalezy akcja?
                    if (!(actions[j].second & showMask))
                        continue;

					kb.name = actions[j].first;

					kb.textArea = mBindingOptions[playerNumber]->CreateTextArea( "binding-txt" );
					kb.textArea->SetFont(gGUI.GetFontSetting("FONT_MENU_LABEL"));
                    kb.textArea->SetText( KeyStrings::GetBindDesc(actions[j].first) );
					kb.textArea->SetPosition( 10.0f + (i%2) * 40.f, 5.0f*((i/2)+5), 25.0f, 4.0f );
                    kb.textArea->UpdateText();

					kb.keyReader = mBindingOptions[playerNumber]->CreateKeyReader( "key-reader" );
					kb.keyReader->SetFont(gGUI.GetFontSetting("FONT_DEFAULT_CONTROL", 1.2f));
					kb.keyReader->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
                    kb.keyReader->UpdateSprites(0.f);   // zapobiega mruganiu napisow w rogu

                    // najpierw zmienione przed chwila
                    if (prevBindings.find(kb.name) != prevBindings.end())
                        kb.keyReader->SetKey(prevBindings[kb.name]);
                    // pozniej te z wybranego bindManagera
                    else if ((it = keys.find(actions[j].first)) != keys.end())
					    kb.keyReader->SetKey( it->second );

					kb.keyReader->SetCenter( true );
					kb.keyReader->SetPosition( 35.0f + (i%2) * 40.f, 5.0f*((i/2)+5), 10.0f, 4.0f );
                    // dex: teraz beda cuda
                    kb.keyReader->GetClickParamCallback()->bind( &mPrevMenus, &std::stack<std::wstring>::push );
                    kb.keyReader->SetClickCallbackParams( StringUtils::ConvertToWString("binding-options" + playerStr) );
                    kb.keyReader->GetClickCallback()->bind( &mPrevMenus, &std::stack<std::wstring>::pop );
                    // koniec cudow

					mKeyBindings[playerNumber].push_back( kb );

                    // dodalismy jeden binding, wiec ++
                    ++i;
				}
			}
		}
	}
}

void CMenuScreens::SaveBindingOptions(const std::wstring& playerWStr)
{
	unsigned playerNumber = StringUtils::Parse<unsigned>( playerWStr );
	std::string playerStr = StringUtils::ConvertToString( playerWStr );

	if ( mBindingOptions[playerNumber] != NULL )
	{
        const std::wstring& name = gLocalizator.GetText("CTRL_SET_USER");
        System::Input::CBindManager::SetActualBindManager(name, playerNumber);
        if (playerNumber == 0)
            tmpControls0 = name;
        else
            tmpControls1 = name;

        // edytowalne tylko user-defined
		std::string controlsStr = StringUtils::ConvertToString(name);

        // no more mousecaster!
        System::Input::CBindManager* bindMgr = System::Input::CBindManager::GetBindManager(name, playerNumber);
        if (!bindMgr)
        {
            fprintf(stderr, "ERROR: bind manger not found: %s (CMenuScreens::SaveBindingOptions)\n", controlsStr.c_str());
        }

        CMouseCaster* mouseCaster = bindMgr->GetMouseCaster();
        if (mouseCaster != NULL)
        {
            delete mouseCaster;
            bindMgr->SetMouseCaster(NULL);
        }

        // ..or maybe yes?
        CCheckBox* mouseCast = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("mouse-cast"));
        if (mouseCast && mouseCast->GetState())
        {
            std::vector<std::wstring> abiKeys;
	        abiKeys.push_back(L"Q");
	        abiKeys.push_back(L"W");
	        abiKeys.push_back(L"E");
	        abiKeys.push_back(L"R");

	        CMouseCaster* mc = new CMouseCaster();
	        mc->Initialize( abiKeys, 80.0f );
	        mc->SetKeyImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png" );
            const GUI::FontSetting & fs = gGUI.GetFontSetting("FONT_DEFAULT"); //bylo 16.f na 14.f
	        mc->SetKeyFont(fs.name, fs.size, fs.unit);
	        mc->SetKeyColor( sf::Color::White );
	        mc->SetKeySize( sf::Vector2f(60.0f,60.0f) );

	        bindMgr->SetMouseCaster( mc );
        }

        // zapisywanie reszty ustawien
        CCheckBox* absolute = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("absolute"));
        CCheckBox* mouseLook = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("mouse-look"));
        CCheckBox* separateSeq = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("separate-seq"));
        CCheckBox* pncMovement = dynamic_cast<CCheckBox*>(mBindingOptions[playerNumber]->FindObject("pnc-movement"));

        bindMgr->SetIsAbsolute(absolute && absolute->GetState());
        bindMgr->SetMouseLook(mouseLook && mouseLook->GetState());
        bindMgr->SetPointNClickMove(pncMovement && pncMovement->GetState());

        // trzeba wrzucic do gameOptions, zeby sie zapisalo
        int flags = 0;
        if (absolute && absolute->GetState())
            flags |= CGameOptions::cfAbsolute;
        if (mouseLook && mouseLook->GetState())
            flags |= CGameOptions::cfMouseLook;
        if (mouseCast && mouseCast->GetState())
            flags |= CGameOptions::cfMousecast;
        if (separateSeq && separateSeq->GetState())
            flags |= CGameOptions::cfSeparateSeq;
        if (pncMovement && pncMovement->GetState())
            flags |= CGameOptions::cfPointNClick;

        gGameOptions.SetUserControlsFlags(playerNumber, flags);

		for ( unsigned i = 0; i < mKeyBindings[playerNumber].size(); i++ )
		    gGameOptions.SetKeyBinding(	controlsStr,
									    mKeyBindings[playerNumber][i].name,
									    mKeyBindings[playerNumber][i].keyReader->GetKey(),
									    playerNumber );
	}

	ShowPrevious();
}

void CMenuScreens::InitReadmeScreen()
{
    if (!mReadmeScreen)
    {
        CWindow* readme = gGUI.CreateWindow("readme-screen");
        readme->SetPosition(0.f, 0.f, 100.f, 100.f);
		readme->SetBackgroundImage("data/GUI/bg-options.jpg");

        CScrollBar* scroll = readme->CreateScrollBar("readme-scroll");
        scroll->SetPosition(90.f, 10.f, 2.f, 60.f);
	    scroll->SetBackgroundImage( "data/GUI/scrollbar-v.png" );
	    scroll->SetHandleImage( "data/GUI/scrollbar-handle.png" );
	    scroll->SetOrientation( ORIENTATION_Y );
	    scroll->SetHandleSize( 5.0f );

        CScrollPanel* panel = readme->CreateScrollPanel("readme-panel");
        panel->SetPosition(10.f, 10.f, 80.f, 60.f);
        panel->SetScrollBarY(scroll);

        CTextArea* text = panel->CreateTextArea("readme-panel-text");
        text->SetPosition(0.f, 0.f, 100.f, 0.f);
        text->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
        text->SetText(gLocalizator.GetText("README_TEXT"));
        text->SetAutoHeight(true);

        CButton* retButton = readme->CreateButton("readme-return-button");
        retButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        retButton->SetText(gLocalizator.GetText("MENU_RETURN"));
		retButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		retButton->SetPosition( 30.0f, 80.0f, 40.0f, 10.0f );
		retButton->SetCenter( true );
		retButton->GetClickCallback()->bind( this, &CMenuScreens::ShowPrevious );

        mReadmeScreen = readme;
    }
}

void CMenuScreens::InitLoadingScreen(){
	if(!mLoadingScreen){
		CWindow *loading = gGUI.CreateWindow("loading-screen");
		loading->SetPosition(0.f, 0.f, 100.f, 100.f);
		loading->SetBackgroundImage("data/GUI/loading-screen.jpg");

		mLoadingScreen = loading;
	}
}

bool CMenuScreens::HideLoadingScreen(bool fade){
    if (!fade)
        mLoadingScreen->SetVisible(false);
    if (!mLoadingScreen->IsVisible())
        return false;

    mLoadingScreen->SetOffset(sf::Vector2f(gGameOptions.GetWidth() - (gGameOptions.GetWidth() - mLoadingScreen->GetOffset().x) * 0.95f + 1.f, 0.f));
    if (mLoadingScreen->GetOffset().x >= gGameOptions.GetWidth())
    {
        mLoadingScreen->SetOffset(sf::Vector2f(0.f, 0.f));
	    mLoadingScreen->SetVisible(false);
    }

    return mLoadingScreen->IsVisible();
}


void CMenuScreens::InitAchievementsScreen()
{
    if (!mAchievementsScreen)
    {
        CWindow* w = gGUI.CreateWindow("achievements-screen");
		w->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
		w->SetBackgroundImage( "data/GUI/bg-options.jpg" );

        CScrollBar* scroll = w->CreateScrollBar("achievements-scroll");
        scroll->SetPosition(90.f, 10.f, 2.f, 60.f);
	    scroll->SetBackgroundImage( "data/GUI/scrollbar-v.png" );
	    scroll->SetHandleImage( "data/GUI/scrollbar-handle.png" );
	    scroll->SetOrientation( ORIENTATION_Y );
	    scroll->SetHandleSize( 5.0f );

        CScrollPanel* panel = w->CreateScrollPanel("achievements-panel");
        panel->SetPosition(10.f, 10.f, 80.f, 60.f);
        panel->SetPadding(0.f, 0.f, 0.f, 0.f);
        panel->SetScrollBarY(scroll);

        CButton* retButton = w->CreateButton("achievements-return-button");
        retButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        retButton->SetText(gLocalizator.GetText("MENU_RETURN"));
		retButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		retButton->SetPosition( 30.0f, 80.0f, 40.0f, 10.0f );
		retButton->SetCenter( true );
		retButton->GetClickCallback()->bind( this, &CMenuScreens::ShowPrevious );

        mAchievementsScreen = w;
    }
}

void CMenuScreens::UpdateAchievementsScreen()
{
    if (!mAchievementsScreen)
        InitAchievementsScreen();

    CScrollPanel* panel = dynamic_cast<CScrollPanel*>(mAchievementsScreen->FindObject("achievements-panel"));
    if (!panel)
        return;

    // czyszczenie achievementow
    CGUIObject* obj = panel->FindObject("achievement-0");
    for (int i = 0; obj != NULL; ++i, obj = panel->FindObject("achievement-" + StringUtils::ToString(i)))
    {
        // tooltipy tez wywal, a co!
        if (obj->GetTooltip())
            obj->GetTooltip()->Remove();

        obj->Remove();
    }

    // rozmiar obrazka i przerwa miedzy nimi
    float size = 128.f, gap = 10.f;

    // ile obrazkow w rzedzie?
    unsigned width = (unsigned)(gGUI.ConvertToGlobalPosition(panel->GetSize()).x / (size + gap));

    const std::vector<CAchievementManager::SAchievement>& achievs = gAchievementManager.GetAchievements();
    for (size_t i = 0; i < achievs.size(); ++i)
    {
        CImageBox* img = panel->CreateImageBox("achievement-" + StringUtils::ToString(i));
        img->SetPosition((float)(i % width) * (size + gap), (float)(unsigned)(i / width) * (size + gap), size, size, UNIT_PIXEL);
        img->AddImageToSequence(achievs[i].completed ? achievs[i].image : "data/GUI/achievements/default.png"); // defaultowy obrazek
        img->SetSequenceState(0);

        CTextArea* tooltip = gGUI.CreateTextArea("achievement-" + StringUtils::ToString(i) + "_tooltip", true, Z_GUI4);
        tooltip->SetPosition(0.f, 0.f, 400.f, 0.f, UNIT_PIXEL);
        tooltip->SetBackgroundImage("data/GUI/transparent-black.png");
        tooltip->SetVisible(false);
        tooltip->SetAutoHeight(true);
        tooltip->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
        tooltip->SetPadding(20.f, 20.f, 20.f, 20.f, UNIT_PIXEL);
        tooltip->SetOffset(sf::Vector2f(size, 20.f));

        std::wstring text = gLocalizator.Localize(achievs[i].name);
        if (achievs[i].completed)
            text += L"\n\n" + gLocalizator.Localize(achievs[i].desc);
        tooltip->SetText(text);

        img->SetTooltip(tooltip);
    }
}

void CMenuScreens::InitBestiaryScreen()
{
    if (!mBestiaryScreen)
    {
        CWindow* w = gGUI.CreateWindow("bestiary-screen");
		w->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
		w->SetBackgroundImage( "data/GUI/bg-options.jpg" );

        CScrollBar* scroll = w->CreateScrollBar("bestiary-scroll");
        scroll->SetPosition(90.f, 10.f, 2.f, 60.f);
	    scroll->SetBackgroundImage( "data/GUI/scrollbar-v.png" );
	    scroll->SetHandleImage( "data/GUI/scrollbar-handle.png" );
	    scroll->SetOrientation( ORIENTATION_Y );
	    scroll->SetHandleSize( 5.0f );

        CScrollPanel* panel = w->CreateScrollPanel("bestiary-panel");
        panel->SetPosition(10.f, 10.f, 80.f, 60.f);
        panel->SetPadding(0.f, 0.f, 0.f, 0.f);
        panel->SetScrollBarY(scroll);

        CButton* retButton = w->CreateButton("bestiary-return-button");
        retButton->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
        retButton->SetText(gLocalizator.GetText("MENU_RETURN"));
		retButton->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
		retButton->SetPosition( 30.0f, 80.0f, 40.0f, 10.0f );
		retButton->SetCenter( true );
		retButton->GetClickCallback()->bind( this, &CMenuScreens::ShowPrevious );

        mBestiaryScreen = w;
    }
}

void CMenuScreens::UpdateBestiaryScreen()
{
    if (!mBestiaryScreen)
        InitBestiaryScreen();

    CGUIObject* panel = mBestiaryScreen->FindObject("bestiary-panel");
    CGUIObject* tooltip = NULL;
    if (panel)
    {
        CGUIObject* child = panel->FindObject("beast0");
        if (child)
        {
            CGUIObject* av = child->FindObject("avatar");
            if (av && av->GetTooltip())
                tooltip = av->GetTooltip();
        }

        // usuwanie avatarow
        for (size_t i = 0; (child = panel->FindObject("beast" + StringUtils::ToString(i))) != NULL; ++i)
            child->Remove();
    }
    else
        return;

    const float size = 80.f, tooltipW = 600.f, tooltipH = 450.f, statsW = 150.f, gap = 10.f;
    
    // ile obrazkow w rzedzie?
    unsigned width = (unsigned)(gGUI.ConvertToGlobalPosition(panel->GetSize()).x / (size + gap));

    const std::set<std::string>& enabledMonsters = gBestiary.GetEnabledMonsters();

    // tooltip jest wspoldzielony, zmieniaja sie tylko dane
    // i nie ma sensu robic tooltipa przy pustym bestiariuszu
    if (enabledMonsters.size() && !tooltip)
    {
        CWindow* tt = gGUI.CreateWindow("bestiary-page_tooltip", true, Z_GUI4);
        tt->SetPosition(0.f, 0.f, tooltipW, tooltipH, UNIT_PIXEL);
        tt->SetBackgroundImage("data/GUI/transparent-black.png");
        tt->SetOffset(sf::Vector2f(size, 20.f));
        tt->SetVisible(false);

        CAnimatedImage* img = tt->CreateAnimatedImage("img");
        img->SetPosition(20.f, 20.f, size, size, UNIT_PIXEL);

        CTextArea* name = tt->CreateTextArea("name");
        name->SetPosition(40.f + size, 20.f, tooltipW - 60.f - size, size, UNIT_PIXEL);
        name->SetFont(gGUI.GetFontSetting("FONT_DEFAULT", 2.0f));
        name->SetCenter(true);

        CTextArea* stats = tt->CreateTextArea("stats");
        stats->SetPosition(20.f, 40.f + size, statsW, tooltipH - 60.f - size, UNIT_PIXEL);
        stats->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));

        CTextArea* desc = tt->CreateTextArea("description");
        desc->SetPosition(40.f + statsW, 40.f + size, tooltipW - 60.f - statsW, tooltipH - 60.f - size, UNIT_PIXEL);
        desc->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));

        tooltip = tt;

        // ma nie reagowac na mysz, bo inaczej bedzie mrugac
        gGUI.UnregisterObject(img);
        gGUI.UnregisterObject(tt);
    }

    size_t i = 0;
    for (std::set<std::string>::const_iterator it = enabledMonsters.begin(); it != enabledMonsters.end(); ++it, ++i)
    {
        CImageBox* bg = panel->CreateImageBox("beast" + StringUtils::ToString(i));
        bg->SetPosition((i % width) * (size + gap), ((unsigned)(i / width)) * (size + gap), size, size, UNIT_PIXEL);
        bg->AddImageToSequence("data/GUI/bestiary/background.png");
        bg->SetSequenceState(0);

        CEnemyTemplate* templ = dynamic_cast<CEnemyTemplate*>(gResourceManager.GetPhysicalTemplate(*it));

        CAnimatedImage* av = bg->CreateAnimatedImage("avatar");
        if (templ->GetAvailableAnims().size() > 0)
            av->SetAnimation(templ->GetAvailableAnims()[0]->GetDefaultAnim());

        // kolorowanie
        av->GetDisplayable()->SetColor(templ->GetColor());

        av->SetPosition(10.f, 10.f, 80.f, 80.f);
        av->GetEventWStringCallback(MOUSE_OVER)->bind(this, &CMenuScreens::UpdateBestiaryTooltip);
        av->SetEventWStringParam(MOUSE_OVER, StringUtils::ConvertToWString(templ->GetFilename()));
        av->SetTooltip(tooltip);
    }
}

void CMenuScreens::UpdateBestiaryTooltip(const std::wstring& templ)
{
    CGUIObject* tooltip = gGUI.FindObject("bestiary-page_tooltip");
    if (!tooltip)
        return;

    CAnimatedImage* img = dynamic_cast<CAnimatedImage*>(tooltip->FindObject("img"));
    CTextArea* name = dynamic_cast<CTextArea*>(tooltip->FindObject("name"));
    CTextArea* desc = dynamic_cast<CTextArea*>(tooltip->FindObject("description"));
    CTextArea* stats = dynamic_cast<CTextArea*>(tooltip->FindObject("stats"));

    CEnemyTemplate* enemyTempl = dynamic_cast<CEnemyTemplate*>(gResourceManager.GetPhysicalTemplate(StringUtils::ConvertToString(templ)));
    if (!enemyTempl)
        return;

    if (enemyTempl->GetAvailableAnims().size() > 0)
        img->SetAnimation(enemyTempl->GetAvailableAnims()[0]->GetDefaultAnim());
    
    // kolorowanie
    img->GetDisplayable()->SetColor(enemyTempl->GetColor());

    name->SetText(gLocalizator.Localize(enemyTempl->GetBestiaryName()));
    desc->SetText(gLocalizator.Localize(enemyTempl->GetBestiaryDesc()));
    stats->SetText(enemyTempl->GetStatsWString());
}

void CMenuScreens::ExitGame()
{
    // dex: w glownym menu nie ma czego zapisywac
    if (gLogic.GetState() == L"pause-menu")
        gLogic.SaveGame(gGameOptions.GetUserDir() + "/game.save", true);

	gGame.GetRenderWindow()->Close();
}

void CMenuScreens::SetFlag(unsigned int flag, bool unset)
{
    if (unset)
        mFlags &= ~flag;
    else
        mFlags |= flag;

    mPrevMenus.push(mPrevMenus.top());
    if (mMainMenu) mMainMenu->Remove(), mMainMenu = 0;
    if (mPauseMenu) mPauseMenu->Remove(), mPauseMenu = 0;
	if (mOptions) mOptions->Remove(), mOptions = 0;
	if (mNewGameOptions) mNewGameOptions->Remove(), mNewGameOptions = 0;
    if (mReadmeScreen) mReadmeScreen->Remove(), mReadmeScreen = 0;
    if (mBindingOptions[0]) mBindingOptions[0]->Remove(), mBindingOptions[0] = 0;
    if (mBindingOptions[1]) mBindingOptions[1]->Remove(), mBindingOptions[1] = 0;
    ShowPrevious();

    InitAll();
}

void CMenuScreens::ChooseControls(const std::wstring& controls)
{
    gGameOptions.SetControls(StringUtils::ConvertToString(controls));
    // gGameOptions.SetControls usuwa user/first_game
    // zapis, zeby wiecej nie wyskakiwalo menu
    gGameOptions.SaveOptions();
    gLogic.StartNewGame(mNewGameMode);
}

void CMenuScreens::TryStartGame(const std::wstring& runMode)
{
    if (FileUtils::FileExists((gGameOptions.GetUserDir() + "/first_game").c_str()))
    {
        mNewGameMode = runMode;
        Show(L"choose-controls-menu");
    }
    else
        gLogic.StartNewGame(runMode);
}

/*void CMenuScreens::UpdateDifficultyString(void* control, unsigned state)
{	// aktualnie nieuzywane, bo wykomentowany jest suwaczek w InitNewGameOptions
    CTextArea* txt = NULL;
    if ((txt = dynamic_cast<CTextArea*>((CGUIObject*)control)) != NULL)
    {
        std::wstring diffTxt = gLocalizator.GetText("DIFFICULTY") + gLocalizator.GetText(("DIFFICULTY_" + StringUtils::ToString(state + 1)).c_str());
        txt->SetText(diffTxt);
        gLogic.SetDifficultyLevel(state);
    }
}*/

/*void CMenuScreens::UpdateArcadeMode()
{	// aktualnie nieuzywane, bo wykomentowany jest przycisk w InitNewGameOptions
    CGUIObject* obj = gGUI.FindObject("ng-options");
    if (obj)
    {
        obj = obj->FindObject("arcade");
        if (obj)
        {
            CCheckBox* cb = dynamic_cast<CCheckBox*>(obj);
            if (cb)
                gCommands.ParseCommand(std::wstring(L"set-arcade-mode ") + (cb->GetState() ? L"on" : L"off"));
        }
    }
}*/

CMenuScreens::CMenuScreens()
:	mMainMenu( NULL ),
    mPauseMenu( NULL ),
	mNewGameOptions( NULL ),
    mChooseControlsMenu( NULL ),
	mOptions( NULL ),
    mReadmeScreen( NULL ),
	mSaveScreen( NULL ),
	mLoadingScreen( NULL ),
    mAchievementsScreen( NULL ),
    mBestiaryScreen( NULL ),
    mFlags( 0 ),
    mSave(false)
{
	mBindingOptions[0] = mBindingOptions[1] = NULL;
}

CMenuScreens::~CMenuScreens()
{
}

