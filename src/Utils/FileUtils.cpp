#include "FileUtils.h"
#include <fstream>

#include <cstdio>
#include <boost/filesystem/operations.hpp>

int FileUtils::GetFileSize(const char* filename)
{
	std::ifstream f;
	f.open(filename, std::ios_base::binary | std::ios_base::in);
	if (!f.good() || f.eof() || !f.is_open()) { return 0; }
	f.seekg(0, std::ios_base::beg);
	std::ifstream::pos_type begin_pos = f.tellg();
	f.seekg(0, std::ios_base::end);
	return static_cast<int>(f.tellg() - begin_pos);
}

bool FileUtils::IsFileExists(const char* filename)
{
	std::ifstream file;
	file.open(filename);
	if(file.is_open())
	{
		file.close();
		return true;
	}
	file.close();
	return false;
}

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
