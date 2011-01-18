#include "FileUtils.h"
#include <fstream>

#include <stdio.h>
#include <sys/stat.h>

//#include <cstdio>
//#include <boost/filesystem/operations.hpp>

int FileUtils::GetFileSize(const char* filename)
{
    FILE * pFile;
    long size;

    pFile = fopen(filename,"rb");
    if (!pFile)
		return 0;
  
    fseek(pFile, 0, SEEK_END);
    size = ftell(pFile);
    fclose(pFile);
  
    return size;
}

bool FileUtils::FileExists(const char* filename)
{
	FILE *f = fopen(filename, "r");
	if (!f) return false;
	fclose(f);
	return true;
}

bool FileUtils::FileExists(const std::string & filename)
{
    return FileExists(filename.c_str());
}

// jakas inna implementacja, z CGameOptions.cpp
//bool FileExists( const std::string& filename )
//{
//    std::ifstream f( filename.c_str(), std::ios::in );
//    f.close();
//    return ( !f.fail() );
//}


#ifdef PLATFORM_LINUX
#include <wordexp.h>
static std::string gUserDir = "~/.WarlocksGauntlet/";
static bool gInitialized = false;
const std::string & FileUtils::GetUserDir()
{
    if (!gInitialized) {
        wordexp_t exp_result;
        wordexp("~/.WarlocksGauntlet/", &exp_result, 0);
        gUserDir = exp_result.we_wordv[0];
        wordfree(&exp_result);

        //inny sposob: char * home = getenv("HOME");

        fprintf(stderr, "Detected userDir as `%s'\n", gUserDir.c_str());

        gInitialized = true;
    }
    return gUserDir;
}

void FileUtils::InitializeUserDir()
{
    fprintf(stderr, "Initializing user dir...\n");
    mkdir(GetUserDir().c_str(), 0700);
    //i tutaj reszta inicjalizacji - kopiowanie 'first_game', 'config.xml' itepe itede
    // mozna tez pomyslec o tym, aby przy uruchamianu WarlocksGauntlet.exe --clean-user-dir 
    // skasowac ten katalog i na nowo go tworzyc
}
#endif    

#ifdef PLATFORM_MACOSX
extern "C" const char * CreateDirectoryIfNotExists(const char *dname);
const std::string & FileUtils::GetUserDir()
{
    return CreateDirectoryIfNotExists((std::string("~/Library/WarlocksGauntlet")+CGameOptions::mModDir).c_str());
}
#endif

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <shlobj.h>
static std::string gUserDir = "%APPDATA%\\WarlocksGauntlet\\";
static bool gInitialized = false;
const std::string & FileUtils::GetUserDir()
{
    if (!gInitialized) {
		char path[ MAX_PATH ];
		if (SHGetFolderPathA( NULL, CSIDL_APPDATA, NULL, 0, path ) != S_OK) {
			fprintf(stderr, "I could not retrieve the user's application data directory!\n");
			exit(-1);
		}
		gUserDir = std::string(path) + "\\WarlocksGauntlet";
        fprintf(stderr, "Detected userDir as `%s'\n", gUserDir.c_str());

        gInitialized = true;
    }
    return gUserDir;
}
void FileUtils::InitializeUserDir()
{
    fprintf(stderr, "Initializing user dir...\n");
	CreateDirectoryA(GetUserDir().c_str(), NULL);
    //i tutaj reszta inicjalizacji - kopiowanie 'first_game', 'config.xml' itepe itede
    // mozna tez pomyslec o tym, aby przy uruchamianu WarlocksGauntlet.exe --clean-user-dir 
    // skasowac ten katalog i na nowo go tworzyc
}
#endif



/*
* A function to list all contents of a given directory
* author: Danny Battison
* contact: gabehabe@hotmail.com
*/ 

/*#include <dirent.h>

void FileUtils::listdir (const char *path)
{
    DIR *pdir = NULL; // remember, it's good practice to initialise a pointer to NULL!
    pdir = opendir (path); // "." will refer to the current directory
    struct dirent *pent = NULL;
    if (pdir == NULL) // if pdir wasn't initialised correctly
    { // print an error message and exit the program
        printf ("\nERROR! pdir could not be initialised correctly");
        return; // exit the function
    }
    while (pent = readdir (pdir)){
        if (pent == NULL) // if pent has not been initialised correctly
        { // print an error message, and exit the program
            printf ("\nERROR! pent could not be initialised correctly");
            return; // exit the function
        }
        // otherwise, it was initialised correctly. let's print it on the console:
        printf ("%s\n", pent->d_name);
    }
    // finally, let's close the directory
    closedir (pdir);
}
*/
