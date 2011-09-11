#ifndef LIGHT_DESCRIPTOR_H
#define LIGHT_DESCRIPTOR_H

#include <SFML/Graphics/Color.hpp>

namespace Map
{
	class CLightDescriptor
	{
	public:
		float x,y;
		float radius;
        sf::Color color;
        
		CLightDescriptor();
		void Create();
	};
};

#endif