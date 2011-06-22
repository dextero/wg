#ifndef __CAStarSearch_h__
#define __CAStarSearch_h__

#include "CWaypointPriorityQueue.h"
#include <map>

class CWaypoint;
class CTravelAgent;

class CAStarSearch
{
public:
	CAStarSearch();
	~CAStarSearch();

    CWaypoint * FindPath(const CWaypoint & start, const CWaypoint & goal,
                         const CTravelAgent & travelAgent);

private:

    void ProcessNeighbours(CWaypoint * current);
    void ProcessNeighbour(CWaypoint * current, CWaypoint * parent);

    CWaypoint * GetWaypointFromDatabase(const CWaypoint * waypoint);
    //void ClearDatabase(); // ponoc unused

    const CWaypoint * mGoal;
    const CTravelAgent * mTravelAgent;

    typedef std::map< int, CWaypoint * > Waypoints;
    typedef CWaypointPriorityQueue OpenList;

    Waypoints mWaypoints;
    OpenList mOpenList;
};

#endif // __CAStarSearch_h__
