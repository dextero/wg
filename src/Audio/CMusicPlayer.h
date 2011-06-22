#ifndef AUDIO_CMUSICPLAYER_H_
#define AUDIO_CMUSICPLAYER_H_

#include "CPlayList.h"
#include "../IFrameListener.h"
#include "../ResourceManager/CMusic.h"

namespace Audio
{
    class CMusicPlayer: public IFrameListener
    {
    private:
		unsigned int	mCurrSongId;
		float			mTimeToSongEnd;
		CPlayList		mCurrPlayList;

		float mGeneralVolume;
        System::Resource::CMusic* mCurrentSong;
		System::Resource::CMusic* mLastSong;
		std::vector<int> mShuffle;

    public:
        CMusicPlayer();
        ~CMusicPlayer();

        void Play(System::Resource::CMusic *music);

		void SetMusicVolume(float v);
        inline float GetMusicVolume()	{ return mGeneralVolume; }

        void SetPlayList(const CPlayList &playlist);
		void Shuffle();

        void FrameStarted(float secondsPassed);
        bool FramesDuringPause(EPauseVariant pv) { return true; }; 
    };
}

#endif

