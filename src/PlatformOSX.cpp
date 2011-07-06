#ifdef PLATFORM_MACOSX
#ifndef PLATFORM_IPHONE
extern "C" const char * GetUserDir_OSX();

#include <string>
#include "Utils/FileUtils.h"
static std::string gUserDir = "~/Library/WarlocksGauntlet";
const std::string & FileUtils::GetUserDir()
{
	static bool sInitialized = false;
	if (!sInitialized) {
		gUserDir = std::string(GetUserDir_OSX()) + "/Library/WarlocksGauntlet";
		fprintf(stderr, "Detected userDir as `%s'\n", gUserDir.c_str());
		sInitialized = true;
	}
	return gUserDir;
}

extern "C" void CreateDir_OSX(const char * path);

void FileUtils::CreateDir(const char * filename)
{
	fprintf(stderr, "Creating directory `%s'...\n", filename);
	CreateDir_OSX(filename);
}

extern "C" int AskForFullscreen_OSX(const wchar_t * title, const wchar_t * message, int maxw, int maxh);

bool AskForFullscreen(const wchar_t * title, const wchar_t * message, int maxw, int maxh)
{
	return (bool)AskForFullscreen_OSX(title, message, maxw, maxh);
}

void ShowMessageBox(const wchar_t * title, const wchar_t * message)
{
    // TODO
}

#endif // iphone
#endif //PLATFORM_MACOSX