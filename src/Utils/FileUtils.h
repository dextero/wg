#ifndef __FILEUTILS_H__
#define __FILEUTILS_H__

#include <string>

namespace FileUtils
{
	int GetFileSize(const char* filename);
	bool FileExists(const char* filename);
    bool FileExists(const std::string & filename);

    // zwraca katalog uzytkownika - zaleznie od platformy to moze byc 
    // ~/.WarlocksGauntlet albo %APPDATA/costamcostam itepe
    const std::string & GetUserDir();

    // stwarza i wypelnia katalog usera domyslnymi plikami
    void InitializeUserDir();
}

#endif//__FILEUTILS_H__

