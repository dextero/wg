#ifndef __CRANDOMMAPGENERATOR_H__
#define __CRANDOMMAPGENERATOR_H__

#include "../Utils/CSingleton.h"

#include <string>
#include <map>
#include <vector>
#include <sstream>

#include <SFML/System/Vector2.hpp>

class CLoot;

struct SRandomMapDesc
{
    std::string set;
    unsigned int sizeX, sizeY;
    float obstaclesAreaPercent;
    unsigned int lairs, monsters, loots;
    unsigned int maxLivingMonsters, maxMonsters;
    int level;
    std::string nextMap;
    float minMonsterDist;   // jak daleko potwory maja sie spawnowac od wejscia

    SRandomMapDesc(): sizeX(0), sizeY(0), obstaclesAreaPercent(0.f), lairs(0), monsters(0), loots(0),
        maxLivingMonsters(0), maxMonsters(0), level(0), nextMap("@RANDOM"), minMonsterDist(0.f) {}
};


class CRandomMapGenerator: public CSingleton<CRandomMapGenerator>
{
private:
    // pare flag, zeby nieruchome obiekty na siebie nie zachodzily
    static const unsigned int BLOCKED = 0;
    static const unsigned int FREE    = 1;
    static const unsigned int REGION  = 1 << 1;
    static const unsigned int LAIR    = 1 << 2;
    static const unsigned int LOOT    = 1 << 3;

    struct SPartSet
    {
        struct SDoodah
        {
            std::string file;
            bool isInForeground;

            SDoodah(std::string file, bool isInForeground): file(file), isInForeground(isInForeground) {}
        };

        std::vector<std::string> tiles;
        std::vector<SDoodah> doodahs;
    };

public:    
    struct SPhysical
    {
        std::string type;
        std::string file;
        int minLevel;
        int maxLevel;
        float frequency;

        SPhysical(
                std::string type = "",
                std::string file = "",
                int minLevel = 0,
                int maxLevel = 0,
                float frequency = 0.0f
                ) :            
            type(type), file(file), minLevel(minLevel), maxLevel(maxLevel), frequency(frequency) {}
    };
    typedef std::vector<SPhysical> PhysicalsVector;
private:

    std::map<std::string, SPartSet> mPartSets;
    PhysicalsVector mPhysicals;
    std::vector<std::string> mTileMasks;

    unsigned int** mCurrent;
    unsigned int mPassableLeft; // TODO: sprawdzic, do czego to sluzylo poza generowaniem tuneli
    SRandomMapDesc mDesc;
    std::stringstream mXmlText;
	std::string mTilesFolder;	// sciezka do folderu, gdzie maja ladowac generowane kafle
	sf::Vector2i mEntryPos;		// pozycja startu - uzywane do spawnowania potworow z dala od gracza

    // metody generowania tuneli
    bool GenerateTunnelsFromRandomCenter();
    bool GenerateTunnelsGraph();

    unsigned int mSpawnedChestsCount;

    float mSpawnWeaponProbability;
    unsigned int mSpawnedWeaponsCount;

    // BFS
    unsigned int DistanceDijkstra(sf::Vector2i start, sf::Vector2i end);

    // generowanie posrednich kafli
    bool GenerateIntermediateTile(const std::string& outFile, const std::string& topLeft, const std::string& topRight, const std::string& bottomLeft, const std::string& bottomRight, unsigned int tileMask);

    // samo generowanie pol pustych/nie do przejscia
    // stawianie invisible-walli: PlaceWalls()
    bool GenerateMap();

    bool PlaceTiles();
    bool PlaceWalls();      // invisible-walls
    bool PlaceRegions();    // entry & exit - koniecznie przed PlaceLairs/Monsters, bo inaczej potwory moga sie spawnowac na glowie gracza
    bool PlaceDoodahs();
    bool PlaceLairs();
    bool PlaceMonsters();
    bool PlaceLoots();

    void ReleaseCurrent();

    std::string GenerateNextLootTemplateFile(bool canBeObstacle = false, float additionalWeaponProbability = 0.0f);

public:
    CRandomMapGenerator();
    ~CRandomMapGenerator();

    bool LoadPartSets(const std::string& filename);

    bool GenerateRandomMap(const std::string& filename, const SRandomMapDesc& desc);

    std::string GetRandomWeaponFile(int level);

//  std::string GenerateNextFinding(bool isLootFromDyingMonster);

    // tworzy i zwraca nowa znajdzke - uzywane m.in. przez ginace potwory
    CLoot * GenerateNextLoot(float additionalWeaponProbability = 0.0f);

    // sortowanie potworow po levelu
    friend bool VectorCompareFunc(const SPhysical& first, const SPhysical& last);
};

#define gRandomMapGenerator CRandomMapGenerator::GetSingleton()

#endif // __CRANDOMMAPGENERATOR_H__
