#include "CDynamicRegionsGenerator.h"
#include "CDynamicRegion.h"
#include <algorithm>
// #include <cstdio>
// #include <fstream>

using namespace Map;

int CDynamicRegionsGenerator::mLastRegionID = 0;

Map::CDynamicRegionsGenerator::CDynamicRegionsGenerator(
    const sf::Vector2i & mapSize,
    const sf::Vector2i & regionSize) :
    mMapSize(mapSize),
    mRegionSize(regionSize),
    mLastPointChecked(0, 0),
    mCellList(NULL),
    mRegionList(NULL)
{}

void Map::CDynamicRegionsGenerator::GenerateRegions(
    std::vector< CCell > & cellList,
    std::vector< CDynamicRegion * > & regionList)
{   
    mCellList = &cellList;
    mRegionList = &regionList;
    mLastPointChecked = sf::Vector2i(0, 0);
    mOpenList.clear();
    
    mLastPointChecked = FindNextStartingPoint();
    while (mLastPointChecked != sf::Vector2i(-1, -1))
    {
        FillRegion(mLastPointChecked);
        mLastPointChecked = FindNextStartingPoint();
    }
    
//     FILE * reg = fopen("regions.raw", "wb");
//     FILE * col = fopen("collisions.raw", "wb");
//     for (int y = 0; y < mMapSize.y; ++y)
//     {
//         for (int x = 0; x < mMapSize.x; ++x)
//         {
//             char a = 1, b = 0;
//             fwrite(&(mCellList->at(y * mMapSize.x + x).regionId), sizeof(char), 1, reg);
//             fwrite((mCellList->at(y * mMapSize.x + x).collision ? &a : &b), sizeof(char), 1, col);
//         }
//     }
//     fclose(reg);
//     fclose(col);
//     
//     std::ofstream out("links.txt");
//     for (unsigned int i = 0; i < mRegionList->size(); ++i)
//     {
//         out << (*mRegionList->at(i)) << std::endl;
//     }    
    
    mCellList = NULL;
    mRegionList = NULL;
}

const sf::Vector2i Map::CDynamicRegionsGenerator::FindNextStartingPoint() const
{
    for (int y = mLastPointChecked.y; y < mMapSize.y; ++y)
    {
        for (int x = mLastPointChecked.x; x < mMapSize.x; ++x)
        {
            int offset = y * mMapSize.x + x;
            if (mCellList->at(offset).collision == false &&
                mCellList->at(offset).regionId == 0)
            {
                return sf::Vector2i(x, y);
            }
        }
    }
    return sf::Vector2i(-1, -1);
}


void Map::CDynamicRegionsGenerator::GenerateLinks(
    int region, CDynamicRegion * newRegion)
{
    std::pair< LinkList::iterator, LinkList::iterator > range = mLinkList.equal_range(region);
    for (LinkList::iterator it = range.first; it != range.second; ++it)
    {
        newRegion->AddNeighbour(FindRegion((*it).second));
    }
}

int Map::CDynamicRegionsGenerator::PositionToRegion(
    const sf::Vector2i & position) const
{
    return (position.y / mRegionSize.y) * ((mMapSize.x / mRegionSize.x) + 1 ) + 
           (position.x / mRegionSize.x);
}

void Map::CDynamicRegionsGenerator::CreateRegion( 
    int region, int regionID)
{
    CDynamicRegion * newRegion = FindRegion(regionID);
    if (newRegion == NULL)
    {
        newRegion = new CDynamicRegion(regionID);
        mRegionList->push_back(newRegion);
    }

    GenerateLinks(region, newRegion);
}

bool Map::CDynamicRegionsGenerator::ShouldBeVisited(
    const sf::Vector2i & position) const
{
    if (position.x >= 0 && position.x < mMapSize.x &&
        position.y >= 0 && position.y < mMapSize.y)
    {
        int cellIndex = position.y * mMapSize.x + position.x;
        return (mCellList->at(cellIndex).regionId == 0 &&
                mCellList->at(cellIndex).collision == false);
    }

    return false;
}

void Map::CDynamicRegionsGenerator::FillRegion(
    const sf::Vector2i & startingPoint)
{    
    mOpenList[PositionToRegion(startingPoint)].push_back(startingPoint);

    while (!mOpenList.empty())
    {
        int currentRegion = mOpenList.begin()->first;
        CreateRegion(currentRegion, ++mLastRegionID);
        
        // czy dany region ma jeszcze jakies punkty na "open" liscie?
        while (mOpenList[currentRegion].empty() == false)
        {
            // pobierz pierwszy punkt i usun go z listy
            sf::Vector2i currentCell = mOpenList[currentRegion].front();
            mOpenList[currentRegion].erase(mOpenList[currentRegion].begin());

            if (ShouldBeVisited(currentCell) == true)
            {
                FindRegion(mLastRegionID)->AddCell(currentCell);
                mCellList->at(currentCell.y * mMapSize.x + currentCell.x).regionId = mLastRegionID;

                ProcessPoint(currentCell, sf::Vector2i(-1, 0));
                ProcessPoint(currentCell, sf::Vector2i(-1,-1));
                ProcessPoint(currentCell, sf::Vector2i( 0,-1));
                ProcessPoint(currentCell, sf::Vector2i( 1,-1));
                ProcessPoint(currentCell, sf::Vector2i( 1, 0));
                ProcessPoint(currentCell, sf::Vector2i( 1, 1));
                ProcessPoint(currentCell, sf::Vector2i( 0, 1));
                ProcessPoint(currentCell, sf::Vector2i(-1, 1));
            }
        }
        mOpenList.erase(mOpenList.find(currentRegion));
    }
}

void Map::CDynamicRegionsGenerator::ProcessPoint(
    const sf::Vector2i & source,
    const sf::Vector2i & offset)
{
    sf::Vector2i point = source + offset;
    if (ShouldBeVisited(point) == true)
    {
        int sourceRegion = PositionToRegion(source);
        int targetRegion = PositionToRegion(point);
        
        mOpenList[targetRegion].push_back(point);
        if (sourceRegion != targetRegion)
            mLinkList.insert(std::make_pair(targetRegion, mLastRegionID));
    }
}

CDynamicRegion * Map::CDynamicRegionsGenerator::FindRegion(
    int regionID)
{
    for (std::vector< CDynamicRegion * >::const_iterator it = mRegionList->begin();
        it != mRegionList->end(); ++it)
    {
        if ((*it)->GetRegionID() == regionID)
            return *it;
    }
    
    return NULL;
}
