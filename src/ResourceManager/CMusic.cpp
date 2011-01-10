#include "CMusic.h"
#include "../Utils/StringUtils.h"
#include "../Utils/MathsFunc.h"
#include "../CGameOptions.h"
#include <string.h>
#include <cstdlib>

using namespace System::Resource;
using namespace StringUtils;

bool CMusic::Load(std::map<std::string,std::string>& argv)
{
	if ( mMusicStream.OpenFromFile ( gGameOptions.GetModDir() + argv["name"] ) // otwórz plik z moda
		|| mMusicStream.OpenFromFile( argv["name"] ) ) // jesli nie ma, to otwórz plik z data
	{
        if (argv.count("volume") == 0)
            argv["volume"] = "100";
        mMusicStream.SetVolume( Parse<float>( argv["volume"] ) * 100.0f );
		mMusicStream.SetLoop( argv["loop"] == "true" );

		return true;
	}
	
	return false;
}

std::string CMusic::GetType()
{
	return "sf::Music";
};

void CMusic::Drop()
{
	delete this;
}
