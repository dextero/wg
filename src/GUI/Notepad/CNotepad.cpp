#include "CNotepad.h"
#include "../CWindow.h"
#include "../CTextArea.h"
#include "../CScrollPanel.h"
#include "../CScrollBar.h"
#include "../CButton.h"
#include "../CRoot.h"
#include "../../Utils/StringUtils.h"
#include "../Localization/CLocalizator.h"

using namespace GUI;
using namespace StringUtils;

const float CAPTION_BTN_HEIGHT = 20.0f;

template<> CNotepad* CSingleton<CNotepad>::msSingleton = 0;

CNotepad::CNotepad()
{
	mNotepadWnd = gGUI.CreateWindow( "notepad", true, Z_GUI3 );
	mNotepadWnd->SetPosition( 50.0f,50.0f,0.0f,0.0f,UNIT_PERCENT );
	mNotepadWnd->SetPosition(-350.0f, -300.0f, 700.0f, 600.0f,UNIT_PIXEL );
	mNotepadWnd->SetBackgroundImage( "data/GUI/abilities-holder.png" );

	mCaptionsScrollBar = mNotepadWnd->CreateScrollBar( "np-caption-sb" );
	mCaptionsScrollBar->SetPosition( 35.0f, 5.0f, 2.0f, 90.0f );
	mCaptionsScrollBar->SetBackgroundImage( "data/GUI/scrollbar-v.png" );
	mCaptionsScrollBar->SetHandleImage( "data/GUI/scrollbar-handle.png" );
	mCaptionsScrollBar->SetOrientation( ORIENTATION_Y );
	mCaptionsScrollBar->SetHandleSize( 5.0f );

	mCaptionsPanel = mNotepadWnd->CreateScrollPanel( "np-caption-panel" );
	mCaptionsPanel->SetPosition( 3.0f, 3.0f, 30.0f, 94.0f );
	mCaptionsPanel->SetScrollBarY( mCaptionsScrollBar );

	mPageScrollBar = mNotepadWnd->CreateScrollBar( "np-page-sb" );
	mPageScrollBar->SetPosition( 95.0f, 5.0f, 2.0f, 90.0f );
	mPageScrollBar->SetBackgroundImage( "data/GUI/scrollbar-v.png" );
	mPageScrollBar->SetHandleImage( "data/GUI/scrollbar-handle.png" );
	mPageScrollBar->SetOrientation( ORIENTATION_Y );
	mPageScrollBar->SetHandleSize( 5.0f );

	CScrollPanel* pagePanel = mNotepadWnd->CreateScrollPanel( "np-page-panel" );
	pagePanel->SetPosition( 40.0f, 3.0f, 53.0f, 94.0f );
	pagePanel->SetScrollBarY( mPageScrollBar );

	mPageContent = pagePanel->CreateTextArea( "np-page-content" );
	mPageContent->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
	mPageContent->SetFont( gLocalizator.GetFont(GUI::FONT_DIALOG), 14.0f );
	mPageContent->SetColor( sf::Color::White );
	mPageContent->SetAutoHeight( true );

	Hide();
}

CNotepad::~CNotepad()
{
}

void CNotepad::Show(const std::wstring &page)
{
	mNotepadWnd->SetVisible( true );
	mCaptionsScrollBar->SetState( 0.0f );
	mPageScrollBar->SetState( 0.0f );

	if ( SPage* pg = FindPage( page ) )
	{
		std::wstring txt;

		for ( unsigned i = 0; i < pg->notes.size(); i++ )
		{
			txt += pg->notes[i];
			txt += L"\n\n";
		}

		mPageContent->SetText( txt );
	}
}

void CNotepad::Hide()
{
	mNotepadWnd->SetVisible( false );
}

void CNotepad::AddNote(const std::wstring &page, const std::wstring &note)
{
	SPage * pg;

	if (( pg = FindPage( page ) ))
	{
		pg->notes.push_back( note );
	}
	else
	{
		pg = AddPage( page );
		pg->notes.push_back( note );
	}
}

/* ================== PRIVATE METHODS ===================== */

CNotepad::SPage* CNotepad::FindPage(const std::wstring &caption)
{
	for ( unsigned i = 0; i < mPages.size(); i++ )
	if ( mPages[i].caption == caption )
		return &mPages[i];

	return NULL;
}

CNotepad::SPage* CNotepad::AddPage(const std::wstring &caption)
{
	SPage page;
	page.caption = caption;
	page.caption_btn = mCaptionsPanel->CreateButton( std::string("caption-btn") + ToString(mPages.size()) );
	page.caption_btn->SetPosition( 0.0f, 0.0, 100.0f, 0.0f, UNIT_PERCENT );
	page.caption_btn->SetPosition( 0.0f, CAPTION_BTN_HEIGHT * mPages.size(), 0.0f, CAPTION_BTN_HEIGHT, UNIT_PIXEL );
	page.caption_btn->SetImage( "data/GUI/droplist-option.png", "data/GUI/droplist-optiond.png" );
	page.caption_btn->SetFont( gLocalizator.GetFont(GUI::FONT_DIALOG), 14.0f );
	page.caption_btn->SetColor( sf::Color::White );
	page.caption_btn->SetText( caption );
	page.caption_btn->GetClickParamCallback()->bind( this, &GUI::CNotepad::Show );
	page.caption_btn->SetClickCallbackParams( caption );
	page.caption_btn->SetVisible( mNotepadWnd->IsVisible() );

	mPages.push_back( page );
	return &mPages[ mPages.size() - 1 ];
}

