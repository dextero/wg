#ifndef __IFRAMELISTENER_H__
#define __IFRAMELISTENER_H__

enum EPauseVariant{ // potrzebne, bo czasem jeden komponent musi dzialac w czasie niektorych pauz a reszta nie
    pvTotal,
    pvConsoleInduces,
    pvLogicOnly,
    pvPauseMenu,
    pvLoading
};

class IFrameListener
{
public:
    virtual ~IFrameListener() {};
    virtual void FrameStarted( float secondsPassed ) = 0;
    
    virtual bool FramesDuringPause(EPauseVariant pv) { return false; };
};


#endif//__IFRAMELISTENER_H__
