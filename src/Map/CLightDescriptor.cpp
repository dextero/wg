#include "CLightDescriptor.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/SLight.h"

Map::CLightDescriptor::CLightDescriptor()
:	x(0.0f),
	y(0.0f), 
	radius(100.0f),
    color(sf::Color::White)
{
}

void Map::CLightDescriptor::Create()
{
	SLight* light = gDrawableManager.CreateLight();
	light->mPosition = sf::Vector3f(x, y, 30.0f);
	light->mRadius = radius;
	light->mColor = color;
}

