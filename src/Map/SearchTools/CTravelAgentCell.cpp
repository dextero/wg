#include "CTravelAgentCell.h"
#include "CWaypointCell.h"
#include "../CMapManager.h"
#include "../CCollisionMap.h"
#include "../../Utils/Maths.h"
#include <cassert>

CTravelAgentCell::CTravelAgentCell(int movementCost) :
    mMovementCost(movementCost)
{}

int
CTravelAgentCell::GetTravelCost(const CWaypoint * a, const CWaypoint * b) const
{
    const CWaypointCell * cellA = dynamic_cast<const CWaypointCell * >(a);
    const CWaypointCell * cellB = dynamic_cast<const CWaypointCell * >(b);
    assert(cellA != NULL && cellB != NULL);

    return Maths::ManhattanDistance(cellA->GetCell(), cellB->GetCell()) * mMovementCost;
}

int
CTravelAgentCell::GetMovementCost() const
{
    return mMovementCost;
}

int CTravelAgentCell::GetVisitCost( const CWaypoint * a ) const
{
    const CWaypointCell * cell = dynamic_cast< const CWaypointCell * >(a);
    assert(cell != NULL);

    return gMapManager.GetCollisionMap()->GetDensityByCell(cell->GetCell());
}
