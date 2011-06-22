#ifndef __PHYSICALCATEGORIES_H__
#define __PHYSICALCATEGORIES_H__

// Liosan, 21.05.09
// Wydzielam kategorie physicali, dodaje rozbudowane filtry
// a takze parsowanie (np przy wczytywaniu z XMLi)

#include <string>

// dodajac nowy, trzeba uaktualnic .cpp!
enum physCategory			/*	Niech zostanie taka kolejnosc i zaleznosc, */
{							/*	ze obiekty poruszajace sie sa na poczatku */
	PHYSICAL_DETECTOR = 1,
	PHYSICAL_PLAYER = 2,
	PHYSICAL_MONSTER = 4,
	PHYSICAL_NPC = 8,
	PHYSICAL_BULLET = 16,
	PHYSICAL_LAIR = 32,
    PHYSICAL_WALL = 64,
    PHYSICAL_LOOT = 128,
    PHYSICAL_DOOR = 256,
    PHYSICAL_TRAP = 512,
    PHYSICAL_REGION = 1024,
    PHYSICAL_OBSTACLE = 2048,
	PHYSICAL_HOOK = 4096
};

// dodajac nowa kategorie, uaktualnij to:
const int PHYSICAL_CATEGORIES_COUNT = 13;

// dodajac nowy, trzeba uaktualnic .cpp!
const int PHYSICAL_HOSTILES = PHYSICAL_MONSTER | PHYSICAL_LAIR;
const int PHYSICAL_FRIENDLY = PHYSICAL_PLAYER | PHYSICAL_NPC;
const int PHYSICAL_ACTORS = PHYSICAL_HOSTILES | PHYSICAL_FRIENDLY;
const int PHYSICAL_STATIC = PHYSICAL_BULLET | PHYSICAL_WALL | PHYSICAL_LOOT | PHYSICAL_DOOR | PHYSICAL_HOOK;

const int PHYSICAL_MOVING = PHYSICAL_DETECTOR | PHYSICAL_PLAYER | PHYSICAL_MONSTER | PHYSICAL_NPC | PHYSICAL_BULLET;
const int PHYSICAL_NONMOVING = PHYSICAL_LAIR | PHYSICAL_WALL | PHYSICAL_LOOT | PHYSICAL_DOOR | PHYSICAL_HOOK;
const int PHYSICAL_PATHBLOCKER = PHYSICAL_WALL | PHYSICAL_DOOR | PHYSICAL_OBSTACLE;
const int PHYSICAL_DESTRUCTIBLE = PHYSICAL_ACTORS | PHYSICAL_OBSTACLE;
const int PHYSICAL_NONE = 0;

const int PHYSICAL_SUMMONBLOCKER = PHYSICAL_ACTORS | PHYSICAL_PATHBLOCKER;

const int PHYSICAL_ANY = (1 << PHYSICAL_CATEGORIES_COUNT) - 1;

int ParsePhysicalFilter(const std::string &input);
std::string SerializePhysicalFilter(int filter);

#endif /* __PHYSICALCATEGORIES_H__ */
