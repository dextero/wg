#include "CMusicPlayer.h"
#include "../CGame.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Utils/CRand.h"
#include <algorithm>

#include <iostream>

const float OCCURRING = 3.0f;

namespace Audio
{

CMusicPlayer::CMusicPlayer()
:	mTimeToSongEnd(0),
    mGeneralVolume(1.0f),
	mCurrentSong(0),
	mLastSong(0),
    mShuffle(NULL)
{
    gGame.AddFrameListener(this);
    fprintf(stderr, "CMusicPlayer::CMusicPlayer()\n");
}

CMusicPlayer::~CMusicPlayer()
{
    fprintf(stderr, "CMusicPlayer::~CMusicPlayer()\n");
}

void CMusicPlayer::SetPlayList(const Audio::CPlayList &playlist)
{
    if(playlist.mSongs.size() == 0)
        return;

    mCurrPlayList = playlist;

	if (mCurrPlayList.IsRandomizable())
	{
		mShuffle.clear();

		for (size_t i=0; i<playlist.mSongs.size(); ++i)
			mShuffle.push_back(i);

		std::random_shuffle(mShuffle.begin(), mShuffle.end());
	}

	Play(playlist.mSongs[ mShuffle[ mCurrSongId = 0 ] ]);
}

void CMusicPlayer::FrameStarted(float secondsPassed)
{
    if( mCurrentSong )
    {
		float secToEnd = mCurrentSong->GetMusicStream().GetDuration() - mCurrentSong->GetMusicStream().GetPlayingOffset();

        if( secToEnd <= OCCURRING )
        {
			if (++mCurrSongId >= mShuffle.size()) {
				mCurrSongId = 0;
				if ( mCurrPlayList.IsRandomizable() )
					std::random_shuffle(mShuffle.begin(), mShuffle.end());
			}

            Play(mCurrPlayList.mSongs[ mShuffle[mCurrSongId] ] );
        }
    }

	if ( mTimeToSongEnd > 0 )
    {
        mTimeToSongEnd -= secondsPassed;
        if( mTimeToSongEnd <= 0.0f )
        {
            mTimeToSongEnd = 0;
			mCurrentSong->GetMusicStream().SetVolume( mGeneralVolume * 100.0f );
            mLastSong->GetMusicStream().Stop();
        }
		else
        {
            mLastSong->GetMusicStream().SetVolume( mGeneralVolume * mTimeToSongEnd / OCCURRING * 100.0f );
			mCurrentSong->GetMusicStream().SetVolume( mGeneralVolume * (1.0f - mTimeToSongEnd / OCCURRING) * 100.0f );
        }
    }
}

void CMusicPlayer::Play(System::Resource::CMusic *music)
{
	if ( music && music != mCurrentSong )
	{
		if ( mLastSong )
			mLastSong->GetMusicStream().Stop();

		mLastSong = mCurrentSong;
		mCurrentSong = music;
		mCurrentSong->GetMusicStream().Stop();
		mCurrentSong->GetMusicStream().Play();

		if ( mLastSong )
		{
			mTimeToSongEnd = OCCURRING;
			mCurrentSong->GetMusicStream().SetVolume( 0.0f );
		}
		else
		{
			mTimeToSongEnd = 0;
			mCurrentSong->GetMusicStream().SetVolume( mGeneralVolume * 100.0f );
		}
	}
    else if (music == mCurrentSong)
    {
        mLastSong = mCurrentSong;
        mCurrentSong->GetMusicStream().Stop();
        mCurrentSong->GetMusicStream().Play();
    }
}

void CMusicPlayer::SetMusicVolume(float v)
{
    mGeneralVolume = v;
    if (mCurrentSong)
		mCurrentSong->GetMusicStream().SetVolume( mGeneralVolume * 100.0f );
}

void CMusicPlayer::Shuffle()
{
	std::random_shuffle(mShuffle.begin(), mShuffle.end());
}

} // namespace Audio

