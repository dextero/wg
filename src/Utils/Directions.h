#ifndef __DIRECTIONS_H__
#define __DIRECTIONS_H__

#include <SFML/System/Vector2.hpp>

sf::Vector2f RotationToVector(float angle);

int RotationFromVector(sf::Vector2f v);

#endif // __DIRECTIONS_H__

