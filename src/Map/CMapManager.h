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

struct CWorldGraph;

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

        int mLevel; // ktora to juz plansza?
        const std::string mWorld;
        CWorldGraph * mWorldGraph; 

        struct DefferedMapData {
			std::string map;
			std::string region;
            bool loadCompleteMap;
		};

        DefferedMapData mDefferedMapData; //for scheduled map changing
        std::string mLastEntryRegion;   // #1179 - nazwa regionu, z ktorego weszlismy na plansze

        std::string mCurrentMapId; // id z worldGraphu
	public:

		CMapManager();
		virtual ~CMapManager();

		inline CMap * GetCurrent()						{ return m_map; }
		inline CQuadTreeSceneManager& GetSceneManager() { return *m_sceneManager; }
		inline CCollisionMap * GetCollisionMap()		{ return m_collisionMap; }
		inline float GetCurrentMapTimeElapsed()			{ return mCurrentMapTimeElapsed; }

		void ScheduleSetMap( const std::string & mapFile, bool loadCompleteMap, const std::string & region = "" );

		void SetCurrentMapAsVisited();
        bool SetMap( const std::string &mapFile, bool loadCompleteMap, const std::string &region = "");
        void SetDefferedMap();

        const CWorldGraph & GetWorldGraph();
        const std::string & GetCurrentMapId();
        void SetCurrentMapId(const std::string & mapId);

        void EnterMap(const std::string & mapId, const std::string & region);
        void LoadStartingMap();
        void SetLevel(int newLevel) { mLevel = newLevel; }
        int GetLevel() { return mLevel; }

		virtual void FrameStarted(float secondsPassed);
        virtual bool FramesDuringPause(EPauseVariant pv) { return pv == pvLogicOnly || pv == pvPauseMenu || pv == pvLoading; };

        bool SaveMap(const std::string &mapFile);
		void SaveEmptyMap(const std::string& filename, const sf::Vector2i& size);

        const std::string& GetWorld() { return mWorld; }
        const std::string GetWorldPath(); // zwraca sciezke do folderu, w ktorym sa mapy skladajace sie na 'swiat', z / na koncu
        const std::string GetWorldsDirPath(); // zwraca scieke do folderu zawierajacego "swiaty", z / na koncu

        const std::string& GetLastEntryRegion() { return mLastEntryRegion; }
	};
}
#endif /*__CMAPMANAGER_H__*/

