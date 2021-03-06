#include "MapCommands.h"
#include "CCommands.h"
#include "../Console/CConsole.h"
#include "../Utils/StringUtils.h"
#include "../Map/CMapManager.h"
#include "../Map/CRandomMapGenerator.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Logic/Factory/CPhysicalTemplate.h"
#include "../Logic/CPhysicalManager.h"
#include "../Logic/MapObjects/CRegion.h"
#include "../Logic/CLogic.h"
#include "../Logic/CLair.h"
#include "../Logic/CEnemy.h"

#include <SFML/System/Clock.hpp>
#include <boost/filesystem.hpp>

class CPhysical;

using namespace Map;

void CommandNextMap(size_t argc, const std::vector<std::wstring> &argv);
void CommandLoadStartingMap(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetMapLevel(size_t argc, const std::vector<std::wstring> &argv);
void CommandRegisterMonsterAtLair(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetCurrentMapId(size_t argc, const std::vector<std::wstring> &argv);

 // na koncu musi byc {0,0,0}, bo sie wszystko ***
CCommands::SCommandPair MapCommands [] =
{
    {L"load-map"                        , "$MAN_LOAD_MAP"           , CommandLoadMap },
    {L"load-starting-map"               , "$MAN_LOAD_STARTING_MAP"  , CommandLoadStartingMap },
    {L"set-map-level"                   , "$MAN_SET_MAP_LEVEL"      , CommandSetMapLevel },
    {L"preload-map"                     , "$MAN_PRELOAD_MAP"        , CommandPreloadMap },
	{L"unload-map"                      , "$MAN_UNLOAD_MAP"         , CommandUnloadMap },
	{L"print-scene-stats"				, "$MAN_PRINT_SCENE_STATS"  , CommandPrintSceneStats},
    {L"create-wall"                     , "$MAN_CREATE_WALL"        , CommandCreateWall},
    {L"show-region"                     , "$MAN_SHOW_REGION"        , CommandShowRegion},
    {L"editor-help"                     , "$MAN_EDITOR_HELP"        , CommandEditorHelp},
    {L"save-map"                        , "$MAN_SAVE_MAP"           , CommandSaveMap},
	{L"save-empty-map"					, "$MAN_SAVE_EMPTY_MAP"     , CommandSaveEmptyMap},
    {L"delete-region"                   , "$MAN_DELETE_REGION"      , CommandDeleteRegion},
    {L"generate-random-map"             , "$MAN_GENERATE_RANDOM_MAP", CommandGenerateRandomMap},
    {L"generate-random-world"           , "$MAN_GENERATE_RANDOM_WORLD", CommandGenerateRandomWorld},
    {L"register-monster-at-lair"        , "$MAN_REGISTER_MONSTER_AT_LAIR", CommandRegisterMonsterAtLair},
    {L"set-current-map-id"              , "$MAN_SET_CURRENT_MAP_ID" , CommandSetCurrentMapId },
    {0,0,0}
};

void CommandUnloadMap(size_t argc, const std::vector<std::wstring> &argv)
{
	gMapManager.SetCurrentMapAsVisited();
	gConsole.Printf( L"Map with scene unloaded");
}

void CommandPrintSceneStats(size_t argc, const std::vector<std::wstring> &argv)
{
	int totalObjects = 0;
	int totalObjectsDraw = 0;

	gScene.SceneStats(totalObjectsDraw,totalObjects);

	gConsole.Printf( L"Scene stats:\n Total scene objects: %i\n Total draw objects: %i\n",totalObjects,totalObjectsDraw);
}

void CommandLoadMap(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 2)
    {
        gConsole.Printf( L"usage: %ls folder_name/file_name [region] [saveGame]" , argv[0].c_str() );
        gConsole.Printf( L"       or file_name [region] [saveGame] if you want to load it from data/maps" );
        return;
    }

    std::wstring mapName(argv[1]);
    if (mapName.find(L"/") == std::wstring::npos)
    {
        mapName.insert(0, L"data/maps/");
    }

    const bool loadCompleteMap = true;
    if (argc == 2) // zapisz
        gMapManager.ScheduleSetMap(StringUtils::ConvertToString(mapName), loadCompleteMap);
    else if (argc == 3) // zapisz
        gMapManager.ScheduleSetMap(StringUtils::ConvertToString(mapName),loadCompleteMap, StringUtils::ConvertToString(argv[2]));
    else // nie zapisuj
        gMapManager.SetMap(StringUtils::ConvertToString(mapName), loadCompleteMap, StringUtils::ConvertToString(argv[2]));
}

void CommandPreloadMap(size_t argc, const std::vector<std::wstring> &argv)
{
	std::string mapFileName;
	std::string region;
	bool hasRegion = false;
	bool hasSaveGame = false;

	// mega-hack to handle absolute file paths with spaces and non-ASCII characters. 
	// Assuming:
	// - map file name always ends with '.xml'
	// - map file name always is the first argument
	// we can perform the following algorithm
	// - find the first argument that contains '.xml'; let's call it argX
	// - merge all the arguments from arg1 to argX into one string, updating argc/argv values
	int argX = -1;
	std::wstring separator;
	for (unsigned int i = 1; i < argv.size(); i++){
		mapFileName += StringUtils::ConvertToString(separator + argv[i]);
		if (argv[i].find(L".xml") != std::wstring::npos){
			argX = i;
			if (i + 1 < argv.size()) {
				hasRegion = true;
				region = StringUtils::ConvertToString(argv[i + 1]);
			}
			if (i + 2 < argv.size()) {
				hasSaveGame = true;
			}
			break;
		}
		separator = L" ";
	}
	// normal procedure follows

    if ( argc < 2)
    {
        gConsole.Printf( L"usage: %ls folder_name/file_name [region] [saveGame]" , argv[0].c_str() );
        gConsole.Printf( L"       or file_name [region] [saveGame] if you want to load it from data/maps" );
        return;
    }

    if (mapFileName.find("/") == std::string::npos)
    {
        mapFileName.insert(0, "data/maps/");
    }

    const bool loadCompleteMap = false;
    if (!hasRegion && !hasSaveGame) // zapisz
        gMapManager.ScheduleSetMap(mapFileName, loadCompleteMap);
    else if (!hasSaveGame) // zapisz
        gMapManager.ScheduleSetMap(mapFileName, loadCompleteMap, region);
    else // nie zapisuj
        gMapManager.SetMap(mapFileName, loadCompleteMap, region);
}

void CommandCreateWall(size_t argc, const std::vector<std::wstring> &argv){
    std::wstring id = L"";
    std::string scheme = "";
    if ( argc >= 2 ){
        scheme = StringUtils::ConvertToString(argv[1]);
        if (argc >= 3)
            id =  argv[2];
    }else {
        gConsole.Printf( L"usage: %ls scheme id" , argv[0].c_str() );
        return;
    }

    CPhysicalTemplate *wallTemplate = gResourceManager.GetPhysicalTemplate(scheme);
    if (wallTemplate == NULL){
        gConsole.Printf(L"Couldn't find template %ls.\n",argv[1].c_str());
        return;
    }
    wallTemplate->Create(id);
}

void CommandShowRegion(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 2){
        gConsole.Printf( L"usage: %ls region_name [on|off]" , argv[0].c_str() );
        return;
    }

    bool on = true;
    if (argc > 2){
        if (argv[2] == L"on")
            on = true;
        else if (argv[2] == L"off")
            on = false;
        else {
            gConsole.Printf( L"usage: %ls region_name [on|off]" , argv[0].c_str() );
            return;
        }
    }

    CPhysical *p = gPhysicalManager.GetPhysicalById(argv[1]);
    if (p == NULL){
        gConsole.Printf( L"region %ls not found in physical manager" , argv[1].c_str() );
        return;
    }

    CDynamicRegion *r = dynamic_cast<CDynamicRegion*>(p);
    if (r == NULL){
        gConsole.Printf( L"region %ls found in physical manager, but is not a region!" , argv[1].c_str() );
        return;
    }

    r->SetDisplay(on);
}

void CommandEditorHelp(size_t argc, const std::vector<std::wstring> &argv){
    gConsole.Printf( L"load-map <map-name>");
    gConsole.Printf( L"save-map <map-name>");
	gConsole.Printf( L"save-empty-map <map-name> <width> <height>");
}

void CommandSaveMap(size_t argc, const std::vector<std::wstring> &argv){
    if ( argc < 2)
    {
        gConsole.Printf( L"usage: %ls folder_name/file_name" , argv[0].c_str() );
        return;
    }

    bool result = gMapManager.SaveMap(StringUtils::ConvertToString(argv[1]));
    if (!result)
		gConsole.Printf( L"Cannot save map: %ls",argv[1].c_str());
}

void CommandSaveEmptyMap(size_t argc, const std::vector<std::wstring> &argv){
	if ( argc < 4 )
	{
		gConsole.Printf( L"usage: %ls dir_name/file_name width height", argv[0].c_str() );
		return;
	}

	gMapManager.SaveEmptyMap(
		StringUtils::ConvertToString( argv[1] ), 
		sf::Vector2i( StringUtils::Parse<int>(argv[2]), StringUtils::Parse<int>(argv[3]) )
	);
}

void CommandDeleteRegion(size_t argc, const std::vector<std::wstring> &argv)
{    
    if ( argc < 2){
        gConsole.Printf( L"usage: %ls region_name" , argv[0].c_str() );
        return;
    }

    CPhysical *p = gPhysicalManager.GetPhysicalById(argv[1]);
    if (p == NULL){
        gConsole.Printf( L"region %ls not found in physical manager" , argv[1].c_str() );
        return;
    }

    CDynamicRegion *r = dynamic_cast<CDynamicRegion*>(p);
    if (r == NULL){
        gConsole.Printf( L"region %ls found in physical manager, but is not a region!" , argv[1].c_str() );
        return;
    }

    gPhysicalManager.DestroyPhysical(r);
}

void CommandGenerateRandomMap(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 6)
    {
        gConsole.Printf(L"usage: %ls filename partsSet sizeX sizeY obstaclesPercent [mapLevel monstersCount lairsCount lootsCount maxMonsters maxLivingMonsters]", argv[0].c_str());
        return;
    }

    SRandomMapDesc desc;
    desc.set = StringUtils::ConvertToString(argv[2]);
    desc.sizeX = StringUtils::Parse<unsigned short>(argv[3]);
    desc.sizeY = StringUtils::Parse<unsigned short>(argv[4]);
    desc.obstaclesAreaPercent = StringUtils::Parse<float>(argv[5]);

    // dex: tak, *bez* break;-ow // tox: o Ty brzydalu, tak bez breakow?
    switch (argc)
    {
    case 12:
        desc.maxLivingMonsters = StringUtils::Parse<unsigned int>(argv[11]);
    case 11:
        desc.maxMonsters = StringUtils::Parse<unsigned int>(argv[10]);
    case 10:
        desc.loots = StringUtils::Parse<unsigned int>(argv[9]);
    case 9:
        desc.lairs = StringUtils::Parse<unsigned short>(argv[8]);
    case 8:
        desc.monsters = StringUtils::Parse<unsigned short>(argv[7]);
    case 7:
        desc.level = StringUtils::Parse<unsigned int>(argv[6]);
    default:
        break;
    }

    bool result = gRandomMapGenerator.GenerateRandomMap(StringUtils::ConvertToString(argv[1]), desc);
    gConsole.Printf(L"Generating map %ls: %ls", argv[1].c_str(), (result ? L"OK!" : L"FAILED!"));
}

void CommandGenerateRandomWorld(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2)
    {
        gConsole.Printf(L"usage: %ls numNodes", argv[0].c_str());
        return;
    }

    sf::Clock clock;
    // dex: wiem, co robie... chyba. siedz cicho, kompilatorze
    const_cast<CWorldGraph&>(gMapManager.GetWorldGraph()).Generate(StringUtils::Parse<unsigned>(argv[1]));
    float graphTime = clock.GetElapsedTime();

    // odswiezanie mapy
    gLogic.GetGameScreens()->Hide(L"map");
    gLogic.GetGameScreens()->InitMap(true);

    float totalTime = clock.GetElapsedTime();
    gConsole.Printf(L"world generated: %fs (graph: %fs, map image: %fs)", totalTime, graphTime, totalTime - graphTime);
}

void CommandLoadStartingMap(size_t argc, const std::vector<std::wstring> &argv)
{
    gMapManager.LoadStartingMap();
}

void CommandSetMapLevel(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2) {
        gConsole.Printf(L"usage: %ls newLevel", argv[0].c_str());
        return;
    }
    int newLevel = StringUtils::Parse<int>(argv[1]);

    gMapManager.SetLevel(newLevel);
}

void CommandSetCurrentMapId(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2) {
        gConsole.Printf(L"usage: %ls mapId", argv[0].c_str());
        return;
    }
    gMapManager.SetCurrentMapId(StringUtils::ConvertToString(argv[1]));
}

void CommandRegisterMonsterAtLair(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 3) {
        gConsole.Printf(L"usage: %ls monsterIndex lairIndex", argv[0].c_str());
        return;
    }
    int monsterIndex = StringUtils::Parse<int>(argv[1]);
    int lairIndex = StringUtils::Parse<int>(argv[2]);
    CEnemy * monster;
    CLair * lair;
    const std::vector< CPhysical *>& physicals = gPhysicalManager.GetPhysicals();
    for (std::vector< CPhysical * >::const_iterator it = physicals.begin();
         it != physicals.end(); ++it)
    {
        switch ((*it)->GetCategory())
        {
        case PHYSICAL_MONSTER:
            if (monsterIndex-- == 0) {
                monster = (CEnemy*)(*it);
            }
            break;
        case PHYSICAL_LAIR:
            if (lairIndex-- == 0) {
                lair = (CLair*)(*it);
            }
        default:
            break;
        }
    }
    if ( !lair || !monster ) return;
    lair->RegisterMonsterAsSpawned(monster);	
}
