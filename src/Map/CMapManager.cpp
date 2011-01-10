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

template<> Map::CMapManager* CSingleton<Map::CMapManager>::msSingleton = 0;


namespace Map{

	void CMapManager::SetMapFromData(const void *data){
		CMapManager::NextMapData *d = (CMapManager::NextMapData*)data;
		gMapManager.SetMap(*(d->mNextMap), d->mLoadCompleteMap, *(d->mNextMapRegion));
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

	void CMapManager::ScheduleSetMap( const std::string *mapFile, bool loadCompleteMap, const std::string *region)
    {
#ifndef __EDITOR__
	    NextMapData *data = new NextMapData();
	    data->mNextMap = mapFile;
	    data->mNextMapRegion = region;
        data->mLoadCompleteMap = loadCompleteMap;

	    CGame::loadingRoutine setMap;
	    setMap.bind(this, &CMapManager::SetMapFromData);
        gGame.ScheduleLoadingRoutine(setMap, data, mHideLoadingScreen);
#else
        SetMap(*mapFile, true, *region);
#endif
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

			/*  uczy� map� nierenderowaln� TODO: zmieni�, mo�na pamieta� stan mapy
				a w tym momencie zwalnia� zasoby mapy */
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

#ifndef __I_AM_TOXIC__
		if ( m_map != NULL && m_map->GetFilename() == mapFile ){
			if ( region != "" )
			{
                gPlayerManager.MovePlayersToRegion(region);
            }
			return true;
		}
#endif

		SetCurrentMapAsVisited();

#ifdef __I_AM_TOXIC__
		gResourceManager.LoadMap( mapFile );
#endif
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

		std::ofstream file( filename.c_str() );

        if (file.is_open())
		{
			file << "<map>\n";
			file << "<version>" << Map::CURRENT_MAP_VERSION << "</version>\n";
			file << "<width>" << size.x << "</width>\n";
			file << "<height>" << size.y << "</height>\n";

			file << "<tiletype code=\"aa\" image=\"data/maps/themes/test/rgrass.png\"/>\n";

			// kafle
			file << "<tiles>";
			for (int i = 0; i < size.x*size.y; i++)
				file << "aa ";
			file << "</tiles>\n";

			file << "</map>\n";
		}
	}
}
