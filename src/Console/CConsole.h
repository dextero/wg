#ifndef __CCONSOLE_H__
#define __CCONSOLE_H__

#include "../Utils/CSingleton.h"

#include "../IFrameListener.h"
#include "../IKeyListener.h"

#include <string>
#include <list>

class CHudSprite;
class CHudStaticText;

#define gConsole CConsole::GetSingleton()

class CConsole : public CSingleton< CConsole >, IFrameListener, IKeyListener
{
public:
    CConsole();
    virtual ~CConsole();

    virtual void FrameStarted( float secondsPassed );
    
    virtual void KeyPressed( const sf::Event::KeyEvent &e );
    virtual void KeyReleased( const sf::Event::KeyEvent &e );

    virtual void Cleanup();

    void Print( const std::wstring& text );
    void Printf( const wchar_t* text, ... );
    void CleanScreen();

    bool GetVisible();
    void SetVisible(bool visible);
    
    unsigned int GetWidth();
    unsigned int GetHeight();

    virtual bool FramesDuringPause(EPauseVariant pv) { return true; };
private:
    void AddCommandToHistory( const std::wstring & command );
    void PrintBackHistory();
    void PrintForwardHistory();
    void ShowAvailableCommands();

	unsigned mWidth;
	unsigned mHeight;
    bool mVisible;

    CHudSprite* mHudSprite;
    CHudStaticText* mHudStaticText;
	CHudStaticText* mVersionStaticText;

    std::wstring mPrompt;

    typedef std::list< std::wstring > CommandsList;

    CommandsList mLines;
    CommandsList mCommandHistory;
    CommandsList::iterator mCurrentHistoryCommand;

    const static unsigned int msCommandHistoryCount;
};

#endif//__CCONSOLE_H__
