#include "CEditorScreens.h"
#include "../CWindow.h"
#include "../CButton.h"
#include "../CTextArea.h"
#include "../CProgressBar.h"
#include "../CImageBox.h"
#include "../CRoot.h"
#include "../CMultipanel.h"
#include "../CScrollPanel.h"
#include "../CScrollBar.h"
#include "../CDropList.h"
#include "../../Utils/StringUtils.h"
#include "../../Editor/CEditor.h"
#include "CEditorResourcePanel.h"
#include "../Localization/CLocalizator.h"
#include "../Localization/GeneralKeys.h"
#include <algorithm>
#undef CreateWindow

using namespace GUI;

void CEditorScreens::Show()
{
    if (!mEditor)
        Init();
    mEditor->SetVisible(true);
}

void CEditorScreens::Hide()
{
    if (mEditor)
        mEditor->SetVisible(false);
}

void CEditorScreens::Init()
{
    if (!mEditor){
		gEditor.SetEditorScreens( this );
		gEditor.SetEditorWorkspace( 0.0f, 0.0f, 80.0f, 100.0f );

        CWindow *editor = gGUI.CreateWindow("editor-window");
		editor->SetBackgroundImage( "data/GUI/editor-bckg.png" );
		editor->SetPosition(80.0f,0.0f,20.0f,100.0f);

		mEditorMode = editor->CreateDropList( "editor-mode" );
		mEditorMode->SetFont("data/GUI/verdana.ttf", 45.0f, UNIT_PERCENT );
		mEditorMode->SetImage( "data/GUI/droplist.png", "data/GUI/droplistd.png" );
		mEditorMode->SetOptionFont("data/GUI/verdana.ttf", 45.0f, UNIT_PERCENT );
		mEditorMode->SetOptionImage( "data/GUI/droplist-option.png", "data/GUI/droplist-optiond.png" );
		mEditorMode->SetOptionCenter( true );
		mEditorMode->SetPosition( 10.0f, 1.0f, 80.0f, 4.0f );
		mEditorMode->SetCenter( true );   
		mEditorMode->AddOption( L"Budowanie" );
		mEditorMode->AddOption( L"Usuwanie (doodahy)" );
		mEditorMode->AddOption( L"Usuwanie (physicale)" );
		mEditorMode->SetSelectedOption( L"Budowanie" );

        CMultipanel *panel = editor->CreateMultipanel("panel",3);
        panel->SetPosition(0.0f,6.0f,100.0f,86.0f);
        panel->SetSwitchersToPanelRatio(10.0f);
        panel->SetSwitcherImage(0,"data/GUI/editor-switcher-1.png");
        panel->SetSwitcherImage(1,"data/GUI/editor-switcher-2.png");
        panel->SetSwitcherImage(2,"data/GUI/editor-switcher-3.png");

        CWindow *wnd = panel->GetWindow(0);
        CEditorResourcePanel *ers = wnd->CreateEditorResourcePanel("pnl");
        ers->SetPosition(0.0f,0.0f,100.0f,100.0f);
        ers->Load(gEditor.GetTiles());

		wnd = panel->GetWindow(1);
		ers = wnd->CreateEditorResourcePanel("pnl");
		ers->SetPosition(0.0f,0.0f,100.0f,100.0f);
		ers->Load(gEditor.GetDoodahs());

		wnd = panel->GetWindow(2);
		ers = wnd->CreateEditorResourcePanel("pnl");
		ers->SetPosition(0.0f,0.0f,100.0f,100.0f);
		ers->Load(gEditor.GetPhysicals());

		CTextArea * hint = editor->CreateTextArea("hint");
		hint->SetText(L"pomoc: F11 -> editor-help\nrotacja: lewo prawo\nskalowanie: gora dol\nwysokosc doodaha: z");
        hint->SetFont(gGUI.GetFontSetting("FONT_DEFAULT")); // bylo 12.
        hint->SetColor(sf::Color::Black);
        hint->SetPosition(10.0f,92.0f,80.0f,8.0f);
        
        mEditor = editor;
    }
}

void CEditorScreens::Update()
{
	if (mEditor)
	{
		CEditor::editorMode em;
		if ( mEditorMode->GetSelectedOption() == L"Budowanie" )					em = CEditor::PLACEMENT_MODE;
		else if ( mEditorMode->GetSelectedOption() == L"Usuwanie (doodahy)" )	em = CEditor::DOODAHS_ERASING_MODE;
		else																	em = CEditor::PHYSICALS_ERASING_MODE;

		if (em != gEditor.GetMode())
			gEditor.SetMode(em);
	}
}

CEditorScreens::CEditorScreens()
:   mEditor( NULL ), mEditorMode( NULL )
{
}

CEditorScreens::~CEditorScreens()
{
}

