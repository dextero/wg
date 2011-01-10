#ifndef CINPUTHANDLER_H_
#define CINPUTHANDLER_H_

#include "../Utils/CSingleton.h"
#include "../IKeyListener.h"
#include "../IFrameListener.h"
#include "../IJoyListener.h"

#define gInputHandler CInputHandler::GetSingleton()

class CInputHandler : public CSingleton<CInputHandler>, public IFrameListener
{
public:
	CInputHandler();
	virtual ~CInputHandler();

    virtual void FrameStarted( float secondsPassed );

    void SwitchControls();
    void InitControlSets();
};

#endif /*CINPUTHANDLER_H_*/
