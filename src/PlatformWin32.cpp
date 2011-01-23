#ifndef PLATFORM_WINDOWS
#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif
#endif
#ifdef PLATFORM_WINDOWS

#include <windows.h>
#include <shlobj.h>
#include <string>
#include "Utils/FileUtils.h"

static std::string gUserDir = "%APPDATA%\\WarlocksGauntlet";
const std::string & FileUtils::GetUserDir()
{
	static bool gInitialized = false;
    if (!gInitialized) {
		char path[ MAX_PATH ];
		if (SHGetFolderPathA( NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path ) != S_OK) {
			char * reason = "I could not create the user application data directory!\n";
			fprintf(stderr, "%s\n", reason);
			MessageBoxA(0, reason, "wg", MB_ICONEXCLAMATION | MB_OK);
			ExitProcess(1);
		}
		gUserDir = std::string(path) + "\\WarlocksGauntlet";
        fprintf(stderr, "Detected userDir as `%s'\n", gUserDir.c_str());

        gInitialized = true;
    }
    return gUserDir;
}

void FileUtils::CreateDir(const char* filename)
{
    fprintf(stderr, "Creating directory `%s'...\n", filename);
	CreateDirectoryA(filename, NULL);
}

bool AskForFullscreen(const wchar_t * title, const wchar_t * message, int maxw, int maxh)
{
	wchar_t message2[4096];
	wsprintf(message2, message, maxw, maxh);
	return IDOK == MessageBoxW(0, message2, title, MB_OKCANCEL | MB_ICONQUESTION);
}

#endif //PLATFORM_WINDOWS
