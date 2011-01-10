#include "CWaypointRegion.h"
#include "../CDynamicRegion.h"

CWaypointRegion::CWaypointRegion(CDynamicRegion * region) :
    CWaypoint(), mRegion(region)
{}

CWaypointRegion::CWaypointRegion(const CWaypointRegion & other) :
    CWaypoint(other), mRegion(other.GetRegion())
{}

CWaypointRegion::~CWaypointRegion()
{}

int CWaypointRegion::GetHash() const
{
    return mRegion->GetRegionID();
}

std::vector< CWaypoint * > CWaypointRegion::GetNeighbours() const
{
    std::vector< CWaypoint * > result;

    const DynamicRegionList neighbours = mRegion->GetNeighbours();
    for (DynamicRegionList::const_iterator it = neighbours.begin();
         it != neighbours.end(); ++it)
    {
        result.push_back(new CWaypointRegion(*it));
    }

    return result;
}

CWaypoint * CWaypointRegion::Clone() const
{
    return new CWaypointRegion(*this);
}