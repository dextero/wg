#ifndef __IKEYLISTENER_H__
#define __IKEYLISTENER_H__

#include <SFML/Window/Event.hpp>

class IKeyListener
{
public:
    virtual ~IKeyListener() {};
    virtual void KeyPressed( const sf::Event::KeyEvent &e ) {};
    virtual void KeyReleased( const sf::Event::KeyEvent &e ) {};

};


#endif//__IKEYLISTENER_H__
