#ifndef __IJOY_LISTENER_H__
#define __IJOY_LISTENER_H__

#include <SFML/Window/Event.hpp>

class IJoyListener
{
public:
    virtual ~IJoyListener() {};
	virtual void JoyButtonPressed( const sf::Event::JoyButtonEvent &e ) {};
	virtual void JoyButtonReleased( const sf::Event::JoyButtonEvent &e ) {};
	virtual void JoyMoved( const sf::Event::JoyMoveEvent &e ) {}; 

};


#endif//__IJOYSTICK_LISTENER_H__
