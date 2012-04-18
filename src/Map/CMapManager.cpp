#include <cstdio>

#include "CMapManager.h"
#include "../Editor/CEditor.h"
#include "../CGame.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CCamera.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Logic/CPlayer.h"
#include "../Logic/CPlayerManager.h"
#include "../Input/CPlayerController.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "../Logic/CLogic.h"
#include "../Logic/Boss/CBossManager.h"
#include "../Logic/Factory/CLootTemplate.h"
#include "../Commands/CCommands.h"
#include "../CGameOptions.h"
#include "../Utils/HRTimer.h"
#include "../Utils/FileUtils.h"
#include "../Utils/CRand.h"
#include "CRandomMapGenerator.h"
#include "CWorldGraph.h"

#include <boost/filesystem.hpp>

template<> Map::CMapManager* CSingleton<Map::CMapManager>::msSingleton = 0;

static int lastMapLevelHack = -1;
std::string lastMapIdHack = "-1";

namespace Map{

	CMapManager::CMapManager() :
		m_map( NULL ),
		m_sceneManager( new CQuadTreeSceneManager() ),
		m_collisionMap( new CCollisionMap() ),
        mCurrentMapTimeElapsed( 0.0f ),
        mHideLoadingScreen( false ),
        mLevel(0),
        mWorld("current"),
        mWorldGraph(NULL)
	{
		fprintf( stderr, "CMapManager::CMapManager()\n");
		gGame.AddFrameListener(this);
		m_visitedMaps.clear();

        mWorldGraph = new CWorldGraph();
        if (FileUtils::FileExists(GetWorldPath() + "world-graph.xml"))
            mWorldGraph->LoadFromFile(GetWorldPath() + "world-graph.xml");
        else
        {
            mWorldGraph->Generate(25);
            mWorldGraph->SaveToFile(GetWorldPath() + "world-graph.xml");
        }

        if (!boost::filesystem::exists(mWorld))
            boost::filesystem::create_directories(mWorld);
	}

	CMapManager::~CMapManager()
	{
		fprintf( stderr, "CMapManager::~CMapManager()\n");

		if ( m_sceneManager )	delete m_sceneManager;
		if ( m_map )			gResourceManager.DropResource( m_map->GetFilename() );

		for ( unsigned i = 0; i < m_visitedMaps.size(); i++ )
			gResourceManager.DropResource( m_visitedMaps[i]->GetFilename() );

        delete mWorldGraph;
		m_visitedMaps.clear();
	}

	void CMapManager::ScheduleSetMap(const std::string & mapFile, bool loadCompleteMap, const std::string & region)
    {
        fprintf(stderr, "ScheduleSetMap(%s, %d, %s)\n", mapFile.c_str(), (int)loadCompleteMap, region.c_str());
        mDefferedMapData.map = mapFile;
	    mDefferedMapData.region = region;
        mDefferedMapData.loadCompleteMap = loadCompleteMap;

        gGame.ScheduleLoadingRoutine(mHideLoadingScreen);
    }

	void CMapManager::SetDefferedMap() {
        SetMap(mDefferedMapData.map, mDefferedMapData.loadCompleteMap, mDefferedMapData.region);
	}

	void CMapManager::SetCurrentMapAsVisited()
	{
		if ( m_map != NULL )
		{
            // zapisz stan mapy, jesli juz z niej wychodzimy
            std::string mapStateFile;
            if (lastMapIdHack == "-1") { // fallback
                mapStateFile = m_map->GetFilename();
            } else {
                mapStateFile = GetWorldPath() + lastMapIdHack + ".xml"; //hack hack
                lastMapIdHack = mCurrentMapId;
            }
            if (mapStateFile.size() > 10 && mapStateFile.substr(0, 10) == "data/maps/")
            {
                // zamien \ i / na _, jesli mapa jest 'statyczna' (#1164)
                mapStateFile = StringUtils::ReplaceAllOccurrences(
                    StringUtils::ReplaceAllOccurrences(mapStateFile, "\\", "_"),
                    "/", "_");

                mapStateFile = GetWorldPath() + mapStateFile;   // i dopisz na poczatku sciezke do userDir/$world
            }

            fprintf(stderr, "SaveMapStateToFile, file = %s, skill = %d\n", std::string(mapStateFile + ".console").c_str(), GetLevel());
            gLogic.SaveMapStateToFile(mapStateFile + ".console");

			gEditor.SetSelectedToErase( NULL );
			m_sceneManager->ClearScene();

			for ( unsigned i = 0; i < m_visitedMaps.size(); i++ )
			{
				if ( m_visitedMaps[i]->GetFilename() == m_map->GetFilename() ) break;
				if ( i == m_visitedMaps.size() - 1 ) m_visitedMaps.push_back( m_map );
			}

			/*  uczyñ mapê nierenderowaln¹ TODO: zmieniæ, mo¿na pamietaæ stan mapy
				a w tym momencie zwalniaæ zasoby mapy */
			sf::FloatRect zero(0.0f,0.0f,0.0f,0.0f);
			m_map->CullVisibility(zero);
			m_map = NULL;
		}
	}

    bool CMapManager::SetMap(const std::string& mapFile, bool loadCompleteMap, const std::string &region)
	{
        // mierzenie czasu zaladowania
        sf::Clock timer;
        timer.Reset();

        // #1179 - nazwa regionu, z ktorego weszlismy na plansze   
        if (region != "")
            mLastEntryRegion = region;
        else
            mLastEntryRegion = "entry";

		if ( m_map != NULL && m_map->GetFilename() == mapFile ){
			if ( region != "" )
			{
                gPlayerManager.MovePlayersToRegion(region);
            }

			return true;
		}

        if (lastMapLevelHack != -1) { //zeby sie zapisal poprawny level w odwiedzonej mapie
            int currMapLevel = mLevel;
            mLevel = lastMapLevelHack;
    		SetCurrentMapAsVisited();
            mLevel = currMapLevel;
            lastMapLevelHack = -1;
        } else {
    		SetCurrentMapAsVisited();
        }

        // bo segfault przy ominieciu bossa..
        gBossManager.ClearData();
        // bo problemy ze sterowaniem point&click..
        for (unsigned int i = 0; i < gPlayerManager.GetPlayerCount(); ++i)
        {
            // "po prostu przestan isc"
            if (CPlayer* player = gPlayerManager.GetPlayerByNumber(i))
                player->GetController()->SetWalkTarget(false, sf::Vector2f(), true);
        }

        std::string mapStateFile = mapFile + ".console";
        if (mapStateFile.size() > 10 && mapStateFile.substr(0, 10) == "data/maps/")
        {
            // zamien \ i / na _, jesli mapa jest 'statyczna' (#1164)
            mapStateFile = StringUtils::ReplaceAllOccurrences(
                StringUtils::ReplaceAllOccurrences(mapStateFile, "\\", "_"),
                "/", "_");        

            mapStateFile = GetWorldPath() + mapStateFile;   // i dopisz na poczatku sciezke do userDir/$world
        }

        // jesli istnieje plik z zapisanym stanem mapy, to nie laduj calej mapy, tylko przywroc to co bylo
        bool mapStateFileExists = FileUtils::FileExists(mapStateFile);
        loadCompleteMap = loadCompleteMap && !mapStateFileExists;

        // fix na #1177 - duplikaty przedmiotow po powrocie na mape
        if (mapStateFileExists)
            gResourceManager.DropResource(mapFile);

		m_map = gResourceManager.GetMap( mapFile );
		if ( m_map )
		{
			fprintf( stderr, "mapFile = %s\n", mapFile.c_str() );
			fprintf( stderr, "m_CurrentMap->GetFilename() = %s\n", m_map->GetFilename().c_str() );
			m_sceneManager->Initialize( m_map->GetSize() );
            m_map->RespawnMapObjects(loadCompleteMap);
			m_map->RespawnDoodahs();
            gDrawableManager.DestroyAllLights();
			m_map->RespawnLights();
			m_collisionMap->Initialize( m_map->GetSize() * 4 );

			for ( unsigned i = 0; i < gPlayerManager.GetPlayerCount(); i++ )
			{
				m_sceneManager->AddSceneNode( gPlayerManager.GetPlayerByOrder(i)->GetSceneNode() );
			}

            if ( region != "" )
			{
                gPlayerManager.MovePlayersToRegion(region);
				gCamera.Update(0.0f);
            }

            mCurrentMapTimeElapsed = 0.0f;

            // save przeniesione do CGame::MainLoopStep

            gLogic.GetGameScreens()->ResetCompass();
            gLogic.GetGameScreens()->Show(L"hud");

            // odpal skrypt zawierajacy zapisany stan mapy, jesli taki istnieje (i jest taka potrzeba)
            if (!loadCompleteMap && mapStateFileExists)
                gCommands.ParseCommand(L"exec " + StringUtils::ConvertToWString(mapStateFile));
			
			if(mapFile.find("town") == std::string::npos)
			{
				gCommands.ParseCommand(L"load-playlist data/music/testpl.xml");
			}
			else
			{
				gCommands.ParseCommand(L"load-playlist data/music/town.xml");
			}

            // jak stoimy z czasem? oplaca sie pokazywac loading screena czy nie?
            float time = timer.GetElapsedTime();
            fprintf(stderr, "Map %s loaded in %.4fs, will %s loading screen\n", mapFile.c_str(), time, (time < LOADING_TOO_SLOW ? "hide" : "show"));
            mHideLoadingScreen = (time < LOADING_TOO_SLOW);

            gDrawableManager.SetLighting(true);
            gDrawableManager.SetAmbient(sf::Color(100, 100, 100));
            gDrawableManager.SetNormalMappingAmbient(sf::Color(150, 150, 150));
            gDrawableManager.SetNormalMappingContrast(1.3f);

			// Daj graczom nieœmiertelnoœæ na 3 sekundy
			for(int i = 0; i <= (signed)gPlayerManager.GetPlayerCount(); i++)
			{
				CPlayer * player = gPlayerManager.GetPlayerByNumber(i);
				if(player != NULL)
				{
					CLootTemplate* templ = dynamic_cast<CLootTemplate*>(gResourceManager.GetPhysicalTemplate("data/loots/short-immortality.xml"));
					if (templ == NULL) {
						gConsole.Printf(L"Couldn't find template %ls.\n","short-immortality");
					}
					else
					{
						CLoot * loot = templ->Create();
						if (loot) {
							loot->SetPosition(sf::Vector2f(player->GetPosition().x, player->GetPosition().y));
							loot->SetLevel(mLevel);
                            loot->HandleCollision(player);
						}
					}
				}
			}

			return true;
		}

        timer.GetElapsedTime();
        gConsole.Printf(L"Cannot load map: %s\n", mapFile.c_str());

		return false;
	}

	void CMapManager::FrameStarted(float secondsPassed)
	{
		sf::FloatRect cameraRect
		(
			gGame.GetRenderWindow()->GetDefaultView().GetRect().Left / Map::TILE_SIZE,
			gGame.GetRenderWindow()->GetDefaultView().GetRect().Top / Map::TILE_SIZE,
			gGame.GetRenderWindow()->GetDefaultView().GetRect().Right / Map::TILE_SIZE,
			gGame.GetRenderWindow()->GetDefaultView().GetRect().Bottom / Map::TILE_SIZE
		);

		m_sceneManager->UpdateScene();
		m_sceneManager->CullVisibility(cameraRect);

		if ( m_map )
		{
			m_map->CullVisibility(cameraRect);
		}

        /*if (mNextMap != NULL)
		{
            SetMap(*mNextMap,*mNextMapRegion);
            mNextMap = NULL;
            mNextMapRegion = NULL;
        }*/

        // TODO: potencjalny bug: jesli gra bedzie zafreezowana, ten czas nadal leci
        mCurrentMapTimeElapsed += secondsPassed;
	}

    bool CMapManager::SaveMap(const std::string &mapFile){
        if (m_map == NULL)
            return false;
        else
            return m_map->Save(mapFile);
    }

	void CMapManager::SaveEmptyMap(const std::string& filename, const sf::Vector2i& size){

		std::stringstream ss;
		ss << "<map>\n";
		ss << "<version>" << Map::CURRENT_MAP_VERSION << "</version>\n";
		ss << "<width>" << size.x << "</width>\n";
		ss << "<height>" << size.y << "</height>\n";

		ss << "<tiletype code=\"aa\" image=\"data/maps/themes/test/rgrass.png\"/>\n";

		// kafle
		ss << "<tiles>";
		for (int i = 0; i < size.x*size.y; i++)
			ss << "aa ";
		ss << "</tiles>\n";

		ss << "</map>\n";
		
		FileUtils::WriteToFile(filename, ss.str().c_str());
	}

    void CMapManager::LoadStartingMap() {
        const std::string & map = mWorldGraph->startingMap;
        const std::string & region = mWorldGraph->startingRegion;
        fprintf(stderr, "LoadingStartingMap: %s %s\n", map.c_str(), region.c_str());
        EnterMap(map, region.empty() ? "entry" : region);
    }

    const CWorldGraph & CMapManager::GetWorldGraph()
    {
        assert(mWorldGraph != NULL);
        return (*mWorldGraph);
    }

    const std::string & CMapManager::GetCurrentMapId()
    {
        return mCurrentMapId;
    }

    void CMapManager::SetCurrentMapId(const std::string & mapId)
    {
        mCurrentMapId = mapId;
        lastMapIdHack = mCurrentMapId;
    }

    void CMapManager::EnterMap(const std::string & mapId, const std::string & region) {
        if (FileUtils::FileExists(mapId)) { //i.e. mapId = "data/maps/town.xml"
            mCurrentMapId = mapId;
            ScheduleSetMap(mapId, true, region);
            return;
        }
        std::string realFilename = GetWorldPath() + mapId + ".xml";
        if (FileUtils::FileExists(realFilename)) {
            mCurrentMapId = mapId;
            ScheduleSetMap(realFilename, true, region.empty() ? "entry" : region);
            return;
        }
        
        //generate it!
        if (mWorldGraph->maps.find(mapId) == mWorldGraph->maps.end()) {
            fprintf(stderr, "Error, EnterMap: can't find mapId=%s\n", mapId.c_str());
            return;
        }
        mCurrentMapId = mapId;
        const CWorldGraphMap & mapDef = mWorldGraph->maps.find(mapId)->second;
        lastMapLevelHack = mLevel;
        mLevel = mapDef.level;

        SRandomMapDesc desc;
		desc.set = mapDef.scheme;
        desc.exits = mapDef.exits;
        desc.sizeX = 32;
        desc.sizeY = 32;
        desc.obstaclesAreaPercent = (float)gRand.Rnd(40, 60);
        desc.maxLivingMonsters = gRand.Rnd(40, 60);
        desc.maxMonsters = gRand.Rnd(30, 60);
        desc.monsters = gRand.Rnd(15, 30);
        desc.lairs = gRand.Rnd(0, 2);
        desc.loots = gRand.Rnd(4, 8);
        desc.level = mLevel;
        desc.minMonsterDist = 10.f;
        desc.narrowPathsPercent = (float)gRand.Rnd(40, 60);
        
        if (mapDef.level == CWorldGraphMap::MAP_LEVEL_CITY)
            desc.mapType = SRandomMapDesc::MAP_CITY;
        else if (mapDef.final)
            desc.mapType = SRandomMapDesc::MAP_FINAL_BOSS;
        else if (mapDef.boss.empty())
            desc.mapType = SRandomMapDesc::MAP_NORMAL;
        else
            desc.mapType = SRandomMapDesc::MAP_FINAL_BOSS;

        bool result = gRandomMapGenerator.GenerateRandomMap(realFilename, desc);
        fprintf(stderr, "Generating map %s: %s", realFilename.c_str(), (result ? "OK!" : "FAILED!"));

		gResourceManager.DropResource(realFilename); // forces reload of the map from hard drive
        ScheduleSetMap(realFilename, true, region);
    }

    const std::string CMapManager::GetWorldPath()
    {
        return gGameOptions.GetUserDir() + "/world/" + mWorld + "/";
    }

    const std::string CMapManager::GetWorldsDirPath()
    {
        return gGameOptions.GetUserDir() + "/world/";
    }

    void CMapManager::SetLevel(int newLevel) {
        mLevel = newLevel;
        gRandomMapGenerator.SetGeneratedLootsLevel(newLevel);
    }

    int CMapManager::GetLevel() {
        return mLevel;
    }
}
