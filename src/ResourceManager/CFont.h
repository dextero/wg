#ifndef __CFONT_H__
#define __CFONT_H__

#include "IResource.h"
#include <SFML/Graphics/Font.hpp>

namespace System
{
	namespace Resource
	{
		class CFont: public IResource, public sf::Font
		{
		private:
			bool Load(std::map<std::string,std::string>& argv);
		public:
			void Drop();
			std::string GetType();
		};
	}
}

#endif//__CFONT_H__

