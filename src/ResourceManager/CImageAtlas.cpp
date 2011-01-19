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
	FILE *infile = fopen((gGameOptions.GetModDir() + argv["name"]).c_str(), "rb");
    if (!infile)
	{
		infile = fopen(argv["name"].c_str(), "rb");
		if (!infile)
		{
			if (argv.count("verbose") == 0)
				fprintf( stderr, "error: CImageAtlas - unable to open file %s\n", argv["name"].c_str() );
		    return false;
		}
	}

    std::string line;

    sf::IntRect rect;

	char buf[4096];
    while (!feof(infile))
    {
       	std::string line(fgets(buf, 4096, infile) != NULL ? buf : "");
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
	fclose(infile);

    return true;
}

void CImageAtlas::Drop()
{
	delete this;
}

