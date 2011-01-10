#ifndef __CTravelAgentRegion_h__
#define __CTravelAgentRegion_h__

#include "CTravelAgent.h"

class CTravelAgentRegion : public CTravelAgent
{
public:
    virtual int GetTravelCost(const CWaypoint * a, const CWaypoint * b) const;
    virtual int GetMovementCost() const;
    virtual int GetVisitCost(const CWaypoint * a) const;
};

#endif // __CTravelAgentRegion_h__
