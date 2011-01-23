#ifndef __FILEUTILS_H__
#define __FILEUTILS_H__

#include <string>

namespace FileUtils
{
	size_t GetFileSize(const char* filename);
	size_t GetFileSize(const std::string & filename);
	bool FileExists(const char* filename);
    bool FileExists(const std::string & filename);

	std::string ReadFromFile(const std::string & filename);
	bool WriteToFile(const std::string & filename, const std::string & contents);

    // zwraca katalog uzytkownika - zaleznie od platformy to moze byc 
    //   ~/.WarlocksGauntlet pod linuksem
	//   %APPDATA%/WarlocksGauntlet pod windowsem
	//   ~/Library/WarlocksGauntlet pod macOSem
    const std::string & GetUserDir();

	// nie moze byc CreateDirectory bo sie gryzie z windows.h
	void CreateDir(const char* filename);
	void CreateDir(const std::string & filename);
}

#endif//__FILEUTILS_H__

