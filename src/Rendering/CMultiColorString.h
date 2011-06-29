#ifndef __CMULTICOLORSTRING_H__
#define __CMULTICOLORSTRING_H__
#include <cstddef>
#include <SFML/Graphics/String.hpp>

class CMultiColorString: public sf::String
{
public:
    static const std::wstring NextColor(sf::Color col);

    virtual void Render(sf::RenderTarget& target) const;
};

#endif // __CMULTICOLORSTRING_H__