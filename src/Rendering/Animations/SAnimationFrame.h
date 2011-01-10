#ifndef __SANIMATION_FRAME__
#define __SANIMATION_FRAME__

#include <string>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

struct SAnimationFrame
{
    SAnimationFrame( const std::string& _imageName, 
            const sf::IntRect& _rect = sf::IntRect(0,0,0,0),
            const sf::Vector2f& _offset = sf::Vector2f(0.0f, 0.0f),
            bool _isFlipped = false,
            const sf::Vector2i& _normalSize = sf::Vector2i(0,0) );

    std::string imageName;
    sf::IntRect rect; // jesli rect = 0,0,0,0 to umawiamy sie,
                      // ze chodzi o caly obrazek

    sf::Vector2f offset;
    sf::Vector2i normalSize; // <-- byc moze to przerzucic do calej animacji
    bool isFlipped; // ramka odbita wzgledem lewo-prawo

};

#endif// __SANIMATION_FRAME__

