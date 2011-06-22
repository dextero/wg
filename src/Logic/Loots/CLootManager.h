#ifndef __CLOOTMANAGER_H__
#define __CLOOTMANAGER_H__

#include <SFML/System/Vector2.hpp>

class CLoot;

class CLootManager
{
public:
    /// zwraca przedmiot-znajdzke lub NULL jesli nic nie ma "wypasc" po smierci potwora
    static CLoot * DropLootAt(const sf::Vector2f & position);
};

#endif //__CLOOTMANAGER_H__//
