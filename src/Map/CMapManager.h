#ifndef __CMAPMANAGER_H__
#define __CMAPMANAGER_H__

/**
 * DamorK (6.06.09):
 * Mapy mo¿na ³adowaæ tylko przez gMapManager.SetMap( string ), która¿ to metoda 
 * automatycznie czyœci bie¿¹c¹ scenê i np. usuwa losow¹ iloœæ gniazd, ¿eby po ponownym 
 * wejœciu na mapê by³o na niej mniej powtorów.
 */

#include "../IFrameListener.h"
#include "CMap.h"
#include "../Utils/CSingleton.h"
#include "SceneManager/CQuadTreeSceneManager.h"
#include "CCollisionMap.h"
#include <string>
#include <vector>

#define gMapManager Map::CMapManager::GetSingleton()
#define gScene Map::CMapManager::GetSingleton().GetSceneManager()

namespace Map{

    // jesli czas ladowania mapy > to, to pokaz loading-screen
    const float LOADING_TOO_SLOW = 0.5f;

	class CMapManager : public CSingleton<CMapManager>, IFrameListener
	{
	private:
		CMap * m_map;
		CQuadTreeSceneManager * m_sceneManager;
		CCollisionMap * m_collisionMap;
		std::vector<CMap*> m_visitedMaps;

        float mCurrentMapTimeElapsed;

        bool mHideLoadingScreen;
	public:
		void SetMapFromData(const void *data);

		struct NextMapData{
			std::string mNextMap;
			std::string mNextMapRegion;
            bool mLoadCompleteMap;
		};

		CMapManager();
		virtual ~CMapManager();

		inline CMap * GetCurrent()						{ return m_map; }
		inline CQuadTreeSceneManager& GetSceneManager() { return *m_sceneManager; }
		inline CCollisionMap * GetCollisionMap()		{ return m_collisionMap; }
		inline float GetCurrentMapTimeElapsed()			{ return mCurrentMapTimeElapsed; }

		void ScheduleSetMap( const std::string & mapFile, bool loadCompleteMap, const std::string & region = "" );

		void SetCurrentMapAsVisited();
        bool SetMap( const std::string &mapFile, bool loadCompleteMap, const std::string &region = "");
        void NextMap();

		virtual void FrameStarted(float secondsPassed);
        virtual bool FramesDuringPause(EPauseVariant pv) { return pv == pvLogicOnly || pv == pvPauseMenu || pv == pvLoading; };

        bool SaveMap(const std::string &mapFile);
		void SaveEmptyMap(const std::string& filename, const sf::Vector2i& size);
	};
}
#endif /*__CMAPMANAGER_H__*/

