#include "CTravelAgentRegion.h"
#include "CWaypointRegion.h"
#include "../CDynamicRegion.h"
#include "../../Utils/Maths.h"
#include <cassert>

int CTravelAgentRegion::GetTravelCost(const CWaypoint * a, const CWaypoint * b) const
{
    const CWaypointRegion * regionA = dynamic_cast<const CWaypointRegion * >(a);
    const CWaypointRegion * regionB = dynamic_cast<const CWaypointRegion * >(b);
    assert(regionA != NULL && regionB != NULL);

    return Maths::ManhatanLength(regionA->GetRegion()->GetCenter() - 
                                 regionB->GetRegion()->GetCenter());
}

int CTravelAgentRegion::GetMovementCost() const
{
    return 1;
}

int CTravelAgentRegion::GetVisitCost( const CWaypoint * a ) const
{
    return 1;
}
