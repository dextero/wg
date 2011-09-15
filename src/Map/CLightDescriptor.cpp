#include "CLightDescriptor.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/SLight.h"
#include "CMap.h"

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
	light->mPosition = sf::Vector3f(x * Map::TILE_SIZE, y * Map::TILE_SIZE, 30.0f);
	light->mRadius = radius * Map::TILE_SIZE; //todo: better to do the TILE_SIZE scaling at gDrawableManager...
	light->mColor = color;
}

