#include "CFont.h"
#include "../Utils/StringUtils.h"
#include <math.h>
#include "../CGameOptions.h"

using namespace System::Resource;

std::string CFont::GetType()
{
	return "sf::Font";
};

bool CFont::Load(std::map<std::string,std::string>& argv)
{
	unsigned charsize = 30;
	size_t pos;

	/* Jesli w miejsce nazwy czcionki podamy 'verdana.ttf:12' to zostanie wczytana verdana o rozmiarze 12 */
	
	if ( (pos = argv["name"].find_last_of(':')) != std::string::npos )
	{
		StringUtils::FromString( argv["name"].substr(pos + 1), charsize );
		argv["name"] = argv["name"].substr( 0, pos );
	}
	else if ( argv.count("charsize") )
		StringUtils::FromString( argv["charsize"].substr(pos + 1), charsize );

	// wczytaj plik z moda
	if ( !LoadFromFile ( gGameOptions.GetModDir() + argv["name"], std::max<unsigned>(charsize, 1), StringUtils::ValidChars ) )
	{
		// jak siê nie uda, wczytaj z data
		if ( !LoadFromFile ( argv["name"], std::max<unsigned>(charsize, 1), StringUtils::ValidChars ) )
			return false;
	}

	return true;
}

void CFont::Drop()
{
	delete this;
}

