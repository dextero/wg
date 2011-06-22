#ifndef __CMESSAGESYSTEM_H__
#define __CMESSAGESYSTEM_H__


#include "../../CGame.h"
#include "../../IFrameListener.h"
#include <list>

#include <limits>

#include "../CTextArea.h"

class CPhysical;

#define gMessageSystem CMessageSystem::GetSingleton()

// Klasa zajmuje sie rysowaniem wszelkiego typu wiadomosci dla gracza,
// przykladowo odnosnie tego ze podniosl jakis przedmiot, zdobyl nowy poziom
// czy tez trzeba pokazac zastosowana sekwencje.
class CMessageSystem : public CSingleton<CMessageSystem>, public IFrameListener
{
public:
    CMessageSystem();
    virtual ~CMessageSystem() {}

    virtual void FrameStarted( float secondsPassed );

    /// Dodaje "globalne" komunikaty - buforowane, wyswietlajace sie u gory ekranu
    void AddMessage(const wchar_t *message);
    void AddMessagef(const wchar_t *message, ...);

    /// Dodaje niebuforowany komunikat powiazany z jakims physicalem (np: wyswietlane obrazenia)
    void AddMessageTo(const CPhysical *physical, sf::Color col, const wchar_t *message );
    void AddMessageTof(const CPhysical *physical, sf::Color col, const wchar_t *message, ...);

    float GetFadeSpeed() { return mFadeSpeed; }
    void SetFadeSpeed(float speed) { mFadeSpeed = speed; }

    float GetDefaultTTL() { return mDefaultTTL; }
    void SetDefaultTTL(float ttl) { mDefaultTTL = ttl; }

    float GetFlySpeed() { return mFlySpeed; }
    void SetFlySpeed(float speed) { mFlySpeed = speed; }

    sf::Vector2f GetSize() { return sf::Vector2f(mWidth, mHeight); }
    void SetSize( sf::Vector2f size ) { mWidth = size.x; mHeight = size.y;  }
    void SetSize(float width, float height) { mWidth = width; mHeight = height; }

    void Clear();

private:

    struct MS_Message
    {
        GUI::CTextArea *text;
        float ttl;
        float flySpeed;
        float fadeSpeed;
    };

    // pozycja startowa :
    sf::Vector2f mStartPosition;
    // szybkosc zanikania :
    float mFadeSpeed;
    // domyslny czas zycia komunikatu :
    float mDefaultTTL;
    // tempo wznoszenia :
    float mFlySpeed;
    // wymiary komunikatow :
    float mWidth, mHeight;
    // odleglosc pomiedzy kolejnymi komunikatami :
    float mBuff;

    std::list<MS_Message *>::iterator it;

    std::list<MS_Message *> mGlobalMessages;    
    std::list<MS_Message *> mGlobalMessageBuffer;

    std::list<MS_Message *> mQuickMessages;

    void ProcessMessages(std::list<MS_Message *> &msgList, float dt);
};

#endif //__CMESSAGESYSTEM_H__//
