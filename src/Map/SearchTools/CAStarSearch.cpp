#include "CAStarSearch.h"
#include "CTravelAgent.h"

CAStarSearch::CAStarSearch():
    mGoal(NULL),
    mTravelAgent(NULL)
{}

CAStarSearch::~CAStarSearch()
{
//     ClearDatabase();
}

CWaypoint * CAStarSearch::FindPath(
    const CWaypoint & start,
    const CWaypoint & goal,
    const CTravelAgent & travelAgent)
{
    mTravelAgent = &travelAgent;
    mGoal = &goal;

    CWaypoint * result = NULL;

    mOpenList.Push(start.Clone());
    while (!mOpenList.IsEmpty())
    {
        CWaypoint * nextWaypoint = mOpenList.Pop();
        
        if (*nextWaypoint == goal)
        {
            result = nextWaypoint;
            break;
        }
    
        ProcessNeighbours(nextWaypoint);
        nextWaypoint->SetOnClosed(true);
    }
    
//     ClearDatabase();
    return result;
//     *path = NULL;    
// 
//     mStartIndex = gMapManager.GetCollisionMap()->GetCellByCoords(start);
//     mGoalIndex = gMapManager.GetCollisionMap()->GetCellByCoords(goal);
//     mPhysicalSize = gMapManager.GetCollisionMap()->ConvertSizeToCells(physicalSize);
// 
//     int travelCost = mTravelAgent.GetTravelCost(mStartIndex, mGoalIndex);
//     mOpenWaypoints.Push(UpdateWaypoint(mStartIndex, 0, travelCost, NULL));
// 
//     while (!mOpenWaypoints.IsEmpty())
//     {
//         WaypointInt * nextWaypoint = mOpenWaypoints.Pop();
// 
//         if (mGoalIndex == nextWaypoint->GetIndex())
//         {
//             *path = new CPath(mPhysicalSize, nextWaypoint);
//             break;
//         }
// 
//         ProcessNeighbors(nextWaypoint);
//         nextWaypoint->SetOnClosed(true);
//     }
// 
//     time_t cleanStart = clock();
// 
//     ClearContainers();    
// 
//     return *path != NULL;

}

void CAStarSearch::ProcessNeighbours(CWaypoint * current)
{
    std::vector< CWaypoint * > neighbours = current->GetNeighbours();
    for (std::vector< CWaypoint * >::iterator it = neighbours.begin();
        it != neighbours.end(); ++it)
    {
        ProcessNeighbour(*it, current);
    }
    
    for (std::vector< CWaypoint * >::iterator it = neighbours.begin();
         it != neighbours.end(); ++it)
    {
        delete *it;
    }
}

void CAStarSearch::ProcessNeighbour(CWaypoint * current, CWaypoint * parent)
{
    // Ignorujemy punkty z ktorych przyszlismy
    if (parent->GetParent() && parent->GetParent() == current)
        return;

    int costFromStart = parent->GetCostFromStart() + 
                        mTravelAgent->GetVisitCost(current) +
                        mTravelAgent->GetTravelCost(parent, current);
    int costToGoal = mTravelAgent->GetTravelCost(current, mGoal);
         
    CWaypoint * waypoint = GetWaypointFromDatabase(current);
    if (waypoint == NULL)
    {        
        waypoint = current->Clone();
        
        waypoint->Update(costFromStart, costToGoal, parent);
        mOpenList.Push(waypoint);
        mWaypoints[waypoint->GetHash()] = waypoint;
    }
    else if (costFromStart < waypoint->GetCostFromStart())
    {
        waypoint->Update(costFromStart, costToGoal, parent);
        if (waypoint->IsOnOpen())
            mOpenList.Update(waypoint);
        else
        {
            waypoint->SetOnOpen(true);
            waypoint->SetOnClosed(false);
            mOpenList.Push(waypoint);
        }
    }

    

    //     int newCostFromStart = parentWaypoint->GetCostFromStart()
    //         + gMapManager.GetCollisionMap()->GetDensityByRect(newIndex, newIndex + mPhysicalSize)
    //         + mTravelAgent.GetTravelCost(newIndex, parentWaypoint->GetIndex());
    //     int newCostToGoal = mTravelAgent.GetTravelCost(newIndex, mGoalIndex);
    // 
}

CWaypoint * CAStarSearch::GetWaypointFromDatabase(const CWaypoint * waypoint)
{
    if (mWaypoints.count(waypoint->GetHash()) != 0)
        return mWaypoints[waypoint->GetHash()];
    
    return NULL;
}

// ponoc unused
/*void CAStarSearch::ClearDatabase()
{
    for (Waypoints::iterator it = mWaypoints.begin(); it != mWaypoints.end(); ++it)
    {
        delete it->second;
    }
    mWaypoints.clear();
}*/