#ifndef __ILINEAREFFECT_H__
#define __ILINEAREFFECT_H__

#include <SFML/System/Vector2.hpp>

#include "../../IDrawable.h"

class ILinearEffect: public IDrawable
{
public:
    sf::Vector2f from;
    sf::Vector2f to;

    virtual void Animate(float secondsPassed) = 0;
};

#endif

