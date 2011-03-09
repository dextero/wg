#ifndef __VFS_H__
#define __VFS_H__

#include "../Utils/CSingleton.h"

#include <string>
#include <vector>
#include <map>
#include <string.h>

#include <boost/cstdint.hpp>

#define gVFS System::IO::VFSReader::GetSingleton()

namespace System
{
	namespace IO
	{
		#define VFS_VERSION 0x001
        #pragma pack(1)

		struct VFSHeader
		{
			char mSig[4];
			boost::uint32_t mVersion;
			boost::uint32_t mHeaderSize;
			boost::uint32_t mFileTableEntrySize;
			boost::uint32_t mFileCount;
			boost::uint32_t mFileTableOffset;
			boost::uint32_t mDataOffset;
		};


		struct VFSFileTableEntry
		{
			boost::uint64_t mHash;
			boost::uint32_t mFlags;
			boost::uint32_t mCompressedSize;
			boost::uint32_t mUnCompressedSize;
			boost::uint32_t mOffset;
		};

        #pragma pack()

		class VFSFile
		{
		private:
			char* mData;
			boost::int32_t mSize;
		public:
			VFSFile(): mData(NULL), mSize(0) {};
            VFSFile(const VFSFile& f): mSize(f.mSize)
            {
                mData = new char[f.mSize];
                memcpy(mData, f.mData, f.mSize);
            };
            // nie kopiuje danych! nie usuwac ich, bo bedzie niedobrze
			VFSFile(char* data, int size): mData(data), mSize(size) {};
			~VFSFile() { delete[] mData; };
			const char* GetData() const {return mData;};
			int GetSize() const {return mSize;};
			void Drop() {if(mData){delete[] mData;mData=NULL;mSize=0;}}
            bool IsValid() const { return mData != NULL && mSize != 0; }
		};

		class VFSReader : public CSingleton< VFSReader >
		{
		private:
            bool ready;
			VFSHeader header;
			VFSFileTableEntry* file_table;
			std::ifstream* file;
            std::vector<std::string> mFilenames;
            std::map<boost::uint64_t, VFSFile*> mFiles;
		public:
            VFSReader(): ready(false) {OpenFileSystem("data.vfs");};
            ~VFSReader();
			void OpenFileSystem(const char* filename);
			const VFSFile GetFile(const char* filename);
            inline const std::vector<std::string> &GetFilenames(){ return mFilenames; }
			void ExtractFiles( const std::string& dir );

            void LoadAll();
            void Clear();
            void SaveToFile(const char* filename);
            bool IsInVFS(const char* filename);
            
            // po dodaniu/usunieciu plikow trzeba zapisac, bo zmiany zostana w pamieci
            // kopiuje buffer - zadbaj o jego usuniecie po wywolaniu; nadpisuje istniejacy plik.
            bool AddFile(const char* filename, char* buffer, boost::int32_t data_size);
            bool RemoveFile(const char* filename);
		};
	}
}
#endif//__VFS_H__
