#ifndef __PRIMITIVE_CLASS_H__
#define __PRIMITIVE_CLASS_H__

/* Liosan, 27-05:
 * klasa prymitywow graficznych - wspolny zestaw parametrow dla ksztaltow
 */

#include <string>
#include <SFML/Graphics/Color.hpp>

class CPrimitiveClass {
public:
    std::string name;
    
    sf::Color borderColor;

    float defaultTime;

    CPrimitiveClass(std::string &name);
};

#endif /*__PRIMITIVE_CLASS_H__*/
