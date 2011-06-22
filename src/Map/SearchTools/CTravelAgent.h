#ifndef _TRAVEL_AGENT_
#define _TRAVEL_AGENT_

class CWaypoint;

class CTravelAgent
{
public:
    virtual int GetTravelCost(const CWaypoint * a, const CWaypoint * b) const = 0;
    virtual int GetMovementCost() const = 0;
    virtual int GetVisitCost(const CWaypoint * a) const = 0;
};

#endif // _TRAVEL_AGENT_
