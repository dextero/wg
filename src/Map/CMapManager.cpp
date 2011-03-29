#include <cstdio>

#include "CMapManager.h"
#include "../Editor/CEditor.h"
#include "../CGame.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CCamera.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Logic/CPlayer.h"
#include "../Logic/CPlayerManager.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "../Logic/CLogic.h"
#include "../Logic/Boss/CBossManager.h"
#include "../Commands/CCommands.h"
#include "../CGameOptions.h"
#include "../Utils/HRTimer.h"
#include "../Utils/FileUtils.h"
#include "../Utils/CRand.h"
#include "CRandomMapGenerator.h"

template<> Map::CMapManager* CSingleton<Map::CMapManager>::msSingleton = 0;


namespace Map{

	void CMapManager::SetMapFromData(const void *data){
		CMapManager::NextMapData *d = (CMapManager::NextMapData*)data;
		gMapManager.SetMap(d->mNextMap, d->mLoadCompleteMap, d->mNextMapRegion);
	}

	CMapManager::CMapManager() :
		m_map( NULL ),
		m_sceneManager( new CQuadTreeSceneManager() ),
		m_collisionMap( new CCollisionMap() ),
        mCurrentMapTimeElapsed( 0.0f ),
        mHideLoadingScreen( false )
	{
		fprintf( stderr, "CMapManager::CMapManager()\n");
		gGame.AddFrameListener(this);
		m_visitedMaps.clear();
	}

	CMapManager::~CMapManager()
	{
		fprintf( stderr, "CMapManager::~CMapManager()\n");

		if ( m_sceneManager )	delete m_sceneManager;
		if ( m_map )			gResourceManager.DropResource( m_map->GetFilename() );

		for ( unsigned i = 0; i < m_visitedMaps.size(); i++ )
			gResourceManager.DropResource( m_visitedMaps[i]->GetFilename() );

		m_visitedMaps.clear();
	}

	void CMapManager::ScheduleSetMap( const std::string & mapFile, bool loadCompleteMap, const std::string & region)
    {
        fprintf(stderr, "ScheduleSetMap(%s, %d, %s)\n", mapFile.c_str(), (int)loadCompleteMap, region.c_str());
	    NextMapData *data = new NextMapData(); //tox: kto kasuje potem data? dupa, nikt nie kasuje, dobrze widze?
	    data->mNextMap = mapFile;
	    data->mNextMapRegion = region;
        data->mLoadCompleteMap = loadCompleteMap;

	    CGame::loadingRoutine setMap;
	    setMap.bind(this, &CMapManager::SetMapFromData);
        gGame.ScheduleLoadingRoutine(setMap, data, mHideLoadingScreen);
    }

	void CMapManager::SetCurrentMapAsVisited()
	{
		if ( m_map != NULL )
		{
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

		if ( m_map != NULL && m_map->GetFilename() == mapFile ){
			if ( region != "" )
			{
                gPlayerManager.MovePlayersToRegion(region);
            }
			return true;
		}

		SetCurrentMapAsVisited();

        // bo segfault przy ominieciu bossa..
        gBossManager.ClearData();

		m_map = gResourceManager.GetMap( mapFile );
		if ( m_map )
		{
			fprintf( stderr, "mapFile = %s\n", mapFile.c_str() );
			fprintf( stderr, "m_CurrentMap->GetFilename() = %s\n", m_map->GetFilename().c_str() );
			m_sceneManager->Initialize( m_map->GetSize() );
            m_map->RespawnMapObjects(loadCompleteMap);
			m_map->RespawnDoodahs();
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

            // jak stoimy z czasem? oplaca sie pokazywac loading screena czy nie?
            float time = timer.GetElapsedTime();
            fprintf(stderr, "Map %s loaded in %.4fs, will %s loading screen\n", mapFile.c_str(), time, (time < LOADING_TOO_SLOW ? "hide" : "show"));
            mHideLoadingScreen = (time < LOADING_TOO_SLOW);

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

    void CMapManager::NextMap() {
        fprintf(stderr, "NextMap()\n");
        int r = gRand.Rnd(0, 100);
        // todo: robic w katalogu usera:
        std::string filename = "data/maps/rnd" + StringUtils::ToString(r) + ".xml";
        SRandomMapDesc desc;
        desc.set = "forest";
        desc.sizeX = 32;
        desc.sizeY = 32;
        desc.obstaclesAreaPercent = gRand.Rnd(40.0f,60.0f);
        desc.maxLivingMonsters = gRand.Rnd(40, 60);
        desc.maxMonsters = gRand.Rnd(30, 60);
        desc.monsters = gRand.Rnd(15, 30);
        desc.lairs = gRand.Rnd(0, 2);
        desc.loots = gRand.Rnd(3, 5);
        desc.level = 1; // todo: inkrementowac
        bool result = gRandomMapGenerator.GenerateRandomMap(filename, desc);
        fprintf(stderr, "Generating map %s: %s", filename.c_str(), (result ? "OK!" : "FAILED!"));

        ScheduleSetMap(filename, true, "entry");
    }
}
