#ifndef __CMUSIC_H__
#define __CMUSIC_H__

#include "IResource.h"
#include <cstring>
#include <SFML/Audio.hpp>

namespace System
{
	namespace Resource
	{
        class CMusic: public IResource
		{
		private:
			sf::Music mMusicStream;

		public:
			inline sf::Music& GetMusicStream() { return mMusicStream; }

			/* IResource */

			bool Load(std::map<std::string,std::string>& argv);
			void Drop();
			std::string GetType();
		};
	}
}

#endif//__CMUSIC_H__

