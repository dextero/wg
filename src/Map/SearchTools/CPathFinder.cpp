#include "CAStarSearch.h"
#include "CPath.h"
#include "CPathFinder.h"
#include "CTravelAgentCell.h"
#include "CTravelAgentRegion.h"
#include "CWaypointCell.h"
#include "CWaypointRegion.h"
#include "../CCollisionMap.h"
#include "../CDynamicRegion.h"
#include "../CMapManager.h"

const unsigned int kLocalPathLength = 2;
const int kMovementCost = 50;

bool CPathFinder::FindPath(
    const sf::Vector2f & start,
    const sf::Vector2f & goal,
    const sf::Vector2f & physicalSize,
    CPath ** path)
{
    *path = NULL;

    CCollisionMap * collisionMap = gMapManager.GetCollisionMap();
    sf::Vector2i startingCell = collisionMap->GetCellByCoords(start);
    sf::Vector2i finishingCell = collisionMap->GetCellByCoords(goal);
    CWaypointRegion startingRegion(collisionMap->GetRegionByCell(startingCell));
    CWaypointRegion finishingRegion(collisionMap->GetRegionByCell(finishingCell));
    sf::Vector2i size = collisionMap->ConvertSizeToCells(physicalSize);

    const CWaypoint * globalPath = FindPath(startingRegion, finishingRegion);
    if (globalPath != NULL)
    {
        std::vector< const CWaypoint * > reversedPath;
        while (globalPath != NULL)
        {
            reversedPath.insert(reversedPath.begin(), globalPath);
            globalPath = globalPath->GetParent();
        }

        if (reversedPath.size() >= kLocalPathLength)
        {
            CDynamicRegion * region =
                    dynamic_cast< const CWaypointRegion * >(reversedPath.at(kLocalPathLength - 1))->GetRegion();
            finishingCell = region->GetCenter();
        }

        CWaypoint * localPath =
            FindPath(CWaypointCell(startingCell), CWaypointCell(finishingCell));

        if (localPath != NULL)
            *path = new CPath(size, dynamic_cast< CWaypointCell * >(localPath));
    }

    return path != NULL;
}

CWaypoint * CPathFinder::FindPath(
    const CWaypointRegion & start,
    const CWaypointRegion & goal) const
{
    CAStarSearch searchEngine;
    return searchEngine.FindPath(start, goal, CTravelAgentRegion());
}

CWaypoint * CPathFinder::FindPath(
    const CWaypointCell & start,
    const CWaypointCell & goal) const
{
    CAStarSearch searchEngine;
    return searchEngine.FindPath(start, goal, CTravelAgentCell(kMovementCost));
}
