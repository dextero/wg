#include "../Utils/StringUtils.h"
#include "../Utils/FileUtils.h"
#include "../CGameOptions.h"
#include "vfs.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <zlib/zlib.h>
#include <cstring>
#include <boost/filesystem/operations.hpp>

using namespace System::IO;

template<> VFSReader* CSingleton<VFSReader>::msSingleton = 0;

VFSReader::~VFSReader()
{
    for (std::map<unsigned long long, VFSFile*>::iterator it = mFiles.begin(); it != mFiles.end(); ++it)
        delete it->second;
}

void VFSReader::OpenFileSystem(const char* filename)
{
    if(FileUtils::FileExists(filename)){
        file = new std::ifstream();
        file->open(filename, std::ios::binary);
        file->read((char*)&header, sizeof(header));
        file_table = new VFSFileTableEntry[header.mFileCount];
        file->read((char*)file_table, sizeof(VFSFileTableEntry) * header.mFileCount);
        ready = true;
        VFSFile filenames = GetFile("$$filenames$$");
        if (filenames.IsValid())
        {
            char* buffer = new char[filenames.GetSize()];
            memcpy(buffer, filenames.GetData(), filenames.GetSize());

            unsigned int len = strlen(buffer), idx = 0;
            char *reader = buffer;

            for (unsigned int i = 0; i < len; i++){
                if ((buffer[i]=='\t')||(buffer[i]=='\n')){
                    buffer[i]='\0';
                    if (idx % 2 == 1)
                        mFilenames.push_back(std::string(reader));
                    idx++;
                    reader = buffer + i + 1;
                }
            }
            delete[] buffer;

            fprintf(stderr,"Read %d files from VFS\n", (int)mFilenames.size());
            LoadAll();
        }
    } else {
        fprintf(stderr,"WARNING: VFS file %s not found. Continuing happily...\n",filename);
    }
}

const VFSFile VFSReader::GetFile(const char* filename)
{
	std::string filepath = CGameOptions::GetModDir() + filename;
	bool file_found;
	if (FileUtils::FileExists(filepath.c_str()))
		file_found = true;
	else if (FileUtils::FileExists(filename))
	{
		filepath = filename;
		file_found = true;
	}
	else
		file_found = false;

	if (file_found)
	{
		int size = FileUtils::GetFileSize(filepath.c_str());
		std::ifstream file;
		file.open(filepath.c_str(), std::ios::binary);

		if(!file.is_open())
		{
			file.close();
			return VFSFile(NULL, 0);
		}

		char* data = new char[size+1];
		file.read(data, size);
		data[size] = 0;

		file.close();

		return VFSFile(data, size);
	}
    else
    {
        unsigned long long hash = StringUtils::GetStringHash(filename);

        if (mFiles.find(hash) == mFiles.end() && ready)
        {
            for(unsigned int i=0; i<header.mFileCount; i++)
            {
                if(hash == file_table[i].mHash)
                {
                    // dane do VFSFile
                    char* buffer = 0;
                    int data_size;

                    data_size = file_table[i].mCompressedSize;
                    char* data = new char[file_table[i].mCompressedSize+1];

                    file->seekg(file_table[i].mOffset+header.mDataOffset, std::ios::beg);
                    file->read(data, file_table[i].mCompressedSize);
                    data[file_table[i].mCompressedSize] = 0;
                    buffer = data;

                    if(file_table[i].mFlags & 0x01)//compressed file
                    {
                        char* uncompressed_data = new char[file_table[i].mUnCompressedSize+1];
                        int uncompressed_size = file_table[i].mUnCompressedSize;
                        uncompress((Bytef*)uncompressed_data, (uLongf*)&uncompressed_size, (Bytef*)data, (uLongf)file_table[i].mCompressedSize);
                        uncompressed_data[file_table[i].mUnCompressedSize] = 0;

                        buffer = uncompressed_data;
                        data_size = file_table[i].mUnCompressedSize;

                        delete[] data;
                    }

                    mFiles.insert(std::pair<unsigned long long, VFSFile*>(hash, new VFSFile(buffer, data_size)));
                }
            }
        }

        if (mFiles.find(hash) != mFiles.end())
            return *mFiles[hash];
        else
        {
            fprintf(stderr,"WARNING: file %s not found in files or VFS. Continuing happily...\n",filename);
            return VFSFile(NULL, 0);
        }
    }
}

void VFSReader::ExtractFiles( const std::string &dir )
{
	for ( unsigned i = 0; i < gVFS.GetFilenames().size(); i++ )
	{
		std::string	filepath = gVFS.GetFilenames()[i];
		std::string filename = filepath.substr( filepath.find_last_of("\\/") + 1 );

		const VFSFile& file = gVFS.GetFile( filepath.c_str() );
		const char*	buffer = file.GetData();
		int			size = file.GetSize();

		filepath = filepath.substr(0, filepath.find_last_of("\\/"));
		//src/VFS/vfs.cpp:132: warning: comparison between signed and unsigned integer expressions
        size_t at = -1;
        while ((at = filepath.find("\\", at + 1)) != std::string::npos)
            filepath.replace(at, 1, "/");

		if ( buffer )
		{
			filepath = dir + "/" + filepath;
			//src/VFS/vfs.cpp:139: warning: comparison between signed and unsigned integer expressions
            size_t at = -1;
            while ((at = filepath.find("/", at + 1)) != std::string::npos)
                if (!boost::filesystem::exists(filepath.substr(0, at)))
                    boost::filesystem::create_directory(filepath.substr(0, at));
            
			boost::filesystem::create_directory( filepath.c_str() );
			std::ofstream out( (filepath + "/" + filename).c_str(), std::ios::binary );
			
			if ( out.is_open() )
			{
				out.write( buffer, size );
				out.close();
			}
		}
	}
}



void VFSReader::LoadAll()
{
    for (size_t i = 0; i < mFilenames.size(); ++i)
        GetFile(mFilenames[i].c_str());
}

void VFSReader::Clear()
{
    mFilenames.clear();
    mFiles.clear();
    delete file_table;
    delete file;
    ready = false;
}

void VFSReader::SaveToFile(const char* filename)
{
    VFSFileTableEntry* file_table = new VFSFileTableEntry[mFilenames.size()];

    std::ofstream out(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    // rekord $$filenames$$
        std::stringstream ss;
        for (std::vector<std::string>::iterator it = mFilenames.begin(); it != mFilenames.end(); ++it)
            ss << StringUtils::GetStringHash(*it) << "\t" << it->c_str() << "\n";

        std::string filenamesRecord = ss.str();
        char* buf = new char[filenamesRecord.size()];

        memcpy(buf, filenamesRecord.c_str(), filenamesRecord.size());

        // jesli nie ma takiego pliku, to utworzy
        AddFile("$$filenames$$", buf, filenamesRecord.size());
    // koniec $$filenames$$

    int headerSize = sizeof(VFSHeader) + sizeof(VFSFileTableEntry) * mFilenames.size();
    out.seekp(headerSize);

    // pliki
    for (size_t i = 0; i < mFilenames.size(); ++i)
    {
        const VFSFile& f = GetFile(mFilenames[i].c_str());
        
        unsigned long bufSize, flags;
        char* buffer;

        bufSize = compressBound(f.GetSize()); 
        buffer = new char[bufSize];
        compress((unsigned char*)buffer, &bufSize, (unsigned char*)f.GetData(), f.GetSize());

        flags = 0x1; // skompresowane

        // tu sie tworzy file_table
        VFSFileTableEntry entry = { StringUtils::GetStringHash(mFilenames[i]), flags, bufSize, f.GetSize(), (int)(out.tellp()) - headerSize };
        file_table[i] = entry;

        out.write(buffer, bufSize);
        
        if (flags) // flags == 0 dla nieskompresowanych rekordow, ktorych nie wolno usuwac
            delete[] buffer;
    }

    // naglowek i file_table
    out.seekp(0, std::ios::beg);

    VFSHeader header = {
        "VFS",                      // sygnatura
        0x100,                      // wersja
        sizeof(VFSHeader),          // rozmiar naglowka
        sizeof(VFSFileTableEntry),  // rozmiar wpisu fileTable
        mFilenames.size(),          // ilosc plikow
        sizeof(VFSHeader),          // offset fileTable
        sizeof(VFSFileTableEntry) * mFilenames.size() + sizeof(VFSHeader)    // offset danych (od poczatku pliku)
    };

    out.write((char*)&header, sizeof(VFSHeader));
    out.write((char*)file_table, sizeof(VFSFileTableEntry) * mFilenames.size());
    delete[] file_table;

    out.close();
}

bool VFSReader::IsInVFS(const char* filename)
{
    for (std::vector<std::string>::iterator it = mFilenames.begin(); it != mFilenames.end(); ++it)
        if (*it == filename)
            return true;

    return false;
}


bool VFSReader::AddFile(const char* filename, char* buffer, int size)
{
    for (std::vector<std::string>::iterator it = mFilenames.begin(); it != mFilenames.end(); ++it)
        if (*it == filename)
        {
            if (!RemoveFile(filename))
                return false;
            break;
        }

    mFilenames.push_back(filename);

    // kopiowanie danych - VFSFile ma delete[] w destruktorze, nie chcemy kuku
    char* fileBuf = new char[size];
    memcpy(fileBuf, buffer, size);
    mFiles.insert(std::pair<unsigned long long, VFSFile*>(StringUtils::GetStringHash(filename), new VFSFile(fileBuf, size)));

    return true;
}

bool VFSReader::RemoveFile(const char* filename)
{
    // usuwa wpis w mFilenames i plik z mFiles
    for (std::vector<std::string>::iterator it = mFilenames.begin(); it != mFilenames.end(); ++it)
        if (*it == filename)
        {
            mFilenames.erase(it);
            std::map<unsigned long long, VFSFile*>::iterator it;
            if ((it = mFiles.find(StringUtils::GetStringHash(filename))) != mFiles.end())
            {
                delete it->second;
                mFiles.erase(StringUtils::GetStringHash(filename));
            }
            return true;
        }

    return false;
}
