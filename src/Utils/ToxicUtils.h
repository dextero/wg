#ifndef __TOXIC_UTILS_H__
#define __TOXIC_UTILS_H__

#ifdef __I_AM_TOXIC__

#include "../IMouseListener.h"
#include "../IKeyListener.h"

namespace ToxicUtils {

class Editor : public IMouseListener, public IKeyListener
{
    virtual void MousePressed( const sf::Event::MouseButtonEvent &e );
//    virtual void MouseReleased( const sf::Event::MouseButtonEvent &e ) {};
//    virtual void MouseMoved( const sf::Event::MouseMoveEvent& e ) {}; 


    virtual void KeyPressed( const sf::Event::KeyEvent &e );
//    virtual void KeyReleased( const sf::Event::KeyEvent &e ) {}; 
};


static bool isGameInCrimsonMode;

}

#else//__I_AM_TOXIC__

class ToxicUtils {
    public:
        static bool isGameInCrimsonMode;
};

#endif//__I_AM_TOXIC__

#endif//__TOXIC_UTIlS_H__//

