#include "CApp.h"
#include "CEditorWnd.h"

CApp::CApp():
    mSingletonCleaner(new CSingletonCleaner())
{

#ifndef PLATFORM_LINUX
    ::AllocConsole();
    std::freopen("CONOUT$","wb",stderr);
#endif
}

CApp::~CApp()
{

#ifndef PLATFORM_LINUX
    ::FreeConsole();
#endif

    delete mSingletonCleaner;
}

bool CApp::OnInit()
{
	CEditorWnd* editor = new CEditorWnd(NULL, -1, L"WG Editor", wxPoint(50, 50), wxSize(1000, 750));
	editor->Show(true);

	return true;
}

IMPLEMENT_APP(CApp)