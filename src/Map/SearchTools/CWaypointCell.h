#ifndef __CWaypointCell_h__
#define __CWaypointCell_h__

#include "CWaypoint.h"
#include <SFML/System/Vector2.hpp>

class CWaypointCell : public CWaypoint
{
public:
    explicit CWaypointCell(const sf::Vector2i & cell);
    CWaypointCell(const CWaypointCell & other);
    
    virtual ~CWaypointCell();

    virtual int GetHash() const;
    virtual std::vector< CWaypoint * > GetNeighbours() const;

    const sf::Vector2i & GetCell() const    { return mCell; }

    virtual CWaypoint * Clone() const;

private:
    void CheckNeighbour(std::vector< CWaypoint * > & neighbours, const sf::Vector2i neighbour) const;

    sf::Vector2i mCell;

};

#endif // __CWaypointCell_h__
