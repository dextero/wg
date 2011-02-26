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
char APP[] = "./bin/check_fullscreen";
bool CanCreateWindowInFullScreenOnLinux()
{
    int pid,status;
        
    if (!(pid=fork())) {
        int i = execlp(APP, 0);
        fprintf(stderr, "Failed to launch %s, code=%d, aborting\n", APP, i);
        _exit(-1);
    }
    while (pid!=wait(&status)) {
        // do nothing
    }
    return (status == 0);
}

#endif // !PLATFORM_OSX
#endif //PLATFORM_LINUX
