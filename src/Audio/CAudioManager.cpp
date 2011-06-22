#include "CAudioManager.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CSound.h"
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

void CAudioManager::PlaySound(const std::string & sound, const sf::Vector2f & position) {
    System::Resource::CSound * s = gResourceManager.GetSound(sound);
    if (!s) {
        fprintf(stderr, "CAudioManager::PlaySound: failed to play `%s'\n", sound.c_str());
        return;
    }
    mSoundPlayer.Play(s, position);
}

}

