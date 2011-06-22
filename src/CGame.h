#ifndef __CGAME_H__
#define __CGAME_H__

#include "Utils/CSingleton.h"

#include <set>

#include <SFML/System/Vector2.hpp> 
#include <FastDelegate.h>
#include <string>

 
class IFrameListener;
class IKeyListener;
class IMouseListener;
class IJoyListener;
namespace sf {
    class RenderWindow;
};

#define gGame CGame::GetSingleton()

class CGame : public CSingleton< CGame >
{
protected:
    virtual void Cleanup();
public:
    CGame();
    virtual ~CGame();

    void Run();

    sf::RenderWindow* GetRenderWindow();
    void SetRenderWindow(sf::RenderWindow *renderWindow);

    void AddFrameListener( IFrameListener* frameListener );
    void AddKeyListener( IKeyListener* keyListener );
	void AddMouseListener( IMouseListener* mouseListener );
	void AddJoyListener( IJoyListener* joyListener );

    inline bool GetPause() { return mPaused; }
    inline void SetPause(bool p) { mPaused = p; }

    inline bool GetConsoleOpened() { return mConsoleOpened; }
    inline void SetConsoleOpened(bool p) { mConsoleOpened = p; }

    inline bool GetFreezed(){ return mFreezed; }
#ifndef __EDITOR__
    inline void SetFreezed(bool p) { mFreezed = p; }
#else
    inline void SetFreezed(bool p) { mFreezed = true; }
#endif

    inline bool GetShowingFps() { return mShowFps; }
    void SetShowingFps(bool show);

	typedef fastdelegate::FastDelegate1<const void *,void> loadingRoutine;
	void ScheduleLoadingRoutine(loadingRoutine method, void *data, bool hideLoadingScreen = false);

	static bool dontLoadWindowHack;
private:
    // ponizsze przewalic do PIMPL (yikes!)
    // narazie niech tu sobie polezy roboczo:
    // rAum: nie przewalamy do PIMPL bo nie ma takiej potrzeby

    void MainLoopStep();
    sf::RenderWindow* mRenderWindow;

    std::set< IFrameListener* > mFrameListeners;
    std::set< IKeyListener* > mKeyListeners;
	std::set< IMouseListener* > mMouseListeners;
	std::set< IJoyListener* > mJoyListeners;

    
    bool mPaused;
    bool mConsoleOpened;
    bool mFreezed, mWasFreezedBeforeLostFocus;
    bool mShowFps;
    float mTimeAccumulator;
    const float mTimeStep;
	bool mLoading, mLoaded;     // mLoaded == true => animacja chowania
	int mLoadingFrameCount;

#ifdef __EDITOR__
    friend class CSFMLWnd;

    static void Init(sf::RenderWindow* wnd);
#endif

	loadingRoutine mLoadingRoutine;
	void *mLoadingData;

	std::string *mMapToLoadAtInit;
public:
	inline void SetMapToLoadAtInit(std::string *mapToLoadAtInit){
		mMapToLoadAtInit = mapToLoadAtInit;
	}
};


#endif//__CGAME_H__
