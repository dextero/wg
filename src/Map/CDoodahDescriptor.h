#ifndef DOODAH_DESCRIPTOR_H
#define DOODAH_DESCRIPTOR_H

#include <string>

/* TODO: W razie czego mozna dorobic wspolny interfejs z CMapObjectDescriptor zeby ulatwic edycje 
   polozenia obiektu w edytorze. */

namespace Map
{
	class CDoodahDescriptor
	{
	public:
		std::string file;
        std::string anim;
		float x,y;
		float scale;
		float rotation;
		int zindex;
		int uniqueId;
        
		CDoodahDescriptor();
		void Create();
	};
};

#endif