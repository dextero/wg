#include "CButton.h"
#include "CImageBox.h"
#include "CScrollBar.h"
#include "CCheckBox.h"
#include "CRoot.h"
#include "CWindow.h"
#include "CSaveScreen.h"
#include "CScreens.h"
#include "Localization/CLocalizator.h"
#include "../Logic/CLogic.h"
#include "../Utils/StringUtils.h"
#include "../Utils/FileUtils.h"
#include "../CGameOptions.h"
#include "../ResourceManager/CResourceManager.h"
#include <string>
#include <fstream>

#include "../Input/CBindManager.h"
#include "Messages/CMessageSystem.h"


using namespace GUI;

const int kNumberOfSaveSlots = 5;
const float kTopOffset = 10.0f;
const float kDifficultyOffset = 10.f;
const float kCancelButtonOffset = 80.0f;
const float kTotalSaveSlotHeight = (kCancelButtonOffset - kTopOffset - kDifficultyOffset) / kNumberOfSaveSlots;
const float kSaveSlotHeight = kTotalSaveSlotHeight - 3.0f;

#undef CreateWindow

CSaveScreen::CSaveScreen(CScreens * parent)
{
    CWindow * saveScreen = gGUI.CreateWindow( "save-screen" );
    saveScreen->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
    saveScreen->SetBackgroundImage( "data/GUI/bg-options.jpg" );

    CTextArea* diffTxt = saveScreen->CreateTextArea("difficulty-string");
    diffTxt->SetFont(gLocalizator.GetFont(FONT_DIALOG), 14.f);
    diffTxt->SetText(gLocalizator.GetText("DIFFICULTY") + gLocalizator.GetText("DIFFICULTY_1"));
    diffTxt->SetPosition(15.f, 73.f, 30.f, 4.f);
    mDiffTxt = diffTxt;

    CScrollBar* diff = saveScreen->CreateScrollBar("difficulty-bar");
    diff->SetStatesCount(5);
    diff->SetHandleImage("data/GUI/scrollbar-handle.png");
    diff->SetBackgroundImage("data/GUI/scrollbar.png");
    diff->SetPosition(45.0f, 72.0f, 20.0f, 4.0f);
    diff->SetHandleSize(16.0f);
    diff->GetSlideParamCallback()->bind(gLogic.GetMenuScreens(), &CMenuScreens::UpdateDifficultyString);
    diff->SetSlideCallbackParams(diffTxt);
    mDiffBar = diff;

    CButton* loadBtn = saveScreen->CreateButton("load-button");
    loadBtn->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
    loadBtn->SetFont( gLocalizator.GetFont(GUI::FONT_MENU), 38.0f, UNIT_PERCENT );
    loadBtn->SetText( gLocalizator.GetText("MENU_LOAD") );
    loadBtn->SetPosition( 66.0f, 71.f, 29.0f, 6.0f );
    loadBtn->SetCenter( true );
    loadBtn->GetClickParamCallback()->bind(this, &CSaveScreen::SlotPressed);
    // ktory slot? to sie ustawia po zaznaczeniu slota..
    mLoadButton = loadBtn;

    for (int i = 0; i < kNumberOfSaveSlots; ++i)
    {
        std::string slotName = std::string("save-slot") + StringUtils::ToString(i);
        CWindow * saveSlot = saveScreen->CreateWindow( slotName );
        saveSlot->SetPosition(14.0f, kTopOffset + kTotalSaveSlotHeight * i, 
                              86.0f, kSaveSlotHeight);
        saveSlot->SetVisible(false);
        mSlots.push_back(saveSlot);

        CImageBox * thumb = saveSlot->CreateImageBox(slotName + "-thumb");
        thumb->SetPosition(0, 0, 4 * kSaveSlotHeight / 3, 100);

        CButton * button = saveSlot->CreateButton(slotName + "-button");
        button->SetFont( gLocalizator.GetFont(GUI::FONT_MENU), 25.0f, UNIT_PERCENT );
        button->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
        button->SetCenter( true );
        button->SetPosition(4 * kSaveSlotHeight / 3, 0, 100 - 4 * kSaveSlotHeight / 3, 100);
    }

    CButton * exitSaveScreen = saveScreen->CreateButton( "save-screen-exit" );
    exitSaveScreen->SetImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png" );
    exitSaveScreen->SetFont( gLocalizator.GetFont(GUI::FONT_MENU), 38.0f, UNIT_PERCENT );
    exitSaveScreen->SetText( gLocalizator.GetText("OPT_CANCEL") );
    exitSaveScreen->SetPosition( 35.0f, kCancelButtonOffset, 30.0f, 6.0f );
    exitSaveScreen->SetCenter( true );
    exitSaveScreen->GetClickCallback()->bind( parent, &CScreens::ShowPrevious );

    mSaveScreen = saveScreen;
}

CSaveScreen::~CSaveScreen()
{}

void GUI::CSaveScreen::SlotPressed( const std::wstring & params )
{
    if (mSave)
    {
        gLogic.SaveGame(StringUtils::ConvertToString(params), true);
        gLogic.ReturnToGame();
		gMessageSystem.AddMessagef(gLocalizator.GetText("MSG_GAME_SAVED").c_str());
    }
    else
    {
		std::string filename = StringUtils::ConvertToString(params);
		if (FileUtils::FileExists(filename))
        {
			std::string str = FileUtils::ReadFromFile(filename);

            // szukanie poziomu trudnosci
            std::string name = "set-difficulty-factor ";
            size_t at = str.find(name) + name.size();
            size_t end = str.find("\n", at);

            // czy trzeba zmieniac?
            float factor;
            sscanf(str.substr(at, end - at).c_str(), "%f", &factor);
            if (gLogic.GetDifficultyFactor() != factor)
            {
                // tak, podmien w save
                str.replace(at, end - at, StringUtils::ToString(gLogic.GetDifficultyFactor()));
				FileUtils::WriteToFile(filename, str);
            }
        }

        gLogic.LoadGame(StringUtils::ConvertToString(params));
    }
}

void GUI::CSaveScreen::UpdateSlots( bool save )
{
    mSave = save;
    int currentSlot = 0;

    // pasek zmiany trudnosci i button "laduj" tylko przy loadzie
    if (mDiffTxt)   mDiffTxt->SetVisible(!mSave);
    if (mDiffBar)   mDiffBar->SetVisible(!mSave);
    if (mLoadButton)
    {
        mLoadButton->SetVisible(!mSave);
        // zeby nie bylo wczytywania poprzednio zaznaczonego save po ukryciu i przywroceniu menu
        dynamic_cast<CButton*>(mLoadButton)->SetClickCallbackParams(L"");
    }

    if (mSave == false && gLogic.CanLoadGame(gGameOptions.GetUserDir() + "/game.save"))
    {
        UpdateSlot(currentSlot++, 
                   gLocalizator.GetText("MENU_AUTOSAVE"),
                   gGameOptions.GetUserDir() + "/game.save", gGameOptions.GetUserDir() + "/game.jpg");
    }

    for (int i = 0; i < kNumberOfSaveSlots - 1; ++i)
    {
        std::string gameName = gGameOptions.GetUserDir() + std::string("/game") + 
                               StringUtils::ToString(i);

        bool canLoadGame = gLogic.CanLoadGame(gameName + ".save");

        std::wstring caption = canLoadGame ? gLogic.GetGameInfo(gameName + ".save") : 
                                             gLocalizator.GetText("MENU_EMPTY_SAVE_SLOT");
        std::string thumb  = canLoadGame ? gameName + ".jpg" : 
                                           "data/GUI/empty-save-slot.png";
        if (canLoadGame || mSave)
        {
            UpdateSlot(currentSlot++, caption, gameName + ".save", thumb);
        }
    }

    for (int i = currentSlot; i < kNumberOfSaveSlots; ++i)
    {
        mSlots.at(i)->SetVisible(false);
    }
}

void GUI::CSaveScreen::SetVisible( bool visible /*= true*/ )
{
    mSaveScreen->SetVisible(visible);
}

void GUI::CSaveScreen::UpdateDifficultyBar(int hoveredBtnNum)
{
    std::string saveName = gGameOptions.GetUserDir() + std::string("/game") + (((unsigned)hoveredBtnNum > 0) ? StringUtils::ToString((unsigned)hoveredBtnNum - 1) : "") + ".save";

    if (!mSaveScreen || mSave)
        return;

    if (gLogic.CanLoadGame(saveName))
    {
		std::string str = FileUtils::ReadFromFile(saveName);

		// szukanie poziomu trudnosci
        std::string name = "set-difficulty-factor ";
        size_t at = str.find(name);
        
        float factor;
        if (at == std::string::npos)
            factor = 1.f;
        else
        {
            // wycinanie poziomu trudnosci
            at += name.size();
            size_t end = str.find('\n', at);

            if (sscanf(str.substr(at, end - at).c_str(), "%f", &factor) == 0)
                factor = 1.f;
        }

        // zamiana factora na indeks
		unsigned level = gLogic.DifficultyFactorToLevel(factor);

        CScrollBar* scroll = dynamic_cast<CScrollBar*>(mSaveScreen->FindObject("difficulty-bar"));
        if (!scroll)
            return;

        scroll->SetState((float)level / (scroll->GetStatesCount() - 1));
        // i odswiezamy napis..
        (*scroll->GetSlideParamCallback())(mSaveScreen->FindObject("difficulty-string"), (unsigned)(scroll->GetState() * (scroll->GetStatesCount() - 1)));
    }
}

void GUI::CSaveScreen::UpdateSlot(int slot, const std::wstring & caption,
    const std::string & gameName, const std::string & thumbnail )
{
    std::string slotName = std::string("save-slot") + StringUtils::ToString(slot);			

    CButton * button = (CButton *) mSaveScreen->FindObject(slotName + "-button");
    button->SetText(caption);
    button->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");

    if (mSave) // save
    {
        // czysc eventy z load
        button->GetEventWStringCallback(MOUSE_DBL_CLICK)->clear();
        button->GetEventVoidPtrCallback(MOUSE_PRESSED_LEFT)->clear();

        // press: zapisz
        button->GetClickParamCallback()->bind(this, &CSaveScreen::SlotPressed);
        button->SetClickCallbackParams(StringUtils::ConvertToWString(gameName));
    }
    else // load
    {
        // czysc event z save
        button->GetClickParamCallback()->clear();

        // double-click: laduj
        button->GetEventWStringCallback(MOUSE_DBL_CLICK)->bind( this, &CSaveScreen::SlotPressed );
        button->SetEventWStringParam(MOUSE_DBL_CLICK, StringUtils::ConvertToWString(gameName));

        // click: zaznacz i odswiez suwak
        button->GetEventIntCallback(MOUSE_PRESSED_LEFT)->bind( this, &CSaveScreen::SelectSlot );
        button->SetEventIntParam(MOUSE_PRESSED_LEFT, slot);
    }

    CImageBox * image = (CImageBox *) mSaveScreen->FindObject(slotName + "-thumb");
    image->ReleaseImages();
    gResourceManager.DropResource(thumbnail); // trzeba wymusic przeladowanie obrazka, bo mogl sie zmienic
    image->AddImageToSequence(thumbnail);

    mSlots.at(slot)->SetVisible(true);
}

void GUI::CSaveScreen::SelectSlot(int slotNum)
{
    int num = slotNum;

    for (int i = 0; i < (int)mSlots.size(); ++i)
    {
        std::string slotName = std::string("save-slot") + StringUtils::ToString(i) + "-button";

        CButton* btn = dynamic_cast<CButton*>(mSlots[i]->FindObject(slotName));
        if (btn != NULL)
        {
            btn->SetImage((i == num ? "data/GUI/alt/btn-up.png" : "data/GUI/btn-up.png"), "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
            
            if (i == num)
            {
                std::string gameName;
                // jesli nie mozna wczytac gry, to znaczy ze slot i jest pusty, a save jest gdzies dalej..
                while (!gLogic.CanLoadGame(gameName = gGameOptions.GetUserDir() + std::string("/game") + (i > 0 ? StringUtils::ToString(i - 1) : "") + ".save"))
                    ++i;
                dynamic_cast<CButton*>(mLoadButton)->SetClickCallbackParams(StringUtils::ConvertToWString(gameName));
            }
        }
    }

    UpdateDifficultyBar(slotNum);
}
