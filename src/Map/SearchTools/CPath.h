#ifndef __CPath_h__
#define __CPath_h__

#include "CWaypoint.h"
#include "../../Rendering/CDisplayable.h"
#include "../../Rendering/CDrawableManager.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

class CWaypointCell;

class CPath
{
public:
    CPath();
	CPath(const sf::Vector2i & size, const CWaypointCell * waypoint);
	~CPath();

    const sf::Vector2i & GetSize() const;
    const sf::Vector2i & GetWaypoint(int index) const;
    unsigned int Length() const;
    
    bool ShouldBeUpdated() const;
    const sf::Vector2i & GetNextWaypoint();

private:
    sf::Vector2i mSize;
    std::vector< sf::Vector2i > mPath;
    
    unsigned int mIndex;
};

#endif // __CPath_h__
