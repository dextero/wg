#include "CImageAtlas.h"
#include "../Utils/StringUtils.h"
#include <fstream>
#include "CResourceManager.h"
#include "../CGameOptions.h"

using namespace System::Resource;

std::string CImageAtlas::GetType()
{
	return "CImageAtlas";
}

bool CImageAtlas::Load(std::map<std::string,std::string>& argv)
{
	std::ifstream infile ( ( gGameOptions.GetModDir() + argv["name"] ).c_str() );
    if (!infile.good())
	{
		if ( infile.is_open() )
			infile.close();
		infile.open ( argv["name"].c_str() );
		if ( !infile.good() )
		{
			if (argv.count("verbose") == 0)
				fprintf( stderr, "error: CImageAtlas - unable to open file %s\n", argv["name"].c_str() );
		    return false;
		}
	}

    std::string line;

    sf::IntRect rect;

    while ( !infile.eof() )
    {
        getline (infile, line);
        line = StringUtils::TrimWhiteSpaces( line );

        if ( line.empty() ) continue;
        if ( line.find( '#' ) == 0 ) continue;
        if ( line.find( "//" ) == 0 ) continue;
        sscanf( line.c_str() ,"%d %d %d %d", 
            &(rect.Left), &(rect.Top), &(rect.Right), &(rect.Bottom)
            );
        rect.Right += rect.Left;
        rect.Bottom += rect.Top;
        mOffsets.push_back(rect);
    }

    return true;
}

void CImageAtlas::Drop()
{
	delete this;
}

