#ifndef __CDynamicRegion_h__
#define __CDynamicRegion_h__

#include <SFML/System/Vector2.hpp>
#include <fstream>
#include <vector>

class CDynamicRegion;
typedef std::vector< CDynamicRegion * > DynamicRegionList;

class CDynamicRegion
{
public:
	explicit CDynamicRegion(int regionID);

    int GetRegionID() const;
    void AddNeighbour(CDynamicRegion * region);
    void AddCell(const sf::Vector2i & cell);

    const sf::Vector2i & GetCenter() const;

    const DynamicRegionList & GetNeighbours() const;

    friend std::ostream & operator <<(std::ostream & out, const CDynamicRegion & region);

private:
    int mID;    
    DynamicRegionList mNeighbours;
    
    std::vector< sf::Vector2i > mCells;
    sf::Vector2i mCenter;
};

#endif // __CDynamicRegion_h__
