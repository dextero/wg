#ifndef __CWGSPRITE_H__
#define __CWGSPRITE_H__

#include <SFML/Graphics/Sprite.hpp>

class CWGSprite : public sf::Sprite
{
public:
	virtual void Render(sf::RenderTarget &renderTarget) const;
};

#endif /*__CWGSPRITE_H__*/
