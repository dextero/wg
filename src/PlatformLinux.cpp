#ifdef PLATFORM_LINUX
#ifndef PLATFORM_MACOSX
#include <wordexp.h>
#include <string>
#include <stdio.h>
#include <sys/stat.h>

#include "Utils/FileUtils.h"

static std::string gUserDir = "~/.WarlocksGauntlet";
const std::string & FileUtils::GetUserDir()
{
	static bool sInitialized = false;
    if (!sInitialized) {
        wordexp_t exp_result;
        wordexp("~/.WarlocksGauntlet", &exp_result, 0);
        gUserDir = exp_result.we_wordv[0];
        wordfree(&exp_result);

        //inny sposob: char * home = getenv("HOME");

        fprintf(stderr, "Detected userDir as `%s'\n", gUserDir.c_str());

        sInitialized = true;
    }
    return gUserDir;
}

void FileUtils::CreateDir(const char * filename)
{
    fprintf(stderr, "Creating directory `%s'...\n", filename);
    mkdir(filename, 0700);
}

bool AskForFullscreen(const wchar_t * title, const wchar_t * message, int maxw, int maxh)
{
	return true;
}

#include <sys/wait.h>
char APP32[] = "./bin/check_fullscreen.bin32";
char APP64[] = "./bin/check_fullscreen.bin64";
bool CanCreateWindowInFullScreenOnLinux()
{
    int pid,status;
        
    if (!(pid=fork())) {
        int ret32 = execlp(APP32, 0);
        int ret64 = execlp(APP64, 0);
        fprintf(stderr, "Failed to check fullscreen, codes=%d,%d, aborting\n", ret32, ret64);
        _exit(-1);
    }
    while (pid!=wait(&status)) {
        // do nothing
    }
    return (status == 0);
}

#endif // !PLATFORM_OSX
#endif //PLATFORM_LINUX
