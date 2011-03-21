#include "CShortestPaths.h"
#include "../../CGame.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Logic/CPhysicalManager.h"
#include "../../Logic/CPhysical.h"
#include "CActorAI.h"
#include <queue>
#include <iostream>

sf::Vector2f CShortestPaths::GetCoordsByAIData(CActorAIData * data)
{
	return data->GetPosition();// - Maths::Normalize( sf::Vector2f(1.0f,1.0f) ) * physical->GetCircleRadius() * 0.9f;
}

bool CShortestPaths::SetWayPoint( const sf::Vector2i& field, const sf::Vector2i& size, char wayPointState )
{
	if ( field.x >= 0 && 
		 field.y >= 0 && 
		 field.x + size.x < mSize.x && 
		 field.y + size.y < mSize.y &&
		 ( mWayPointState[ field.y * mSize.x + field.x ] & WP_UNKNOWN ) != 0 &&
		 Maths::Length( gMapManager.GetCollisionMap()->GetCoordsByCell( field - mDestIndex ) ) < mMaxDistance )
	{
		bool collision = false;
		CCollisionMap* cmap = gMapManager.GetCollisionMap();

		if ( wayPointState & WP_LEFT ) 
		if ( cmap->GetCollisionByRect( field + sf::Vector2i(size.x,0), field + size ) )
			collision = true;

		if ( wayPointState & WP_RIGHT ) 
		if ( cmap->GetCollisionByRect( field, field + sf::Vector2i(0,size.y) ) )
			collision = true;

		if ( wayPointState & WP_UP ) 
		if ( cmap->GetCollisionByRect( field + sf::Vector2i(0,size.y), field + size ) )
			collision = true;

		if ( wayPointState & WP_DOWN ) 
		if ( cmap->GetCollisionByRect( field, field + sf::Vector2i(size.x,0) ) )
			collision = true;		
		
		if ( collision )
		{
			for ( int y = field.y; y <= field.y + size.y; y++ )
			for ( int x = field.x; x <= field.x + size.x; x++ )
			{
				if ( ( mWayPointState[ y * mSize.x + x ] & WP_UNKNOWN ) != 0 )
				{
					mWayPointState[ y * mSize.x + x ] |= wayPointState;
				}
			}
		}
		
		mWayPointState[ field.y * mSize.x + field.x ] = wayPointState;
		return !collision;
	}
	
	return false;
}

CShortestPaths::CShortestPaths()
:	mSize( 0,0 ),
	mMaxDistance( 0.0f ),
	mPathWidth( 1.0f ),
	mTimeToUpdate( 3.0f ),
	mUpdateInterval( 1.0f )
{
	gGame.AddFrameListener( (IFrameListener*) this );
}

CShortestPaths::~CShortestPaths()
{
	mWayPointState.clear();
}

void CShortestPaths::Initialize( const std::wstring& physicalDestination, float maxDistance, float pathWidth )
{
	mPhysicalDest = physicalDestination;
	mMaxDistance = maxDistance;
	mPathWidth = pathWidth;
	mTimeToUpdate = gRand.Rndf() * mUpdateInterval;
	Update();
}

void CShortestPaths::Update()
{
	CCollisionMap * collisionMap = gMapManager.GetCollisionMap();

	// Uaktualnij wielkosc mapy

	if ( collisionMap->GetSize() != mSize )
	{
		mSize = collisionMap->GetSize();
		mWayPointState.resize( mSize.x * mSize.y );
	}

	// Wyczysc stan sciezki

	for ( unsigned i = 0; i < mWayPointState.size(); i++ )
		mWayPointState[i] = WP_UNKNOWN;

	// Jesli istnieje physical - cel oblicz nowe sciezki

	if ( CPhysical* physicalDest = gPhysicalManager.GetPhysicalById( mPhysicalDest ) )
	{
		std::queue< sf::Vector2i > bfs;
		std::queue< sf::Vector2i > secondBfs;
		sf::Vector2i newField;
		sf::Vector2i curField;

		sf::Vector2i size = collisionMap->GetCellByCoords( sf::Vector2f( mPathWidth, mPathWidth ) );
		sf::Vector2i dest = collisionMap->GetCellByCoords( physicalDest->GetPosition() );
		mDestIndex = dest;

		if ( dest.x >= mSize.x - size.x )	dest.x = mSize.x - size.x - 1;
		if ( dest.y >= mSize.y - size.y )	dest.y = mSize.y - size.y - 1;

		if ( dest.x >= 0 && dest.x + size.x < mSize.x && dest.y >= 0 && dest.y + size.y < mSize.y )
		{
			mWayPointState[ dest.y * mSize.x + dest.x ] = WP_DESTINATION;
			bfs.push( dest );
		}

		while ( !bfs.empty() )
		{
			curField = bfs.front(); bfs.pop();
			char states = 0;

			if ( SetWayPoint( newField = curField - sf::Vector2i( 1, 0 ), size, WP_RIGHT ) ) { 
				bfs.push( newField ); states |= WP_RIGHT; }
			
			if ( SetWayPoint( newField = curField + sf::Vector2i( 1, 0 ), size, WP_LEFT ) ) {
				bfs.push( newField ); states |= WP_LEFT; }

			if ( SetWayPoint( newField = curField - sf::Vector2i( 0, 1 ), size, WP_DOWN ) ) {
				bfs.push( newField ); states |= WP_DOWN; }

			if ( SetWayPoint( newField = curField + sf::Vector2i( 0, 1 ), size, WP_UP ) ) {
				bfs.push( newField ); states |= WP_UP; }

			if ( (states & WP_RIGHT) && (states & WP_DOWN) && SetWayPoint( newField = curField + sf::Vector2i(-1,-1 ), size, WP_RIGHT | WP_DOWN ) )
				bfs.push( newField );

			if ( (states & WP_LEFT) && (states & WP_DOWN) && SetWayPoint( newField = curField + sf::Vector2i( 1,-1 ), size, WP_LEFT | WP_DOWN ) )
				bfs.push( newField );

			if ( (states & WP_RIGHT) && (states & WP_UP) && SetWayPoint( newField = curField + sf::Vector2i(-1, 1 ), size, WP_RIGHT | WP_UP ) )
				bfs.push( newField );

			if ( (states & WP_LEFT) && (states & WP_UP) && SetWayPoint( newField = curField + sf::Vector2i( 1, 1 ), size, WP_LEFT | WP_UP ) )
				bfs.push( newField );
		}

		// TEMP: 
		/*if (this->mPathWidth < 1.0f){
			std::cout << "PATHS:\n";
			for ( int y = 0; y < mSize.y; y++ ) {
				for ( int x = 0; x < mSize.x; x++ )
					std::cout << (int) mWayPointState[ y * mSize.x + x ] << " " ;
				std::cout << "\n";
			}
			std::cout << "\n" << std::flush;
		}*/
	}
}

void CShortestPaths::FrameStarted( float secondsPassed )
{
	mTimeToUpdate -= secondsPassed;

	if ( mTimeToUpdate < 0.0f )
	{
		mTimeToUpdate = mUpdateInterval;
		Update();
	}
}

bool CShortestPaths::GetNextWayPoint( CActorAIData * data, sf::Vector2f * out, bool ignoreWalls )
{
	sf::Vector2f oldPosition = GetCoordsByAIData( data );
	sf::Vector2f newPosition;
	sf::Vector2i index = gMapManager.GetCollisionMap()->GetCellByCoords( oldPosition );
	sf::Vector2i firstIndexChange(0,0), secondIndexChange(0,0);

	if ( index.x >= 0 && index.x < mSize.x && index.y >= 0 && index.y < mSize.y )
	{
		if ( mWayPointState[ index.y * mSize.x + index.x ] & WP_LEFT )	-- firstIndexChange.x; 
		if ( mWayPointState[ index.y * mSize.x + index.x ] & WP_RIGHT )	++ firstIndexChange.x; 
		if ( mWayPointState[ index.y * mSize.x + index.x ] & WP_UP )	-- firstIndexChange.y; 
		if ( mWayPointState[ index.y * mSize.x + index.x ] & WP_DOWN )	++ firstIndexChange.y;
		if (!ignoreWalls){
			if ( ( firstIndexChange.x | firstIndexChange.y ) == 0 )			return false;	// szczegolny przypadek - potwor niby stoi w scianie
		}
		newPosition = gMapManager.GetCollisionMap()->GetCoordsByCell( index += firstIndexChange );

		if ( mWayPointState[ index.y * mSize.x + index.x ] & WP_LEFT )	-- secondIndexChange.x; 
		if ( mWayPointState[ index.y * mSize.x + index.x ] & WP_RIGHT )	++ secondIndexChange.x; 
		if ( mWayPointState[ index.y * mSize.x + index.x ] & WP_UP )	-- secondIndexChange.y; 
		if ( mWayPointState[ index.y * mSize.x + index.x ] & WP_DOWN )	++ secondIndexChange.y;

		// wygladz sciezke, ale nie scinaj katow prostych bo prawdopodobnie stoi tam sciana
		if ( Maths::Dot( firstIndexChange, secondIndexChange ) != 0 || Maths::Length( newPosition - oldPosition ) < 0.2f )
			newPosition = gMapManager.GetCollisionMap()->GetCoordsByCell( index + secondIndexChange );
	} else {
		return false;
	}

	(*out) = data->GetPosition() + newPosition - oldPosition;
	return true;
}

/* TEMP 
void CShortestPaths::MousePressed(const sf::Event::MouseButtonEvent &e)
{
	if ( mPhysicalDest == L"player0" && mPathWidth < 1.0f && mWayPointState.size() > 0 )
	{
		sf::Vector2f pos = ( sf::Vector2f( e.X, e.Y ) + gCamera.GetViewTopLeft() ) / 64.0f;
		sf::Vector2i index = GetIndexByCoords( pos );
		std::cout << "wp: " << pos.x << " " <<pos.y << " " <<(int) mWayPointState[ index.y * mSize.x + index.x ] << std::endl;
	}
}
*/

