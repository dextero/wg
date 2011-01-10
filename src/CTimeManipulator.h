#ifndef __CTIMEMANIPULATOR_H__
#define __CTIMEMANIPULATOR_H__

#include "Utils/CSingleton.h"

#define gTimeManipulator CTimeManipulator::GetSingleton()

namespace Time
{
    enum TimeMode { REWIND_TIME = -1, NORMAL_TIME, BULLET_TIME, SHIFT_TIME  };
};
/// Klasa manipuluj¹ca "czasem" - a w³aœciwie delt¹.
class CTimeManipulator : public CSingleton<CTimeManipulator>
{
public:
    CTimeManipulator();

    float ProcessTime( float delta );
    float operator()(float delta) { return ProcessTime(delta); }

    void SetMode( Time::TimeMode mode ) { mTimeMode = mode; }

    float GetRealDelta() const { return mRealDelta; }
    float GetDelta() const { return mRealDelta * mDeltaMod; }

    inline float GetMaxDt() const { return mMaxDt; }
    inline void SetMaxDt(float maxDt){ mMaxDt = maxDt; }

    /// wartoœci o ile maksymalnie zostanie zwolniony/przyspieszony "czas"
    const float SlowMotionValue;
    const float FastMotionValue;
    /// Szybkoœæ "zmiany zmain czasu" :P
    const float TimeChange;
private:
    float mRealDelta;
    float mDeltaMod;
    float mMaxDt;
    Time::TimeMode mTimeMode;
};

#endif //__CTIMEMANIPULATOR_H__//
