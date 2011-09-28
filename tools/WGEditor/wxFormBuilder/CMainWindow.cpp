///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "CMainWindow.h"

///////////////////////////////////////////////////////////////////////////

CMainWindow::CMainWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 1000,750 ), wxDefaultSize );
	
	mMenuBar = new wxMenuBar( 0 );
	mFileMenu = new wxMenu();
	wxMenuItem* mFileNew;
	mFileNew = new wxMenuItem( mFileMenu, wxID_ANY, wxString( wxT("New") ) + wxT('\t') + wxT("Ctrl+N"), wxEmptyString, wxITEM_NORMAL );
	mFileMenu->Append( mFileNew );
	
	wxMenuItem* mFileOpen;
	mFileOpen = new wxMenuItem( mFileMenu, wxID_ANY, wxString( wxT("Open") ) + wxT('\t') + wxT("Ctrl+O"), wxEmptyString, wxITEM_NORMAL );
	mFileMenu->Append( mFileOpen );
	
	wxMenuItem* m_separator1;
	m_separator1 = mFileMenu->AppendSeparator();
	
	wxMenuItem* mFileSave;
	mFileSave = new wxMenuItem( mFileMenu, wxID_ANY, wxString( wxT("Save") ) + wxT('\t') + wxT("Ctrl+S"), wxEmptyString, wxITEM_NORMAL );
	mFileMenu->Append( mFileSave );
	
	wxMenuItem* mFileSaveAs;
	mFileSaveAs = new wxMenuItem( mFileMenu, wxID_ANY, wxString( wxT("Save as...") ) + wxT('\t') + wxT("Ctrl+Shift+S"), wxEmptyString, wxITEM_NORMAL );
	mFileMenu->Append( mFileSaveAs );
	
	wxMenuItem* m_separator2;
	m_separator2 = mFileMenu->AppendSeparator();
	
	wxMenuItem* mFileExit;
	mFileExit = new wxMenuItem( mFileMenu, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	mFileMenu->Append( mFileExit );
	
	mMenuBar->Append( mFileMenu, wxT("File") ); 
	
	mEdit = new wxMenu();
	wxMenuItem* mEditUndo;
	mEditUndo = new wxMenuItem( mEdit, wxID_ANY, wxString( wxT("Undo") ) + wxT('\t') + wxT("Ctrl+Z"), wxEmptyString, wxITEM_NORMAL );
	mEdit->Append( mEditUndo );
	
	wxMenuItem* mEditRedo;
	mEditRedo = new wxMenuItem( mEdit, wxID_ANY, wxString( wxT("Redo") ) + wxT('\t') + wxT("Ctrl+Y"), wxEmptyString, wxITEM_NORMAL );
	mEdit->Append( mEditRedo );
	
	wxMenuItem* m_separator3;
	m_separator3 = mEdit->AppendSeparator();
	
	wxMenuItem* mEditResize;
	mEditResize = new wxMenuItem( mEdit, wxID_ANY, wxString( wxT("Resize map") ) + wxT('\t') + wxT("Ctrl+R"), wxEmptyString, wxITEM_NORMAL );
	mEdit->Append( mEditResize );
	
	wxMenuItem* m_separator4;
	m_separator4 = mEdit->AppendSeparator();
	
	wxMenuItem* mEditRandomBrushProperties;
	mEditRandomBrushProperties = new wxMenuItem( mEdit, wxID_ANY, wxString( wxT("Random Brush Properties") ) + wxT('\t') + wxT("Ctrl+P"), wxEmptyString, wxITEM_NORMAL );
	mEdit->Append( mEditRandomBrushProperties );
	
	wxMenuItem* mEditRun;
	mEditRun = new wxMenuItem( mEdit, wxID_ANY, wxString( wxT("Run") ) + wxT('\t') + wxT("F5"), wxEmptyString, wxITEM_NORMAL );
	mEdit->Append( mEditRun );
	
	wxMenuItem* mEditXMLSource;
	mEditXMLSource = new wxMenuItem( mEdit, wxID_ANY, wxString( wxT("Show XML Source") ) + wxT('\t') + wxT("F6"), wxEmptyString, wxITEM_NORMAL );
	mEdit->Append( mEditXMLSource );
	
	mMenuBar->Append( mEdit, wxT("Edit") ); 
	
	mOther = new wxMenu();
	wxMenuItem* mOtherHelp;
	mOtherHelp = new wxMenuItem( mOther, wxID_ANY, wxString( wxT("Help") ) + wxT('\t') + wxT("F1"), wxEmptyString, wxITEM_NORMAL );
	mOther->Append( mOtherHelp );
	
	wxMenuItem* mOtherAbout;
	mOtherAbout = new wxMenuItem( mOther, wxID_ANY, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	mOther->Append( mOtherAbout );
	
	mMenuBar->Append( mOther, wxT("Other") ); 
	
	this->SetMenuBar( mMenuBar );
	
	mEditorMainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	mSfml = new CSFMLWnd( this, wxID_ANY, wxPoint(10, 10), wxSize(800, 700), wxWANTS_CHARS );
	mEditorMainSizer->Add( mSfml, 3, wxALL|wxEXPAND, 0 );
	
	mTreeTabs = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxNB_LEFT|wxFULL_REPAINT_ON_RESIZE );
	mTreePanel1 = new wxPanel( mTreeTabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
	wxBoxSizer* mEditorTreeSizer1;
	mEditorTreeSizer1 = new wxBoxSizer( wxVERTICAL );
	
	mTreeTiles = new wxTreeCtrl( mTreePanel1, wxID_ANY, wxPoint( 5,5 ), wxSize( -1,-1 ), wxTR_DEFAULT_STYLE|wxFULL_REPAINT_ON_RESIZE );
	mEditorTreeSizer1->Add( mTreeTiles, 1, wxALL|wxEXPAND, 5 );
	
	mTreePanel1->SetSizer( mEditorTreeSizer1 );
	mTreePanel1->Layout();
	mEditorTreeSizer1->Fit( mTreePanel1 );
	mTreeTabs->AddPage( mTreePanel1, wxT("Tiles"), true );
	mTreePanel2 = new wxPanel( mTreeTabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
	wxBoxSizer* mEditorTreeSizer2;
	mEditorTreeSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	mTreeDoodahs = new wxTreeCtrl( mTreePanel2, wxID_ANY, wxPoint( 5,5 ), wxSize( -1,-1 ), wxTR_DEFAULT_STYLE|wxTR_MULTIPLE|wxFULL_REPAINT_ON_RESIZE );
	mEditorTreeSizer2->Add( mTreeDoodahs, 1, wxALL|wxEXPAND, 5 );
	
	mTreePanel2->SetSizer( mEditorTreeSizer2 );
	mTreePanel2->Layout();
	mEditorTreeSizer2->Fit( mTreePanel2 );
	mTreeTabs->AddPage( mTreePanel2, wxT("Doodahs"), false );
	mTreePanel3 = new wxPanel( mTreeTabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
	wxBoxSizer* mEditorTreeSizer3;
	mEditorTreeSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	mTreePhysicals = new wxTreeCtrl( mTreePanel3, wxID_ANY, wxPoint( 5,5 ), wxSize( -1,-1 ), wxTR_DEFAULT_STYLE|wxTR_MULTIPLE|wxFULL_REPAINT_ON_RESIZE );
	mEditorTreeSizer3->Add( mTreePhysicals, 1, wxALL|wxEXPAND, 5 );
	
	mTreePanel3->SetSizer( mEditorTreeSizer3 );
	mTreePanel3->Layout();
	mEditorTreeSizer3->Fit( mTreePanel3 );
	mTreeTabs->AddPage( mTreePanel3, wxT("Physicals"), false );
	mTreePanel4 = new wxPanel( mTreeTabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
	wxBoxSizer* mEditorTreeSizer4;
	mEditorTreeSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	mTreeOther = new wxTreeCtrl( mTreePanel4, wxID_ANY, wxPoint( 5,5 ), wxSize( -1,-1 ), wxTR_DEFAULT_STYLE|wxFULL_REPAINT_ON_RESIZE );
	mEditorTreeSizer4->Add( mTreeOther, 1, wxALL|wxEXPAND, 5 );
	
	mTreePanel4->SetSizer( mEditorTreeSizer4 );
	mTreePanel4->Layout();
	mEditorTreeSizer4->Fit( mTreePanel4 );
	mTreeTabs->AddPage( mTreePanel4, wxT("Other"), false );
	
	mEditorMainSizer->Add( mTreeTabs, 1, wxALL|wxEXPAND, 0 );
	
	this->SetSizer( mEditorMainSizer );
	this->Layout();
	mStatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( CMainWindow::OnClose ) );
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( CMainWindow::OnSize ) );
	this->Connect( mFileNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileNew ) );
	this->Connect( mFileOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileOpen ) );
	this->Connect( mFileSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileSave ) );
	this->Connect( mFileSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileSaveAs ) );
	this->Connect( mFileExit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileExit ) );
	this->Connect( mEditUndo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditUndo ) );
	this->Connect( mEditRedo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditRedo ) );
	this->Connect( mEditResize->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditResize ) );
	this->Connect( mEditRandomBrushProperties->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditRandomBrushProperties ) );
	this->Connect( mEditRun->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditRun ) );
	this->Connect( mEditXMLSource->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditViewXMLSource ) );
	this->Connect( mOtherHelp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuOtherHelp ) );
	this->Connect( mOtherAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuOtherAbout ) );
	mSfml->Connect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( CMainWindow::OnSfmlWndEnterWindow ), NULL, this );
	mSfml->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( CMainWindow::OnSfmlWndKeyDown ), NULL, this );
	mSfml->Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( CMainWindow::OnSfmlWndLeaveWindow ), NULL, this );
	mSfml->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( CMainWindow::OnSfmlWndLeftDown ), NULL, this );
	mSfml->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( CMainWindow::OnSfmlWndLeftUp ), NULL, this );
	mSfml->Connect( wxEVT_SIZE, wxSizeEventHandler( CMainWindow::OnSfmlWndSize ), NULL, this );
	mTreeTabs->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( CMainWindow::OnTreeTabsPageChanged ), NULL, this );
	mTreeTiles->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CMainWindow::OnTreeTilesSelChanged ), NULL, this );
	mTreeDoodahs->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CMainWindow::OnTreeDoodahsSelChanged ), NULL, this );
	mTreePhysicals->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CMainWindow::OnTreePhysicalsSelChanged ), NULL, this );
	mTreeOther->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CMainWindow::OnTreeOtherSelChanged ), NULL, this );
}

CMainWindow::~CMainWindow()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( CMainWindow::OnClose ) );
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( CMainWindow::OnSize ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileNew ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileOpen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileSave ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileSaveAs ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuFileExit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditUndo ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditRedo ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditResize ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditRandomBrushProperties ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditRun ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuEditViewXMLSource ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuOtherHelp ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CMainWindow::OnMenuOtherAbout ) );
	mSfml->Disconnect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( CMainWindow::OnSfmlWndEnterWindow ), NULL, this );
	mSfml->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( CMainWindow::OnSfmlWndKeyDown ), NULL, this );
	mSfml->Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( CMainWindow::OnSfmlWndLeaveWindow ), NULL, this );
	mSfml->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( CMainWindow::OnSfmlWndLeftDown ), NULL, this );
	mSfml->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( CMainWindow::OnSfmlWndLeftUp ), NULL, this );
	mSfml->Disconnect( wxEVT_SIZE, wxSizeEventHandler( CMainWindow::OnSfmlWndSize ), NULL, this );
	mTreeTabs->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( CMainWindow::OnTreeTabsPageChanged ), NULL, this );
	mTreeTiles->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CMainWindow::OnTreeTilesSelChanged ), NULL, this );
	mTreeDoodahs->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CMainWindow::OnTreeDoodahsSelChanged ), NULL, this );
	mTreePhysicals->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CMainWindow::OnTreePhysicalsSelChanged ), NULL, this );
	mTreeOther->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CMainWindow::OnTreeOtherSelChanged ), NULL, this );
	
}

CXmlSourceDlg::CXmlSourceDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 800,600 ), wxDefaultSize );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	mToolbar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	mToolbar->AddTool( wxID_TOOLBAR_APPLY, wxT("tool"), wxBitmap( wxT("editor/img/save.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Apply"), wxEmptyString ); 
	mToolbar->AddTool( wxID_TOOLBAR_CANCEL, wxT("tool"), wxBitmap( wxT("editor/img/delete.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Cancel"), wxEmptyString ); 
	mToolbar->Realize();
	
	bSizer6->Add( mToolbar, 0, wxEXPAND, 5 );
	
	mText = new wxStyledTextCtrl(this, wxID_ANY);
	mText->SetLayoutCache(wxSTC_CACHE_PAGE);
	
	// lexer
	mText->SetLexer(wxSTC_LEX_XML);
	
	wxChar* keywords1 = L"map version width height tiletype tiles objtype cond type param effect-on-open text region next-map next-map-region sprite";
	wxChar* keywords2 = L"code image file x y z scale name check";
	
	// tagi.. daja cos w ogole te keywordsy? :x
	mText->SetKeyWords(1, keywords1);
	mText->StyleSetForeground(1, wxColour(L"BLUE"));
	
	// atrybuty
	mText->StyleSetForeground(3, wxColour(L"PURPLE"));
	
	// wartosci atrybutow
	mText->StyleSetForeground(6, wxColour(L"ORANGE"));
	
	mText->StyleSetForeground(wxSTC_STYLE_DEFAULT, wxColour(L"DARK GREY"));
	mText->StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour(L"DARK GREY"));
	
	// niech zyje copypasta
	int lineNrId = 0;
	int dividerId = 1;
	int foldingId = 2;
	
	// numerowanie linii
	mText->SetMarginType(lineNrId, wxSTC_MARGIN_NUMBER);
	mText->SetMarginWidth(lineNrId, 40);
	mText->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(L"DARK GREY"));
	mText->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(170, 170, 170));
	
	// jakies tam wtf
	mText->SetMarginType(dividerId, wxSTC_MARGIN_SYMBOL);
	mText->SetMarginWidth(dividerId, 0);
	mText->SetMarginSensitive(dividerId, false);
	
	// skladanie - nie dziala
	mText->SetMarginType (foldingId, wxSTC_MARGIN_SYMBOL);
	mText->SetMarginMask (foldingId, wxSTC_MASK_FOLDERS);
	mText->StyleSetBackground (foldingId, wxColor(L"LIGHT GREY"));
	mText->SetMarginWidth (foldingId, 16);
	mText->SetMarginSensitive (foldingId, false);
	
	mText->SetProperty(L"fold.html", L"1");
	mText->SetProperty(L"fold", L"1");
	mText->SetProperty(L"fold.compact", L"0");
	
	mText->SetFoldFlags (wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED |  wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
	
	// taby
	mText->SetTabWidth(4);
	mText->SetUseTabs(false);
	mText->SetTabIndents(true);
	mText->SetBackSpaceUnIndents(true);
	mText->SetIndent(4);
	
	// ---------------------------
	// hack - do glownego okna, a co!
	this->SetAutoLayout(true);
	bSizer6->Add( mText, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer6 );
	this->Layout();
	bSizer6->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxID_TOOLBAR_APPLY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CXmlSourceDlg::OnToolbarApplyClicked ) );
	this->Connect( wxID_TOOLBAR_CANCEL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CXmlSourceDlg::OnToolbarCancelClicked ) );
}

CXmlSourceDlg::~CXmlSourceDlg()
{
	// Disconnect Events
	this->Disconnect( wxID_TOOLBAR_APPLY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CXmlSourceDlg::OnToolbarApplyClicked ) );
	this->Disconnect( wxID_TOOLBAR_CANCEL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CXmlSourceDlg::OnToolbarCancelClicked ) );
	
}

CMapSizeDlg::CMapSizeDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	mSizeStatic = new wxStaticText( this, wxID_ANY, wxT("Enter map size (in tiles):"), wxDefaultPosition, wxDefaultSize, 0 );
	mSizeStatic->Wrap( -1 );
	bSizer10->Add( mSizeStatic, 1, wxALL, 5 );
	
	bSizer7->Add( bSizer10, 0, wxALIGN_CENTER|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	mXStatic = new wxStaticText( this, wxID_ANY, wxT("x ="), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	mXStatic->Wrap( -1 );
	bSizer8->Add( mXStatic, 0, wxALIGN_CENTER|wxALL, 5 );
	
	mXNum = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 999, 25 );
	bSizer8->Add( mXNum, 1, wxALL, 2 );
	
	bSizer7->Add( bSizer8, 0, wxALIGN_CENTER|wxEXPAND, 0 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	mYStatic = new wxStaticText( this, wxID_ANY, wxT("y ="), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	mYStatic->Wrap( -1 );
	bSizer9->Add( mYStatic, 0, wxALIGN_CENTER|wxALL, 5 );
	
	mYNum = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 999, 25 );
	bSizer9->Add( mYNum, 1, wxALL, 2 );
	
	bSizer7->Add( bSizer9, 0, wxALIGN_CENTER|wxEXPAND, 0 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	mOK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	mOK->SetDefault(); 
	bSizer12->Add( mOK, 1, wxALL, 5 );
	
	mCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( mCancel, 1, wxALL, 5 );
	
	bSizer11->Add( bSizer12, 1, wxEXPAND, 5 );
	
	bSizer7->Add( bSizer11, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

CMapSizeDlg::~CMapSizeDlg()
{
}

CHelpWindow::CHelpWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	mSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_3DSASH );
	mSplitter->Connect( wxEVT_IDLE, wxIdleEventHandler( CHelpWindow::mSplitterOnIdle ), NULL, this );
	
	mTreePanel = new wxPanel( mSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	mHelpTree = new wxTreeCtrl( mTreePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer14->Add( mHelpTree, 1, wxALL|wxEXPAND, 0 );
	
	mTreePanel->SetSizer( bSizer14 );
	mTreePanel->Layout();
	bSizer14->Fit( mTreePanel );
	mContentPanel = new wxPanel( mSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	mHelpHtmlContent = new wxHtmlWindow( mContentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	bSizer15->Add( mHelpHtmlContent, 1, wxALL|wxEXPAND, 0 );
	
	mContentPanel->SetSizer( bSizer15 );
	mContentPanel->Layout();
	bSizer15->Fit( mContentPanel );
	mSplitter->SplitVertically( mTreePanel, mContentPanel, 150 );
	bSizer13->Add( mSplitter, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer13 );
	this->Layout();
	
	// Connect Events
	mHelpTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CHelpWindow::OnTreeSelChanged ), NULL, this );
	mHelpHtmlContent->Connect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( CHelpWindow::OnHtmlLinkClicked ), NULL, this );
}

CHelpWindow::~CHelpWindow()
{
	// Disconnect Events
	mHelpTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CHelpWindow::OnTreeSelChanged ), NULL, this );
	mHelpHtmlContent->Disconnect( wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler( CHelpWindow::OnHtmlLinkClicked ), NULL, this );
	
}

CRandomBrushPropertiesDlg::CRandomBrushPropertiesDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	mMinScaleStatic = new wxStaticText( this, wxID_ANY, wxT("Min scale(%):"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	mMinScaleStatic->Wrap( -1 );
	bSizer17->Add( mMinScaleStatic, 2, wxALIGN_CENTER|wxALL, 5 );
	
	mMinScale = new wxSpinCtrl( this, wxID_ANY, wxT("70"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 10, 120, 0 );
	bSizer17->Add( mMinScale, 3, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer16->Add( bSizer17, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	mMaxScaleStatic = new wxStaticText( this, wxID_ANY, wxT("Max scale(%):"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	mMaxScaleStatic->Wrap( -1 );
	bSizer18->Add( mMaxScaleStatic, 2, wxALIGN_CENTER|wxALL, 5 );
	
	mMaxScale = new wxSpinCtrl( this, wxID_ANY, wxT("110"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 10, 120, 0 );
	bSizer18->Add( mMaxScale, 3, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer16->Add( bSizer18, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );
	
	mOK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	mOK->SetDefault(); 
	bSizer19->Add( mOK, 1, wxALIGN_CENTER|wxALL, 5 );
	
	mCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer19->Add( mCancel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer16->Add( bSizer19, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer16 );
	this->Layout();
	bSizer16->Fit( this );
	
	this->Centre( wxBOTH );
}

CRandomBrushPropertiesDlg::~CRandomBrushPropertiesDlg()
{
}
