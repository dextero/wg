#include "CDynamicRegion.h"
#include <algorithm>

CDynamicRegion::CDynamicRegion(int regionID) : mID(regionID)
{}

int CDynamicRegion::GetRegionID() const
{
    return mID;
}

void CDynamicRegion::AddNeighbour(CDynamicRegion * region)
{
    if (std::find(mNeighbours.begin(), mNeighbours.end(), region) == mNeighbours.end())
    {
        mNeighbours.push_back(region);
        region->AddNeighbour(this);
    }
}

void CDynamicRegion::AddCell(const sf::Vector2i & cell)
{
    bool addCell = true;
    mCenter = sf::Vector2i(0, 0);

    for (std::vector< sf::Vector2i >::const_iterator it = mCells.begin();
         it != mCells.end(); ++it)
    {
        mCenter += *it;
        addCell &= (*it != cell);
    }

    if (addCell)
    {
        mCells.push_back(cell);
        mCenter += cell;
    }

    mCenter /= static_cast< int >(mCells.size());
}

const DynamicRegionList & CDynamicRegion::GetNeighbours() const
{
    return mNeighbours;
}

const sf::Vector2i & CDynamicRegion::GetCenter() const
{
    return mCenter;
}

std::ostream & operator<<(
    std::ostream & out, const CDynamicRegion & region)
{
    out << region.GetRegionID() << ": ";
    for (unsigned int i = 0; i < region.mNeighbours.size(); ++i)
        out << region.mNeighbours.at(i)->GetRegionID() << " ";

    return out;
}


