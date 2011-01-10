#include "PhysicalCategories.h"
#include "../Utils/MathsFunc.h"
#include "../Utils/StringUtils.h"
#include <string>
#include <cstdio>

const int Categories[] = {
    PHYSICAL_DETECTOR, PHYSICAL_PLAYER,
    PHYSICAL_MONSTER, PHYSICAL_NPC, PHYSICAL_BULLET,
    PHYSICAL_LAIR, PHYSICAL_WALL,
    PHYSICAL_ITEM, PHYSICAL_DOOR,
    PHYSICAL_TRAP, PHYSICAL_REGION, PHYSICAL_OBSTACLE, PHYSICAL_HOOK
};

const int CategoryCount = sizeof(Categories)/sizeof(int);

const std::string CategoryNames[] = {
    "detector", "player", "monster", "npc", "bullet", "lair",
    "wall", "item", "door", "trap", "region", "obstacle", "hook"
};

const int Filters[] = {
    PHYSICAL_HOSTILES, PHYSICAL_FRIENDLY,
    PHYSICAL_ACTORS, PHYSICAL_STATIC,
    PHYSICAL_MOVING, PHYSICAL_NONMOVING,
	PHYSICAL_PATHBLOCKER,
    PHYSICAL_DESTRUCTIBLE,
	PHYSICAL_NONE
};

const int FilterCount = sizeof(Filters)/sizeof(int);

const std::string FilterNames[] = {
    "hostiles","friendly",
    "actors","static",
    "moving","nonmoving",
	"pathblocker",
    "destructible",
	"none"
};

int ParsePhysicalFilter(const std::string &input){
    std::vector< std::string > filters = StringUtils::Tokenize( input );
    int ret = 0;

    for ( std::vector< std::string>::const_iterator it = filters.begin() ; it != filters.end() ; it++ )
    {
        bool recognized = false;
        const std::string & filter = *it;
        for (int i = 0; i < CategoryCount; i++)
            if (CategoryNames[i]==filter)
            {
                ret = ret | Categories[i];
                recognized = true;
                break;
            }
        for (int i = 0; i < FilterCount; i++)
            if (FilterNames[i]==filter)
            {
                ret = ret | Filters[i];
                recognized = true;
                break;
            }

        if (!recognized)
            fprintf(stderr,"WARNING: unrecognised physical filter: %s\n",filter.c_str());
    }
    return ret;
}

std::string SerializePhysicalFilter(int filter){
    std::string result = "";
    for (int i = 0; i < FilterCount; i++){
        if (filter == 0) break;
        if (Filters[i] & filter){
            filter ^= Filters[i];
            if (result != "") result += ",";
            result += FilterNames[i];
        }
    }
    for (int i = 0; i < CategoryCount; i++){
        if (filter == 0) break;
        if (Categories[i] & filter){
            filter ^= Categories[i];
            if (result != "") result += ",";
            result += CategoryNames[i];
        }
    }
    return result;

}
