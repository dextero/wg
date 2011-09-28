///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CMainWindow__
#define __CMainWindow__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include "../src/CSFMLWnd.h"
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/toolbar.h>
#include "../wx/contrib/include/wx/stc/stc.h"
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/button.h>
#include <wx/html/htmlwin.h>
#include <wx/splitter.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_TOOLBAR_APPLY 1000
#define wxID_TOOLBAR_CANCEL 1001

///////////////////////////////////////////////////////////////////////////////
/// Class CMainWindow
///////////////////////////////////////////////////////////////////////////////
class CMainWindow : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* mMenuBar;
		wxMenu* mFileMenu;
		wxMenu* mEdit;
		wxMenu* mOther;
		wxBoxSizer* mEditorMainSizer;
		CSFMLWnd* mSfml;
		wxNotebook* mTreeTabs;
		wxPanel* mTreePanel1;
		wxTreeCtrl* mTreeTiles;
		wxPanel* mTreePanel2;
		wxTreeCtrl* mTreeDoodahs;
		wxPanel* mTreePanel3;
		wxTreeCtrl* mTreePhysicals;
		wxPanel* mTreePanel4;
		wxTreeCtrl* mTreeOther;
		wxStatusBar* mStatusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnMenuFileNew( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuFileOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuFileSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuFileSaveAs( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuFileExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuEditUndo( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuEditRedo( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuEditResize( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuEditRandomBrushProperties( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuEditRun( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuEditViewXMLSource( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuOtherHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMenuOtherAbout( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSfmlWndEnterWindow( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnSfmlWndKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnSfmlWndLeaveWindow( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnSfmlWndLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnSfmlWndLeftUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnSfmlWndSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnTreeTabsPageChanged( wxNotebookEvent& event ) { event.Skip(); }
		virtual void OnTreeTilesSelChanged( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeDoodahsSelChanged( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreePhysicalsSelChanged( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeOtherSelChanged( wxTreeEvent& event ) { event.Skip(); }
		
	
	public:
		
		CMainWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1030,750 ), long style = wxCAPTION|wxCLOSE_BOX|wxDEFAULT_FRAME_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxTAB_TRAVERSAL );
		~CMainWindow();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CXmlSourceDlg
///////////////////////////////////////////////////////////////////////////////
class CXmlSourceDlg : public wxDialog 
{
	private:
	
	protected:
		wxToolBar* mToolbar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnToolbarApplyClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnToolbarCancelClicked( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxStyledTextCtrl* mText;
		
		CXmlSourceDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("XML source"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );
		~CXmlSourceDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CMapSizeDlg
///////////////////////////////////////////////////////////////////////////////
class CMapSizeDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* mSizeStatic;
		wxStaticText* mXStatic;
		wxStaticText* mYStatic;
		wxButton* mOK;
		wxButton* mCancel;
	
	public:
		wxSpinCtrl* mXNum;
		wxSpinCtrl* mYNum;
		
		CMapSizeDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Map size"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 175,131 ), long style = wxCAPTION|wxSTAY_ON_TOP );
		~CMapSizeDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CHelpWindow
///////////////////////////////////////////////////////////////////////////////
class CHelpWindow : public wxFrame 
{
	private:
	
	protected:
		wxSplitterWindow* mSplitter;
		wxPanel* mTreePanel;
		wxTreeCtrl* mHelpTree;
		wxPanel* mContentPanel;
		wxHtmlWindow* mHelpHtmlContent;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTreeSelChanged( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnHtmlLinkClicked( wxHtmlLinkEvent& event ) { event.Skip(); }
		
	
	public:
		
		CHelpWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~CHelpWindow();
		
		void mSplitterOnIdle( wxIdleEvent& )
		{
			mSplitter->SetSashPosition( 150 );
			mSplitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CHelpWindow::mSplitterOnIdle ), NULL, this );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CRandomBrushPropertiesDlg
///////////////////////////////////////////////////////////////////////////////
class CRandomBrushPropertiesDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* mMinScaleStatic;
		wxStaticText* mMaxScaleStatic;
		wxButton* mOK;
		wxButton* mCancel;
	
	public:
		wxSpinCtrl* mMinScale;
		wxSpinCtrl* mMaxScale;
		
		CRandomBrushPropertiesDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Random Brush"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxSTAY_ON_TOP );
		~CRandomBrushPropertiesDlg();
	
};

#endif //__CMainWindow__
