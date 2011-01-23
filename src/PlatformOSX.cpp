#ifdef PLATFORM_MACOSX
extern "C" const char * GetUserDir_OSX();

static std::string gUserDir = "~/Library/WarlocksGauntlet";
const std::string & FileUtils::GetUserDir()
{
	static bool sInitialized = false;
	if (!sInitialized) {
		gUserDir = GetUserDir_OSX() + "/Library/WarlocksGauntlet;
        fprintf(stderr, "Detected userDir as `%s'\n", gUserDir.c_str());
        gInitialized = true;
	}
    return gUserDir;
}

extern "C" void CreateDir_OSX(const char * path);

void FileUtils::CreateDir(const char * filename)
{
    fprintf(stderr, "Creating directory `%s'...\n", filename);
    CreateDirectory_OSX(filename);
}

extern "C" int AskForFullscreen_OSX(const wchar_t * title, const wchar_t * message, int maxw, int maxh);

bool AskForFullscreen(const wchar_t * title, const wchar_t * message, int maxw, int maxh)
{
	return (bool)AskForFullscreen_OSX(titleStr.c_str(), messageStr.c_str(), maxw, maxh);
}
#endif //PLATFORM_MACOSX