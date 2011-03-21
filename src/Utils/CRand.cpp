#include "CRand.h"
#include "Maths.h"
using namespace Maths;

template<> CRand * CSingleton<CRand>::msSingleton = 0;

float CRand::Rndf(float mostProbabled)
{
	float m = Clamp( mostProbabled, 0.00001f, 0.99999f );
	float rnd = Rndf();
	float dest;
	float destFactor;

	if ( rnd < m )
	{
		dest = 0.00001f;
		destFactor = Clamp( 0.1f / ( (m-rnd) / m + 0.1f ) - 0.1f );
	}
	else
	{
		dest = 0.99999f;
		destFactor = Clamp( 0.1f / ( (rnd-m) / (1.0f-m) + 0.1f ) - 0.1f );
	}

	return Lerp( m, dest, destFactor );

	/*zuo:
	if ( rnd < m )	return -a / (rnd + a/m) + m + a/m;
	else			return -a / (rnd - a/(m-1) - 1) + m - a/(m - 1);*/
}

