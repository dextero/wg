#ifndef __CSOUND_H__
#define __CSOUND_H__

#include "IResource.h"
#include <cstring>
#include <SFML/Audio.hpp>

#undef LoadImage
#undef CreateWindow
#undef MOUSE_MOVED

namespace System
{
	namespace Resource
	{
		class CSound: public IResource
		{
		public:
            CSound();

			void SetLastStarted(float startedAt)	{ mLastStarted = startedAt; }
            float GetLastStarted()					{ return mLastStarted; }
            float GetMinStartPeriod()				{ return mMinStartPeriod; }
			sf::SoundBuffer& GetSoundBuffer()		{ return mSoundBuffer; }
			std::string GetName()					{ return mName; }

			/* IResource */

			bool Load(std::map<std::string,std::string>& argv);
			void Drop();
			std::string GetType();

			/* ========= */
		private:
			sf::SoundBuffer mSoundBuffer;
			
            float mLastStarted;
            float mMinStartPeriod;
			std::string mName;
		};
	};
};

#endif//__CSOUND_H__
