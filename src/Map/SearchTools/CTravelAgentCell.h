#ifndef __CTravelAgentCell_h__
#define __CTravelAgentCell_h__

#include "CTravelAgent.h"

class CTravelAgentCell : public CTravelAgent
{
public:
    explicit CTravelAgentCell(int movementCost);
    virtual int GetTravelCost(const CWaypoint * a, const CWaypoint * b) const;
    virtual int GetMovementCost() const;
    virtual int GetVisitCost(const CWaypoint * a) const;

private:
    int mMovementCost;
};

#endif // __CTravelAgentCell_h__
