#if _WIN32

#include <windows.h>
#include <Knownfolders.h>
#include <Shlobj.h>
#include <string>
using namespace std;

void _xxx_die(const char * reason)
{
	MessageBoxA(0, reason, "wg", MB_ICONEXCLAMATION | MB_OK);
	ExitProcess(1);
}

#ifndef VISTA_SUX
wstring GetWindowsUserDir_Vista()
{
	PWSTR path = 0;
	if (S_OK != SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_CREATE, 0, &path)) _xxx_die("SHGetKnownFolderPath failed");
	wstring ret = path;
	CoTaskMemFree(path);
	return ret;
}
#endif

wstring GetWindowsUserDir_XP()
{
	WCHAR svgmpath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, svgmpath); 
	wcscat_s(svgmpath, MAX_PATH, TEXT("\\My Saved Games"));
	return svgmpath; 
}

wstring GetWindowsUserDir()
{
	wstring ret;
#ifndef VISTA_SUX
	DWORD winver = GetVersion();

	if ((DWORD)(LOBYTE(LOWORD(winver))) >= 6) 
		ret = GetWindowsUserDir_Vista();
	else
		ret = GetWindowsUserDir_XP();
#else
	ret = GetWindowsUserDir_XP();
#endif
	ret += L"\\Warlock's Gauntlet";
	return ret;
}

#endif // _WIN32
