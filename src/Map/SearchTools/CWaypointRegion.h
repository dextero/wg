#ifndef __CWaypointRegion_h__
#define __CWaypointRegion_h__

#include "CWaypoint.h"

class CDynamicRegion;

class CWaypointRegion : public CWaypoint
{
public:
	explicit CWaypointRegion(CDynamicRegion * region);
	CWaypointRegion(const CWaypointRegion & other);
	
	virtual ~CWaypointRegion();

    virtual int GetHash() const;
    virtual std::vector< CWaypoint * > GetNeighbours() const;

    virtual CWaypoint * Clone() const;
    
    CDynamicRegion * GetRegion() const    { return mRegion; }
        
private:
    CDynamicRegion * mRegion;

};

#endif // __CWaypointRegion_h__
