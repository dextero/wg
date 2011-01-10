#ifndef __IMOUSE_LISTENER_H__
#define __IMOUSE_LISTENER_H__

#include <SFML/Window/Event.hpp>

class IMouseListener
{
public:
    virtual ~IMouseListener() {};
	virtual void MousePressed( const sf::Event::MouseButtonEvent &e ) {};
	virtual void MouseReleased( const sf::Event::MouseButtonEvent &e ) {};
	virtual void MouseMoved( const sf::Event::MouseMoveEvent& e ) {}; 
    virtual void MouseWheelMoved( const sf::Event::MouseWheelEvent& e ) {};

};


#endif//__IMOUSE_LISTENER_H__
