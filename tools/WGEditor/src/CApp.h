#ifndef __CAPP_H__
#define __CAPP_H__

#include "wx/wx.h"
#include "Utils/CSingletonCleaner.h"
#include <stdio.h>

#ifndef PLATFORM_LINUX
#   ifdef _DEBUG
#	    pragma comment(lib, "wxbase28ud.lib")
#       pragma comment(lib, "wxmsw28ud_stc.lib")
#   else
#	    pragma comment(lib, "wxbase28u.lib")
#       pragma comment(lib, "wxmsw28u_stc.lib")
#   endif // _DEBUG
#   pragma comment(lib, "comctl32.lib")
#   pragma comment(lib, "Rpcrt4.lib")
#endif

class CApp: public wxApp
{
public:
    CApp();
    ~CApp();

private:
	CSingletonCleaner* mSingletonCleaner;

	virtual bool OnInit();
};

#endif // __CAPP_H__