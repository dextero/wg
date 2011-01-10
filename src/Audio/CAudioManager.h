#ifndef AUDIO_CAUDIOMANAGER_
#define AUDIO_CAUDIOMANAGER_

#include "../Utils/CSingleton.h"

#include "CSoundPlayer.h"
#include "CMusicPlayer.h"
#include <cstring>

#define gAudioManager Audio::CAudioManager::GetSingleton()

namespace Audio
{
    class CAudioManager: public CSingleton<CAudioManager>
    {
    private:
        CSoundPlayer mSoundPlayer;
        CMusicPlayer mMusicPlayer;
    public:
        CAudioManager();
        CSoundPlayer &GetSoundPlayer() { return mSoundPlayer; }
        CMusicPlayer &GetMusicPlayer() { return mMusicPlayer; }
    };
} // namespace Audio

#endif

