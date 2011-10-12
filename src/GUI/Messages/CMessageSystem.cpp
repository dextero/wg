#include "CMessageSystem.h"
#include <SFML/Graphics.hpp>
#include "../CRoot.h"
#include "../../Utils/StringUtils.h"
#include <cstdarg>
#include <cstdio>
#include "../../Logic/CPhysical.h"
#include "../../Rendering/CCamera.h"
#include "../../Map/CMap.h"
#include "../../GUI/Localization/CLocalizator.h"

template<> CMessageSystem *CSingleton<CMessageSystem>::msSingleton = 0;

CMessageSystem::CMessageSystem()
: mStartPosition(40, 8), mFadeSpeed(2.1f), mDefaultTTL(3), mFlySpeed(4), mWidth(45), mHeight(100), mBuff(0.035f)
{
	fprintf(stderr,"CMessageSystem::CMessageSystem()\n");
    gGame.AddFrameListener(this);
}

void CMessageSystem::Clear()
{
    fprintf(stderr, "Clearing messages\n");
    for (it = mGlobalMessages.begin(); it != mGlobalMessages.end(); ++it)
    {
        if (!(*it))
            continue;
        (*it)->text->Remove();
        delete (*it);
    }
    mGlobalMessages.clear();
    for (it = mGlobalMessageBuffer.begin(); it != mGlobalMessageBuffer.end(); ++it)
    {
        if (!(*it))
            continue;
        (*it)->text->Remove();
        delete (*it);
    }
    mGlobalMessageBuffer.clear();
    for (it = mQuickMessages.begin(); it != mQuickMessages.end(); ++it)
    {
        if (!(*it))
            continue;
        (*it)->text->Remove();
        delete (*it);
    }
    mQuickMessages.clear();
}

void CMessageSystem::ProcessMessages(std::list<MS_Message *> &msgList, float dt)
{
    MS_Message *ptr;
    sf::String* text;
    sf::Color c;
    sf::Vector2f pos;

    it = msgList.begin();
    while( it != msgList.end() )
    {
        ptr = *it;        

        // usuwanie komunikatow
        if ( ptr->ttl < 0.17f ) {
            ptr->text->Remove();
            delete ptr;
            msgList.erase(it++);
            continue;
        }

        text = ptr->text->GetSFString();

        ptr->ttl -= ptr->fadeSpeed * dt;        

        pos = ptr->text->GetPosition();
        pos *= 100.f;
        pos.y -= ptr->flySpeed * dt;
        ptr->text->SetPosition( pos.x , pos.y, mWidth, mHeight );

        if (ptr->ttl < 2.f) {
            c = text->GetColor();
            c.a = static_cast<unsigned char> (ptr->ttl * 127.5f); 
            text->SetColor(c);
        }

        ++it;
    }

}

void CMessageSystem::FrameStarted(float dt)
{
    sf::Vector2f pos;

    // obsluga kolejkowania.
    if (mGlobalMessageBuffer.size() != 0) {
        pos = mGlobalMessages.back()->text->GetPosition();

        if (pos.y < mBuff) {
            mGlobalMessages.push_back( mGlobalMessageBuffer.front() );
            mGlobalMessageBuffer.pop_front();
            mGlobalMessages.back()->text->SetVisible(true);
        }
    }

    // uaktualnienie wiadomosci
    ProcessMessages(mGlobalMessages, dt);
    ProcessMessages(mQuickMessages, dt);
}

void CMessageSystem::AddMessage(const wchar_t *text)
{
    static unsigned int id = 0;
    MS_Message *msg = new MS_Message();

    msg->ttl = mDefaultTTL;
    msg->text = gGUI.CreateTextArea(StringUtils::ToString(id++) + "msg", true, Z_FLOATER_MSG);
    msg->text->SetText(text);
    float size = msg->text->GetSFString()->GetSize();
    msg->text->SetFont(gGUI.GetFontSetting("FONT_IN_GAME_TEXT")); //bylo 22

    msg->text->SetPosition( mStartPosition.x - size * 0.5f, mStartPosition.y ,mWidth, mHeight );

    msg->ttl = mDefaultTTL + 0.03f * size;
    msg->fadeSpeed = mFadeSpeed - 0.04f * size;
    msg->flySpeed = mFlySpeed - 0.04f * size;

    // obs³uga bufora, tak aby komunikaty sie nie pokrywaly.
    if (mGlobalMessages.size() == 0)
        mGlobalMessages.push_back( msg );
    else
    {
        sf::Vector2f lastPos = mGlobalMessages.back()->text->GetPosition();

        // czy mozna juz wrzucic kolejny komunikat?
        if ( lastPos.y < mBuff )
            mGlobalMessages.push_back( msg );
        else {
            msg->text->SetVisible(false);
            mGlobalMessageBuffer.push_back( msg );
        }
    }
}

void CMessageSystem::AddMessageTo(const CPhysical *physical, sf::Color col, const wchar_t *message)
{
    static unsigned int id = 0;
    MS_Message *msg = new MS_Message();

    msg->text = gGUI.CreateTextArea(StringUtils::ToString(id++) + "qmsg",true,Z_FLOATER_MSG + 1);
    msg->text->SetText(message);
    msg->text->SetFont(gGUI.GetFontSetting("FONT_IN_GAME_MESSAGE"));
    
    sf::Vector2f objPos = gCamera.TileToGui( physical->GetPosition() );
    float pixelLen = msg->text->GetSFString()->GetSize();
    float trueLen = gCamera.TileToGui(sf::Vector2f(pixelLen,22.0f)).x/Map::TILE_SIZE;
   
    msg->text->SetPosition(objPos.x - trueLen * 0.5f, objPos.y - 5.f, mWidth, mHeight * 0.7f);
    msg->ttl = mDefaultTTL;
    msg->fadeSpeed = mFadeSpeed * 1.5f;
    msg->flySpeed = mFlySpeed * 1.5f;
    msg->text->SetColor( col );

    mQuickMessages.push_back( msg );
}
void CMessageSystem::AddMessageTof(const CPhysical *p, sf::Color col, const wchar_t *message, ...)
{
    static wchar_t buff[256];

    if (message != NULL && message[0] != L'\0')
    {
        va_list args;
        va_start(args, message);
        vswprintf(buff,255,message,args);
        va_end(args);

        AddMessageTo(p, col, buff);
    }
}

void CMessageSystem::AddMessagef(const wchar_t *text, ...)
{
    static wchar_t buff[2049]; // dluzszych komunikatow raczej nie bedzie ;]

    if (text != NULL && text[0] != L'\0') // sprawdzenie czy dlugosc != 0
    {
        va_list args;
        va_start(args, text);
        vswprintf(buff, 2048, text, args);
        va_end(args);

        AddMessage(buff);
    }
}
