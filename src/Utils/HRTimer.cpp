#ifdef _WIN32
#include "HRTimer.h"

HRTimer::HRTimer(void)
    : start(),
    stop(),
    frequency()

{
    frequency = this->GetFrequency();
}

double HRTimer::GetFrequency(void)
{
    LARGE_INTEGER proc_freq;
    ::QueryPerformanceFrequency(&proc_freq);
	//warning C4244: 'return' : conversion from 'LONGLONG' to 'double', possible loss of data
    return (double)proc_freq.QuadPart;
}

void HRTimer::StartTimer(void)
{
    DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);
    ::QueryPerformanceCounter(&start);
    ::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
}

double HRTimer::StopTimer(void)
{
    DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);
    ::QueryPerformanceCounter(&stop);
    ::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
    return ((stop.QuadPart - start.QuadPart) / frequency);
} 
#endif