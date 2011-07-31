#include "CCollisionMap.h"
#include "CDynamicRegionsGenerator.h"
#include "CMap.h"
#include "CMapManager.h"
#include "SearchTools/CPath.h"
#include "SearchTools/CPathFinder.h"
#include "../Logic/CPhysicalManager.h"
#include "../Logic/CPhysical.h"
#include "../Utils/Maths.h"
#include <climits>
#include <iostream>

const int kUnaccessableTile = INT_MAX;
const int kRegionSize = 10; // kazdy region zajmuje x komurek

CCollisionMap::CCollisionMap()
:	mSize( 0, 0 ),
	mCellPerTile( 0.0f, 0.0f ),
	mEnemyDensity(5)
{
	mCellList.clear();
}

CCollisionMap::~CCollisionMap()
{
	mCellList.clear();
}

void CCollisionMap::Initialize(sf::Vector2i size)
{
	mSize = size;
	
	if ( gMapManager.GetCurrent() )
	{
		mCellPerTile.x = (float) size.x / (float) gMapManager.GetCurrent()->GetSize().x;
		mCellPerTile.y = (float) size.y / (float) gMapManager.GetCurrent()->GetSize().y;
	}
	else
		mCellPerTile = sf::Vector2f( 0.0f, 0.0f );

    mSingleCellSize.x = ( mCellPerTile.x != 0.0f ? 1.0f / mCellPerTile.x : 0.0f );
    mSingleCellSize.y = ( mCellPerTile.y != 0.0f ? 1.0f / mCellPerTile.y : 0.0f );

	mCellList.clear();
	mCellList.resize( mSize.x * mSize.y );
	
    for ( unsigned i = 0; i < mCellList.size(); i++ )
    {
        mCellList[i].collision = false;
    }

	UpdatePhysicals(PHYSICAL_PATHBLOCKER);
	UpdateRegions();
    //UpdatePhysicals(PHYSICAL_ACTORS);
}

void CCollisionMap::UpdateAllPhysicals()
{
	const std::vector< CPhysical* > &physicals = gPhysicalManager.GetPhysicals();
    for ( unsigned i = 0; i < physicals.size(); i++ )
	{
		UpdatePhysical( physicals[i], true );
	}
}

void CCollisionMap::UpdatePhysicals(int category)
{
    const std::vector< CPhysical* > &physicals = gPhysicalManager.GetPhysicals();
    for ( unsigned i = 0; i < physicals.size(); i++ )
    {
        if (physicals[i]->GetSideAndCategory().category & category)
            UpdatePhysical( physicals[i], true );
    }
}

void CCollisionMap::UpdatePhysical(CPhysical *physical, bool collision)
{
	if ( !(physical->GetSideAndCategory().category & PHYSICAL_PATHBLOCKER) )	return;
	if ( ! gMapManager.GetCurrent() )							return;

	// pobierz rozmiar mapy, aby poprawnie zeskalowac wspolrzedne

	sf::Vector2i mapSize = gMapManager.GetCurrent()->GetSize();

	// rzutuj physicala na mape kolizji o danym rozmiarze

	sf::IntRect rect
	(
		int ( (physical->GetPosition().x - physical->GetRectSize().x * 0.49f) * (float) mSize.x / (float) mapSize.x ),
		int ( (physical->GetPosition().y - physical->GetRectSize().y * 0.49f) * (float) mSize.y / (float) mapSize.y ),
		int ( (physical->GetPosition().x + physical->GetRectSize().x * 0.49f) * (float) mSize.x / (float) mapSize.x ),
		int ( (physical->GetPosition().y + physical->GetRectSize().y * 0.49f) * (float) mSize.y / (float) mapSize.y )
	);

	// zabezpiecz przed wyjsciem poza tablice

	if ( rect.Left < 0 )			rect.Left = 0;
	if ( rect.Left >= mSize.x )		rect.Left = mSize.x - 1;
	if ( rect.Right < 0 )			rect.Right = 0;
	if ( rect.Right >= mSize.x )	rect.Right = mSize.x - 1;
	if ( rect.Top < 0 )				rect.Top = 0;
	if ( rect.Top >= mSize.y )		rect.Top = mSize.y - 1;
	if ( rect.Bottom < 0 )			rect.Bottom = 0;
	if ( rect.Bottom >= mSize.y )	rect.Bottom = mSize.y - 1;

	// no i nareszcie...

	for ( unsigned y = rect.Top; y <= (unsigned) rect.Bottom; y++ )
		for ( unsigned x = rect.Left; x <= (unsigned) rect.Right; x++ )
			mCellList[ y * mSize.x + x ].collision = collision;	
}

bool CCollisionMap::GetCollisionByCell( const sf::Vector2i& c ) const
{
    if (c.x < 0 || c.y < 0 || c.x >= mSize.x || c.y >= mSize.y)
        return true;

    return mCellList[CellToIndex(c)].collision;
}

bool CCollisionMap::GetCollisionByRect(const sf::Vector2i &leftUp, const sf::Vector2i &rightDown) const
{
	for ( int y = leftUp.y; y <= rightDown.y; y++ )
		for ( int x = leftUp.x; x <= rightDown.x; x++ )
			if ( mCellList[ y * mSize.x + x ].collision )
				return true;
	return false;
}

int CCollisionMap::GetDensityByCell(const sf::Vector2i& c)
{
    int index = CellToIndex(c);
    return (mCellList[index].collision == true ? kUnaccessableTile : mCellList[index].density);
}

int CCollisionMap::GetDensityByRect(const sf::Vector2i& leftUp, const sf::Vector2i& rightDown)
{
    if (GetCollisionByRect(leftUp, rightDown) == true)
    {
        return kUnaccessableTile;
    }

    int totalDensity = 0;
    for (int y = leftUp.y; y <= rightDown.y; ++y)
    {
        for (int x = leftUp.x; x <= rightDown.x; ++x)
        {
            totalDensity += mCellList[y * mSize.x + x].density;
        }
    }
    return totalDensity;
}

void CCollisionMap::AddPath(const CPath * path)
{
    ModifyPath(path, mEnemyDensity);
}

void CCollisionMap::RemovePath(const CPath * path)
{
    ModifyPath(path, -mEnemyDensity);
}

void CCollisionMap::ModifyPath(const CPath * path, int delta)
{
    sf::Vector2i size = path->GetSize();
    for (unsigned int i = 0; i < path->Length(); ++i)
    {
        sf::Vector2i index = path->GetWaypoint(i);
        for (int y = 0; y < size.y; ++y)
        {
            for (int x = 0; x < size.x; ++x)
            {
                mCellList[(index.y + y) * mSize.x + (index.x + x)].density += delta;
            }
        }        
    }
}

bool CCollisionMap::FindPath(
    const sf::Vector2f & start,
    const sf::Vector2f & goal,
    const sf::Vector2f & physicalSize,
    CPath ** path)
{
    return CPathFinder().FindPath(start, goal, physicalSize, path);
}

sf::Vector2i CCollisionMap::GetCellByCoords( const sf::Vector2f& point ) 
{ 
    return sf::Vector2i( 
        Maths::ClampInt( (int) ( point.x * mCellPerTile.x ), 0, mSize.x-1 ), 
        Maths::ClampInt( (int) ( point.y * mCellPerTile.y ), 0, mSize.y-1 ) 
        );
}

sf::Vector2f CCollisionMap::GetCoordsByCell( const sf::Vector2i& field )
{
    return sf::Vector2f( 
        (float) field.x * mSingleCellSize.x, 
        (float) field.y * mSingleCellSize.y
        );
}

sf::Vector2i CCollisionMap::ConvertSizeToCells(const sf::Vector2f & size)
{
    return sf::Vector2i(
        static_cast< int >(size.x / mSingleCellSize.x),
        static_cast< int >(size.y / mSingleCellSize.y));
}

void CCollisionMap::UpdateRegions()
{
    Map::CDynamicRegionsGenerator regionsGenerator(mSize, sf::Vector2i(kRegionSize, kRegionSize));
    regionsGenerator.GenerateRegions(mCellList, mRegionList);
}

CDynamicRegion * CCollisionMap::GetRegionByCell( const sf::Vector2i & cell )
{
    int regionId = mCellList.at(CellToIndex((cell))).regionId;

    for (std::vector< CDynamicRegion * >::const_iterator it = mRegionList.begin();
        it != mRegionList.end(); ++it)
    {
        if ((*it)->GetRegionID() == regionId)
            return *it;
    }
    return NULL;
}
