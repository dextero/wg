#include "CSound.h"
#include "../Audio/CAudioManager.h"
#include "../Utils/StringUtils.h"
#include "../VFS/vfs.h"
#include <cstdlib>

using namespace System::Resource;
using namespace StringUtils;

CSound::CSound()
: 	mLastStarted(-6000), 
	mMinStartPeriod(0.5)
{
}

std::string CSound::GetType()
{
	return "sf::Sound";
};

bool CSound::Load(std::map<std::string,std::string>& argv)
{
	/*if ( mSoundBuffer.LoadFromFile( argv["name"].c_str() ) )
	{
		if(argv["period"].size() > 0)
			mMinStartPeriod = Parse<float>( argv["period"] );
		mName = argv["name"];
		return true;
	}*/

    System::IO::VFSFile file = gVFS.GetFile(argv["name"].c_str());
    if(file.GetData() != NULL)
    {
        if (mSoundBuffer.LoadFromMemory(file.GetData(), file.GetSize()))
        {
            if(argv["period"].size() > 0)
                mMinStartPeriod = Parse<float>( argv["period"] );
            mName = argv["name"]; 
            return true;
        }
    }

	return false;
}

void CSound::Drop()
{
	delete this;
}

