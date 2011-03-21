#include "CUtilsTests.h"
#include "Maths.h"
#include <cassert>
#include <cstdio>

void AngleBetweenTests(){
	fprintf(stderr,"running AngleBetweenTests...");
	sf::Vector2f p(0,1);
	float angle;
	angle = Maths::AngleBetween(p,sf::Vector2f(0,1));
	assert((angle - 0) < 0.01);
	angle = Maths::AngleBetween(p,sf::Vector2f(0,-1));
	assert((angle - 180) < 0.01);
	angle = Maths::AngleBetween(p,sf::Vector2f(1,0));
	assert((angle - 90) < 0.01);
	angle = Maths::AngleBetween(p,sf::Vector2f(-1,0));
	assert((angle - 90) < 0.01);
	fprintf(stderr,"passed!");
}

void CUtilsTests::RunAll(){
	AngleBetweenTests();
}
