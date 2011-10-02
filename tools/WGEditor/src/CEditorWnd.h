#ifndef __CEDITORWND_H__
#define __CEDITORWND_H__

#include "../wxFormBuilder/CMainWindow.h"
#include "CXmlSourceDialog.h"
#include "CHelpDialog.h"
#include "CActionsList.h"

#include "wx/process.h"


class CEditorWnd: public CMainWindow
{
public:
	// pola

	// metody
	CEditorWnd(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxCAPTION|wxCLOSE_BOX|wxDEFAULT_FRAME_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxTAB_TRAVERSAL);
	~CEditorWnd();

    inline const std::wstring& GetCurrentMap() { return mCurrentMap; }

private:
    std::wstring mCurrentMap;

    // zrodlo xml
    CXmlSourceDialog* mXmlSourceDlg;

    // pomoc
    CHelpDialog* mHelpDlg;

    // rozmiar nowej mapy
    CMapSizeDlg* mMapSizeDlg;

    // opcje pedzla losujacego
    CRandomBrushPropertiesDlg*  mRandomBrushPropertiesDlg;

    // min/max skala na pedzlu losujacym
    float mRandomBrushMinScale, mRandomBrushMaxScale;

    // zapisane akcje undo/redo
    Actions::EActionType mCurrActionType;
    std::wstring mSelectedItemStr;

    // proces odpalonej gry
    int mGameProcessId;

    // rekurencyjne wczytywanie zawartosci folderow, katalogi sa na wierzchu
    static void LoadFromDirectory(std::vector<std::string>& directories, std::vector<std::string>& files);
	static void LoadFromVFS(std::vector<std::string>& files);
    // ladowanie zawartosci folderu do drzewek
    bool LoadMod(const std::string& modDir);

    void HandleAction(SAction action);

    void PrepareRandomBrush();
    void FocusCurrentTree();
    const wxString GetPathToItem(wxTreeCtrl* tree, wxTreeItemId item);

    // -------------------------------------
    void InitTreeTiles(std::vector<std::wstring>& tiles, int defaultImg);
    void InitTreeDoodahs(std::vector<std::wstring>& doodahs, int defaultImg);
    void InitTreePhysicals(std::vector<std::wstring>& physicals, int defaultImg);
    void InitTreeOther(int defaultImg);

    // EVENTY
    virtual void OnClose(wxCloseEvent& event);
	virtual void OnSize(wxSizeEvent& event);
    virtual void OnSfmlWndSize(wxSizeEvent& event);

    // zmiana zakladki
    virtual void OnTreeTabsPageChanged(wxNotebookEvent& event);

    // zmiana wyboru w drzewkach
	virtual void OnTreeTilesSelChanged(wxTreeEvent& event);
	virtual void OnTreeDoodahsSelChanged(wxTreeEvent& event);
	virtual void OnTreePhysicalsSelChanged(wxTreeEvent& event);
	virtual void OnTreeOtherSelChanged(wxTreeEvent& event);

    // nacisniecie klawisza, z focusem na edytorze
    virtual void OnSfmlWndKeyDown(wxKeyEvent& event);

    // najazd mycha na edytor
    virtual void OnSfmlWndEnterWindow(wxMouseEvent& event);
    virtual void OnSfmlWndLeaveWindow(wxMouseEvent& event);

    // klik na edytorze (do undo)
    virtual void OnSfmlWndLeftDown(wxMouseEvent& event);
    // mouseup na edytorze - do losowanego pedzla
    virtual void OnSfmlWndLeftUp(wxMouseEvent& event);

    // menu - file
    virtual void OnMenuFileNew(wxCommandEvent& event);
    virtual void OnMenuFileOpen(wxCommandEvent& event);
    //virtual void OnMenuFileLoadMod(wxCommandEvent& event);
    virtual void OnMenuFileSave(wxCommandEvent& event);
    virtual void OnMenuFileSaveAs(wxCommandEvent& event);
    virtual void OnMenuFileExit(wxCommandEvent& event);

    // menu - edit
    virtual void OnMenuEditUndo(wxCommandEvent& event);
    virtual void OnMenuEditRedo(wxCommandEvent& event);
    virtual void OnMenuEditResize(wxCommandEvent& event);
    virtual void OnMenuEditRandomBrushProperties(wxCommandEvent& event);
    virtual void OnMenuEditRun(wxCommandEvent& event);
    virtual void OnMenuEditViewXMLSource(wxCommandEvent& event);

    // menu - other
    virtual void OnMenuOtherHelp(wxCommandEvent& event);
    virtual void OnMenuOtherAbout(wxCommandEvent& event);
};

#endif // __CEDITORWND_H__