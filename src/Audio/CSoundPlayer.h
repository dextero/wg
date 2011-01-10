#ifndef AUDIO_CSOUNDPLAYER_H_
#define AUDIO_CSOUNDPLAYER_H_

#include "../IFrameListener.h"
#include "../ResourceManager/CSound.h"
#include <vector>
#include <string>
#include <map>
using namespace std;

namespace Audio
{
	struct SSoundGroup
	{
		string		mName;
		unsigned	mPlayedSoundsCount;
		unsigned	mPlayedSoundsLimit;
	};

	struct SSoundDesc		/* opis dzwieku - przynaleznosc do grup */
	{
		string			 mName;
		vector<unsigned> mGroupId;
	};

	typedef map<string,SSoundDesc>::iterator soundDescPtr;

	struct SSoundSource
	{
		sf::Sound*		mSource;
		soundDescPtr	mDescription;
	};

	/* ===================================== */

	class CSoundPlayer: public IFrameListener
	{
	public:
		CSoundPlayer();
		~CSoundPlayer();

        void Play(System::Resource::CSound *sound, const sf::Vector2f where = sf::Vector2f(0.f, 0.f));
		void FrameStarted(float secondsPassed);
		bool FramesDuringPause(EPauseVariant pv) { return true; }

		inline float GetGeneralVolume()			{ return mGeneralVolume; }
		inline void SetGeneralVolume(float val)	{ mGeneralVolume = val; }

	private:
		float mTime;
		float mGeneralVolume;

		vector<SSoundGroup>		mSoundGroups;
		vector<SSoundSource>	mSoundSources;
		map<string,SSoundDesc>	mSoundDescriptions;

		unsigned GetSoundGroupId( const std::string& name );
	};
};

#endif

