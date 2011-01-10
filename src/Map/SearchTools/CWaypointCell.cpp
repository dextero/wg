#include "CWaypointCell.h"
#include "../CMapManager.h"
#include "../CCollisionMap.h"

CWaypointCell::CWaypointCell(const sf::Vector2i & cell) :
    CWaypoint(), mCell(cell)
{}

CWaypointCell::CWaypointCell(const CWaypointCell & other) :
    CWaypoint(other), mCell(other.GetCell())
{}

CWaypointCell::~CWaypointCell()
{}

int CWaypointCell::GetHash() const
{
    return mCell.y * 1024 + mCell.x;    // TODO uzaleznic to od rozmiarow mapy
}

std::vector< CWaypoint * > CWaypointCell::GetNeighbours() const
{
    std::vector< CWaypoint * > result;  // TODO uwzglednic wielkosc potwora

    CheckNeighbour(result, mCell - sf::Vector2i( 1,  1));
    CheckNeighbour(result, mCell - sf::Vector2i( 1,  0));
    CheckNeighbour(result, mCell - sf::Vector2i( 1, -1));
    CheckNeighbour(result, mCell - sf::Vector2i( 0, -1));
    CheckNeighbour(result, mCell - sf::Vector2i(-1, -1));
    CheckNeighbour(result, mCell - sf::Vector2i(-1,  0));
    CheckNeighbour(result, mCell - sf::Vector2i(-1,  1));
    CheckNeighbour(result, mCell - sf::Vector2i( 0,  1));

    return result;
}

CWaypoint * CWaypointCell::Clone() const
{
    return new CWaypointCell(*this);
}

void CWaypointCell::CheckNeighbour(
    std::vector< CWaypoint * > & neighbours,
    const sf::Vector2i neighbour) const
{
    if (gMapManager.GetCollisionMap()->GetCollisionByCell(neighbour) == false)
    {
        neighbours.push_back(new CWaypointCell(neighbour));
    }
}