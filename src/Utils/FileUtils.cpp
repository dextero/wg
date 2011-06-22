#include "FileUtils.h"
#include <fstream>

#include <stdio.h>
#include <sys/stat.h>

size_t FileUtils::GetFileSize(const char* filename)
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

size_t FileUtils::GetFileSize(const std::string & filename)
{
	return GetFileSize(filename.c_str());
}

std::string FileUtils::ReadFromFile(const std::string & filename)
{
	FILE *file = fopen(filename.c_str(), "rb");
	if (!file) return "";
	
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
    char* buf = new char[size + 1];
    buf[size] = '\0';
    if (fread(buf, 1, size, file) != size) 
		fprintf(stderr, "Error while reading %s file\n", filename.c_str());
    fclose(file);

	std::string str(buf);
    delete[] buf;
	return str;
}

bool FileUtils::WriteToFile(const std::string & filename, const std::string & contents)
{
	FILE *file = fopen(filename.c_str(), "wb");
	if (!file) return false;
	if (fwrite(contents.c_str(), 1, contents.size(), file) != contents.size()) {
		fprintf(stderr, "Error while writing to %s file\n", filename.c_str());
		fclose(file);
		return false;
	}
    fclose(file);
	return true;
}

bool FileUtils::FileExists(const char* filename)
{
	struct stat stFileInfo; 
	return (0 == stat(filename, &stFileInfo)); 
}

bool FileUtils::FileExists(const std::string & filename)
{
    return FileExists(filename.c_str());
}

void FileUtils::CreateDir(const std::string & filename)
{
	return CreateDir(filename.c_str());
}
