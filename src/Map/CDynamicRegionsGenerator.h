#ifndef __CDynamicRegionsGenerator_h__
#define __CDynamicRegionsGenerator_h__

#include "CCell.h"
#include "CDynamicRegion.h"
#include <SFML/System/Vector2.hpp>
#include <map>
#include <vector>

namespace Map
{

class CDynamicRegionsGenerator
{
public:
    explicit CDynamicRegionsGenerator(const sf::Vector2i & mapSize,
                                      const sf::Vector2i & regionSize);
    void GenerateRegions(std::vector< CCell > & cellList,
                         std::vector< CDynamicRegion * > & regionList);


private:
    typedef std::vector< sf::Vector2i > PointList;
    typedef std::map< int, PointList > OpenList;
    typedef std::multimap< int, int > LinkList;
    
    const sf::Vector2i FindNextStartingPoint() const;

    void GenerateLinks(int region, CDynamicRegion * newRegion);
    
    int PositionToRegion(const sf::Vector2i & position) const;
    void CreateRegion(int region, int regionID);
    void FillRegion(const sf::Vector2i & startingPoint);
    
    CDynamicRegion * FindRegion(int regionID);

    void ProcessPoint(const sf::Vector2i & source, const sf::Vector2i & offset);
    bool ShouldBeVisited(const sf::Vector2i & position) const;

    static int mLastRegionID;
    
    sf::Vector2i mMapSize;
    sf::Vector2i mRegionSize;
    sf::Vector2i mLastPointChecked;
    
    OpenList mOpenList;
    LinkList mLinkList;
    
    std::vector< CCell > * mCellList;
    std::vector< CDynamicRegion * > * mRegionList;
};

} // namespace Map

#endif // __CDynamicRegionsGenerator_h__
