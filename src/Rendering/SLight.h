#ifndef LIGHT_H
#define LIGHT_H

#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Color.hpp>

class SLight
{
	friend class CDrawableManager;

public:
	sf::Vector3f mPosition;
	sf::Color mColor;
	float mRadius;

	inline static SLight* BLACK_LIGHT()
	{
		static SLight BLACK(sf::Vector3f(0.0f, 0.0f, 0.0f), sf::Color::Black, 1.0f);
		return &BLACK;
	}

private:
	SLight() 
	:	mRadius (1.0f) {}

	SLight(const sf::Vector3f& pos, const sf::Color& color, float radius) 
	:	mPosition(pos), 
		mColor(color), 
		mRadius(radius) {}

	~SLight() {}
};

#endif