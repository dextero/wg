#include "Maths.h"
#include "iostream"


float Maths::PointToSegment( const sf::Vector2f& point, const sf::Vector2f& segA, const sf::Vector2f& segB)
{
    sf::Vector2f closestPoint;
    sf::Vector2f v = segB - segA;
    
    if ( ( v.x < 0.01f ) && ( v.x > -0.01f ) && ( v.y < 0.01f ) && ( v.y > -0.01f ) )
        closestPoint = segA;
    else
    {
        float lineFactor;
        lineFactor = ( v.x * ( point.x - segA.x ) + v.y * ( point.y - segA.y ) ) / ( v.x * v.x + v.y * v.y );
        lineFactor = lineFactor < 0.0f ? 0.0f : lineFactor;
        lineFactor = lineFactor > 1.0f ? 1.0f : lineFactor;
        closestPoint = segA + v * lineFactor;
    }

	return Maths::Length( point - closestPoint );
}


float Maths::SegmentToSegment( sf::Vector2f& segA, sf::Vector2f& segB, sf::Vector2f& segC, sf::Vector2f& segD )
{
	float distA = Maths::PointToSegment( segA, segC, segD );
    float distB = Maths::PointToSegment( segB, segC, segD );
    float distC = Maths::PointToSegment( segC, segA, segB );
    float distD = Maths::PointToSegment( segD, segA, segB );

    return std::min( std::min( distA, distB ), std::min( distC, distD ) );
}


float Maths::PointToRect( sf::Vector2f& point, sf::Vector2f& leftUp, sf::Vector2f& rightDown )
{
	sf::Vector2f leftDown( leftUp.x, rightDown.y );
	sf::Vector2f rightUp( rightDown.x, leftUp.y );

	if ( point.x <= leftUp.x )			return PointToSegment( point, leftUp, leftDown );
	else if ( point.x >= rightDown.x )	return PointToSegment( point, rightUp, rightDown );
	else if ( point.y <= leftUp.y )		return PointToSegment( point, leftUp, rightUp );
	else if ( point.y >= rightDown.y )	return PointToSegment( point, leftDown, rightDown );
	else								return 0.0f;
}

bool Maths::CircleInAngle(const sf::Vector2f& angleCenter, const sf::Vector2f& angleBisector, 
                          float angularWidth, const sf::Vector2f& circleCenter, float circleRadius)
{
	sf::Vector2f angleToCircle = circleCenter - angleCenter;
	sf::Vector2f bisector = Maths::Normalize( angleBisector );

	float halfAngle =	angularWidth * 0.5f * 3.14f / 180.0f; 
	float forwardDist = Maths::Dot( angleToCircle, bisector ); /* odleglosc wzd³uz dwusiecznej srodka okregu i wierzcholka kata */
    if (forwardDist < -circleRadius)
        return false;
    float rightDist =	abs( Maths::Dot( angleToCircle, Maths::VectorRightOf(bisector) ) );
	float width =		tanf(halfAngle) * forwardDist;
	
	return cos(halfAngle) * (rightDist - 0.5f*width) < circleRadius;
}

sf::Vector2f Maths::SeparateCircles(const sf::Vector2f& centerA, float radiusA, 
									const sf::Vector2f& centerB, float radiusB )
{
	sf::Vector2f separateVector( 0.0f, 0.0f );
	float dist = Maths::Length( centerA - centerB );
	if ( dist < radiusA + radiusB && dist > 0.0f )
		separateVector = (centerB - centerA) * ( (radiusA + radiusB - dist) / dist );
	return separateVector;
}

sf::Vector2f Maths::SeparateRectCircle(	const sf::Vector2f& centerA, const sf::Vector2f& sizeA,
										const sf::Vector2f& centerB, const sf::Vector2f& oldCenterB, float radiusB )
{
	sf::Vector2f separateVector( 0.0f, 0.0f );
	sf::Vector2f halfA = sizeA * 0.5f;
	sf::Vector2f vAB = centerB - centerA;
	float r = radiusB;
	
	if ( vAB.x > -halfA.x-r && vAB.x < halfA.x+r && vAB.y > -halfA.y-r && vAB.y < halfA.y+r )
	{
		bool centerBInA = ( vAB.x > -halfA.x && vAB.x < halfA.x && vAB.y > -halfA.y && vAB.y < halfA.y );
		sf::Vector2f vAoldB = oldCenterB - centerA;
		sf::Vector2f closestPoint;

		if ( vAoldB.y <= -Abs( vAoldB.x * halfA.y / halfA.x ) )
			closestPoint = VectorOf( Clamp( vAB.x, -halfA.x, halfA.x ), -halfA.y );
		else if ( vAoldB.y >= Abs( vAoldB.x * halfA.y / halfA.x ) )
			closestPoint = VectorOf( Clamp( vAB.x, -halfA.x, halfA.x ), halfA.y );
		else if ( vAoldB.x >= 0 )
			closestPoint = VectorOf( halfA.x, Clamp( vAB.y, -halfA.y, halfA.y ) );
		else
			closestPoint = VectorOf( -halfA.x, Clamp( vAB.y, -halfA.y, halfA.y ) );

		float vectorFactor = radiusB / (Length( closestPoint - vAB ) + 0.0000001f);
		if ( centerBInA )
			separateVector = ( closestPoint - vAB ) * ( vectorFactor + 1.0f );
		else if ( vectorFactor > 1.0f )
			separateVector = ( closestPoint - vAB ) * (-vectorFactor + 1.0f );
	}

	return separateVector;
}

sf::Vector2f Maths::SeparateRects(	const sf::Vector2f& centerA, const sf::Vector2f& sizeA,
									const sf::Vector2f& centerB, const sf::Vector2f& sizeB )
{
	sf::Vector2f halfSizeAB = (sizeA + sizeB) * 0.5f;
	sf::Vector2f vAB = centerB - centerA;
	sf::Vector2f separateVector(0.0f, 0.0f);

	if ( vAB.x > -halfSizeAB.x && vAB.x < halfSizeAB.x && vAB.y > -halfSizeAB.y && vAB.y < halfSizeAB.y )
	{
		if ( vAB.y <= -Abs( vAB.x * halfSizeAB.y / halfSizeAB.x ) )
			separateVector = VectorOf( 0.0f, -halfSizeAB.y - vAB.y );
		else if ( vAB.y >= Abs( vAB.x * halfSizeAB.y / halfSizeAB.x ) )
			separateVector = VectorOf( 0.0f, halfSizeAB.y - vAB.y );
		else if ( vAB.x >= 0 )
			separateVector = VectorOf( halfSizeAB.x - vAB.x, 0.0f );
		else
			separateVector = VectorOf( -halfSizeAB.x - vAB.x, 0.0f );
	}

	return separateVector;
}

//============================================================================================

