#ifndef __CCLOCK_H__
#define __CCLOCK_H__

#include "CSingleton.h"

#include <SFML/System/Clock.hpp>

#include <ctime>
#include <string>

#define gClock CClock::GetSingleton()

// Prosta klasa reprezentujaca zegar, funkcje inline
class CClock : public CSingleton< CClock >
{
public:
    CClock(): mTime(0.f), mFps(0.f), mFrames(0) { clk.Reset(); }
    virtual ~CClock() {}

    float GetElapsedTime() { return clk.GetElapsedTime();}

    float GetFPS() const { return mFps; }
    float GetAverageFPS() const { return (float) mFrames / mTime; }
    
    unsigned int GetFramesCount() const { return mFrames; }    
    // zwraca czas jaki uplynal od zainicjalizowania zegara
    float GetTotalTime() const { return mTime; }


    time_t GetTime()   { return time(0); }
    clock_t GetClock() { return clock(); }

    // TODO: napisac to lepiej
    const std::string GenerateTimestamp() 
    {
        static char buff[15] = { 0 }; // yyyymmddHHMMSS + NULL == 15
        static struct tm * timeinfo;

        time_t t = time (NULL);

        timeinfo = localtime( &t );

        strftime(buff,sizeof(buff)/sizeof(buff[0]),"%Y%m%d%H%M%S",timeinfo);
    
        return std::string(buff);
    }

    // Wywolywac raz na klatke
    void Update()
    {
        // w celu unikniecia mini-bledow
        static float tmp;

        tmp = clk.GetElapsedTime();
        clk.Reset();

        mTime += tmp;
        mFps = 1.f / tmp;
        ++mFrames;
    }

private:
    float mTime;
    float mFps;
    unsigned int mFrames;
    sf::Clock clk;
};

#endif //__CCLOCK_H__//
