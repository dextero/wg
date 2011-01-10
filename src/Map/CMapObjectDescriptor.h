#ifndef MAP_OBJECT_DESCRIPTOR_H
#define MAP_OBJECT_DESCRIPTOR_H

class CPhysicalTemplate;
class CTemplateParam;

#include <string>

namespace Map{

    class CMapObjectDescriptor
	{
    public:
        std::string code;
        std::string name;
        CPhysicalTemplate *templ;
        CTemplateParam *param;
        float x;
        float y;
        int rot;
		int uniqueId;
        std::string bossPlaylist;
        std::string bossAI;
        float bossRadius; // promien obszaru, w ktorym odpala sie boss-fight

		CMapObjectDescriptor();
        void Create();

        bool CoreMapObject() const;
    };

}
#endif
