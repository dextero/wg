#ifndef __HR_TIMER_H__
#define __HR_TIMER_H__

#ifdef _WIN32
#include <windows.h>

class HRTimer {
public:
    HRTimer(void);
    double GetFrequency(void);
    void StartTimer(void) ;
    double StopTimer(void);
private:
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
    double frequency;
};
#endif

#endif//__HR_TIMER_H__//

