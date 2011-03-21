/* 
*	DamorK:
*	Klasa liczaca najkrotsze sciezki dla wielu zrodel a jednego celu
*	GetWaypoint - zwraca nastepny punkt kluczowy dla podanego physicala
*	pragnacego dostac sie do celu.
*/

#ifndef _SHORTEST_PATHS_
#define _SHORTEST_PATHS_

#include "../../IFrameListener.h"
#include "../../Map/CMapManager.h"
#include "../../Map/CCollisionMap.h"
#include "../../Utils/Maths.h"
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include <iostream>

class CActorAIData;

enum wayPointState
{
	WP_UNKNOWN =	 1,
	
	WP_LEFT =		 2,
	WP_RIGHT =		 4,
	WP_UP =			 8,
	WP_DOWN =		 16,
	WP_DESTINATION = 32
};

class CShortestPaths : public IFrameListener
{
private:
	std::wstring mPhysicalDest;
	sf::Vector2i mSize;
	sf::Vector2i mDestIndex;
	float mMaxDistance;
	float mPathWidth;
	float mTimeToUpdate;
	float mUpdateInterval;

	std::vector<char> mWayPointState;

	sf::Vector2f GetCoordsByAIData( CActorAIData * data );
	bool SetWayPoint( const sf::Vector2i& field, const sf::Vector2i& size, char wayPointState );

public:
	CShortestPaths();
	~CShortestPaths();

	void Initialize( const std::wstring& physicalDestination, float maxDistance, float pathWidth ); // string - dla bezpieczeñstwa
	void Update();
	virtual void FrameStarted( float secondsPassed );	
	bool GetNextWayPoint( CActorAIData * data, sf::Vector2f * out, bool ignoreWalls = false );
};

#endif

