#include "CTimeManipulator.h"
#include "Utils/Maths.h"
#include <cstdio>

template<> CTimeManipulator *CSingleton<CTimeManipulator>::msSingleton = NULL;

CTimeManipulator::CTimeManipulator() : 
        SlowMotionValue( 0.25f ), 
        FastMotionValue( 1.5f ), 
        TimeChange ( 0.1f ), 
        mRealDelta(0), 
        mDeltaMod(1.0f), 
        mMaxDt(0.1f),
        mTimeMode(Time::SHIFT_TIME)
{
	fprintf(stderr,"CTimeManipulator::CTimeManipulator()\n");
}

float CTimeManipulator::ProcessTime( float delta )
{
    mRealDelta = delta;

    float result = delta;
    switch ( mTimeMode )
    {
    case Time::NORMAL_TIME:
        
        if ((mDeltaMod - 1.f) > 0.01f)			mDeltaMod -= TimeChange * delta;
        else if ( (mDeltaMod - 1.f) < -0.01f)   mDeltaMod += TimeChange * delta;
        else									mDeltaMod = 1.0f;
		result = delta * mDeltaMod;
        break;

        //mDeltaMod = ( ( (mDeltaMod - 1.f) > 0.0001f )? ( mDeltaMod - TimeChange * delta ) : ( (mDeltaMod - 1.f) < -0.0001f ? ( mDeltaMod + TimeChange * delta ) : 0.f ));
        //return delta * mDeltaMod;
    case Time::BULLET_TIME:
        mDeltaMod = ( ( mDeltaMod > SlowMotionValue )? ( mDeltaMod - TimeChange * delta ) : mDeltaMod);
        result = delta * mDeltaMod;
        break;
    case Time::SHIFT_TIME:
        mDeltaMod = ( ( mDeltaMod < FastMotionValue )? ( mDeltaMod + TimeChange * delta ) : mDeltaMod);
        result = delta * mDeltaMod;
        break;
    case Time::REWIND_TIME:
        result = -delta;
        break;
    }

    return Maths::Clamp(result,-mMaxDt,mMaxDt);
}
