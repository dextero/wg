#include "CPath.h"
#include "CWaypointCell.h"
#include "../CMapManager.h"
#include <SFML/Graphics.hpp>
#ifdef PLATFORM_MACOSX
#include <gl.h>
#else
#include <GL/gl.h>
#endif


CPath::CPath() :
    mSize(1, 1),
    mIndex(0)
{
    gMapManager.GetCollisionMap()->AddPath(this); 
}

CPath::CPath(const sf::Vector2i & size, const CWaypointCell * waypoint) :
    mSize(size), 
    mIndex(0)
{
    while (waypoint != NULL)
    {    
        mPath.insert(mPath.begin(), waypoint->GetCell());
        waypoint = dynamic_cast< const CWaypointCell * >(waypoint->GetParent());
    }

    gMapManager.GetCollisionMap()->AddPath(this); 
}

CPath::~CPath()
{
    gMapManager.GetCollisionMap()->RemovePath(this);
}

const sf::Vector2i & CPath::GetSize() const
{
    return mSize;
}

const sf::Vector2i & CPath::GetWaypoint(int index) const
{
    return mPath.at(index);
}

unsigned int CPath::Length() const
{
    return (unsigned int)mPath.size();
}

bool CPath::ShouldBeUpdated() const
{
    return mIndex >= mPath.size() / 2;
}

const sf::Vector2i & CPath::GetNextWaypoint()
{
    return mPath.at(mIndex++);
}