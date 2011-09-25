#ifndef __CWGSPRITE_H__
#define __CWGSPRITE_H__

#include <cstddef>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Matrix3.hpp>

namespace sf { class Image; }

class CWGSprite : public sf::Sprite
{
private:
	sf::Image* normalMap;

public:
	inline sf::Image* GetNormalMap()		{ return normalMap; }
	inline void SetNormalMap(sf::Image* nm)	{ normalMap = nm; }
	inline const sf::Matrix3& GetMatrix() const { 
		return sf::Sprite::GetMatrix(); }
	inline const sf::Matrix3& GetInverseMatrix() const { 
		return sf::Sprite::GetInverseMatrix(); }
	virtual void Render(sf::RenderTarget &renderTarget) const;
};

#endif /*__CWGSPRITE_H__*/
