#ifndef __CWGSPRITE_H__
#define __CWGSPRITE_H__

#include <cstddef>
#include <SFML/Graphics/Sprite.hpp>

namespace sf { class Image; }

class CWGSprite : public sf::Sprite
{
private:
	sf::Image* normalMap;

public:
	inline sf::Image* GetNormalMap()		{ return normalMap; }
	inline void SetNormalMap(sf::Image* nm)	{ normalMap = nm; }
	virtual void Render(sf::RenderTarget &renderTarget) const;
};

#endif /*__CWGSPRITE_H__*/
