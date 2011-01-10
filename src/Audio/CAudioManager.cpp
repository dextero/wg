#include "CAudioManager.h"
#include <cstdio>
#include <../../build/SFML_sources/extlibs/headers/AL/al.h>

template<> Audio::CAudioManager* CSingleton<Audio::CAudioManager>::msSingleton = 0;

namespace Audio
{

CAudioManager::CAudioManager()
{
	fprintf(stderr,"CAudioManager::CAudioManager()\n");
    sf::SoundBuffer nothing;
    fprintf(stderr,"OpenAL initialize: %s\n", alGetString(alGetError()));
}

}

