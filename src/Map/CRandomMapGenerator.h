#ifndef __CRANDOMMAPGENERATOR_H__
#define __CRANDOMMAPGENERATOR_H__

#include "../Utils/CSingleton.h"

#include <string>
#include <map>
#include <vector>
#include <sstream>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

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
    float narrowPathsPercent; // ile % sciezek bedzie waskich w "grafowych" mapach (2x2), reszta 3x3 [0.f - 100.f]
    enum EMapType {
        MAP_NORMAL,     // zwykla mapa, bez udziwnien
        MAP_BOSS,       // arena z bossem
        MAP_FINAL_BOSS  // finalowa arena z bossem - po jego zabiciu wyswietla sie ekran victory
    } mapType;

    SRandomMapDesc(): sizeX(0), sizeY(0), obstaclesAreaPercent(0.f), lairs(0), monsters(0), loots(0),
        maxLivingMonsters(0), maxMonsters(0), level(0), nextMap("@RANDOM"), minMonsterDist(0.f), mapType(MAP_NORMAL) {}
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
    static const unsigned int DOOR    = 1 << 4;
    static const unsigned int DOODAH  = 1 << 5; // doodahy lezace pod nogami, przez ktore mozna przejsc

    struct SPartSet
    {
        struct SDoodah
        {
            std::string file;
            bool isInForeground;
            float scale;

            SDoodah(std::string file, bool isInForeground, float scale = 1.0f): file(file), isInForeground(isInForeground), scale(scale) {}
        };

        std::vector<std::string> tiles;
        std::vector<SDoodah> doodahs;
        std::vector<SDoodah> doodahsOnGround; // doodahy lezace pod nogami, przez ktore mozna przejsc
		unsigned int minDoodahsOnGround, maxDoodahsOnGround;
		unsigned int fromLevel;
    };

public:    
    struct SPhysical
    {
        std::string type;
        std::string file;
        int minLevel;
        int maxLevel;
        float frequency;
        float bossTriggerRadius;

        std::string bossTriggerAI, bossPlaylist;
        SPhysical(
                std::string type = "",
                std::string file = "",
                int minLevel = 0,
                int maxLevel = 0,
                float frequency = 0.0f,
                float bossTriggerRadius = 0.f,
                std::string bossTriggerAI = "",
                std::string bossPlaylist = ""
                ) :            
            type(type), file(file), minLevel(minLevel), maxLevel(maxLevel), frequency(frequency),
            bossPlaylist(bossPlaylist), bossTriggerRadius(bossTriggerRadius), bossTriggerAI(bossTriggerAI) {}
    };
    typedef std::vector<SPhysical> PhysicalsVector;
private:

    std::map<std::string, SPartSet> mPartSets;
    PhysicalsVector mPhysicals;
    std::vector<std::string> mTileMasks;

    unsigned int** mCurrent;
    unsigned int mPassableLeft; // TODO: sprawdzic, do czego to sluzylo poza generowaniem tuneli
                                // dex: wszystkie funkcje stawiajace rozne rzeczy na mapie przed wzieciem sie do roboty
                                // sprawdzaja, czy jest jeszcze jakiekolwiek pole, na ktorym mozna cos postawic. inaczej
                                // losowanie wspolrzednych by sie zapetlilo na amen.
    SRandomMapDesc mDesc;
    std::stringstream mXmlText;
	std::string mTilesFolder;	// sciezka do folderu, gdzie maja ladowac generowane kafle
	sf::Vector2i mEntryPos;		// pozycja startu - uzywane do spawnowania potworow z dala od gracza

    // metody generowania tuneli
    bool GenerateTunnelsFromRandomCenter();
    bool GenerateTunnelsGraph();
    bool GenerateTunnelsBossArena();

    unsigned int mSpawnedChestsCount;

    float mSpawnWeaponProbability;
    unsigned int mSpawnedWeaponsCount;

    // generowanie pojedynczego prostego tunelu miedzy dwoma punktami - zwraca ilosc wykopanych pol
    unsigned int GenerateStraightTunnel(const sf::Vector2i& from, const sf::Vector2i& to, unsigned int tunnelSize);

    // generowanie "jaskini" o nieregularnych ksztaltach (wielokat); rectSize - "grubosc" prostokatow, w ktorych moga znalezc sie wierzcholki
    // patrz: http://roguebasin.roguelikedevelopment.org/index.php?title=Irregular_Shaped_Rooms
    unsigned int GenerateIrregularCave(const sf::IntRect& outsideRect, unsigned int rectSize);

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
                            // poniewaz PlaceRegions "kopie" przejscie naokolo teleportu, musi byc przed PlaceWalls
    bool PlaceDoodahs();
    bool PlaceBossDoors();  // drzwi otwierajace sie po zabiciu bossa. opcjonalne, ale jesli wywolywac, to najlepiej przed potworami/gniazdami/itemami
    bool PlaceLairs();
    bool PlaceMonsters();
    bool PlaceLoots();
    bool PlaceMiscEffects();    // inne efekty, niepowiazane z konkretnym obiektem - np. ekran victory po zabiciu bossa

    void ReleaseCurrent();

    std::string GenerateNextLootTemplateFile(bool canBeObstacle = false, float additionalWeaponProbability = 0.0f, const sf::Vector2f & position = sf::Vector2f(0,0));

public:
    CRandomMapGenerator();
    ~CRandomMapGenerator();

    bool LoadPartSets(const std::string& filename);

    bool GenerateRandomMap(const std::string& filename, const SRandomMapDesc& desc);

    std::string GetRandomWeaponFile(int level);

	std::string GetSetForLevel(unsigned int level);

//  std::string GenerateNextFinding(bool isLootFromDyingMonster);

    // tworzy i zwraca nowa znajdzke - uzywane m.in. przez ginace potwory
    CLoot * GenerateNextLoot(float additionalWeaponProbability = 0.0f, const sf::Vector2f & position = sf::Vector2f(0,0));

    // sortowanie potworow po levelu
    friend bool VectorCompareFunc(const SPhysical& first, const SPhysical& last);
};

#define gRandomMapGenerator CRandomMapGenerator::GetSingleton()

#endif // __CRANDOMMAPGENERATOR_H__
