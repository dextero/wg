#include "CSoundPlayer.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Utils/CXml.h"
#include "../Utils/StringUtils.h"
#include "../Utils/MathsFunc.h"
#include "../Rendering/CCamera.h"

namespace Audio
{

CSoundPlayer::CSoundPlayer()
:	mTime(0), 
	mGeneralVolume(1.0f)
{
    fprintf(stderr, "CSoundPlayer::CSoundPlayer()\n");
    gGame.AddFrameListener(this);

	/* Wczytaj z pliku opis dzwiekow i grup */

	CXml xml( "data/sounds/sounds-desc.xml", "root" );
	xml_node<>* node;
	SSoundGroup soundGroup;
	SSoundDesc soundDescription;

	soundGroup.mName = "empty";
	soundGroup.mPlayedSoundsCount = 0;
	soundGroup.mPlayedSoundsLimit = 10;
	mSoundGroups.push_back( soundGroup );

	for ( node = xml.GetChild(0,"group"); node; node = xml.GetSibl(node,"group") )
	{
		soundGroup.mName =				xml.GetString( node, "name" );
		soundGroup.mPlayedSoundsLimit = xml.GetInt( node, "playedSoundsLimit" );
		soundGroup.mPlayedSoundsCount = 0;
		mSoundGroups.push_back( soundGroup );
	}

	for ( node = xml.GetChild(0,"sound"); node; node = xml.GetSibl(node,"sound") )
	{
		soundDescription.mName = xml.GetString( node, "path" );
        // recykling zmiennych sux >_<
        soundDescription.mGroupId.clear();

		std::vector<std::string> groupNames;
		StringUtils::Exclude( xml.GetString( node, "groups" ), ' ', groupNames );

		for ( unsigned i = 0; i < groupNames.size(); i++ )
		{
			unsigned groupId = GetSoundGroupId( groupNames[i] );
			soundDescription.mGroupId.push_back( groupId );
		}

		mSoundDescriptions[ soundDescription.mName ] = soundDescription;
	}
}

CSoundPlayer::~CSoundPlayer()
{
    fprintf(stderr, "~CSoundPlayer::CSoundPlayer()\n");
}

void CSoundPlayer::Play(System::Resource::CSound *sound, sf::Vector2f where)
{
	if ( !sound ) // || mTime - sound->GetLastStarted() < sound->GetMinStartPeriod() )
		 return;

	soundDescPtr soundDesc = mSoundDescriptions.find( sound->GetName() );

	if ( soundDesc != mSoundDescriptions.end() )
	{
		std::vector<unsigned>& groupsToUpdate = soundDesc->second.mGroupId;

		for ( unsigned i = 0; i < groupsToUpdate.size(); i++ )
			if ( mSoundGroups[ groupsToUpdate[i] ].mPlayedSoundsCount >= 
				 mSoundGroups[ groupsToUpdate[i] ].mPlayedSoundsLimit )
				return;
		
		SSoundSource soundSource;
		soundSource.mDescription = soundDesc;
		soundSource.mSource = new sf::Sound;
		soundSource.mSource->SetBuffer( sound->GetSoundBuffer() );
		soundSource.mSource->SetPitch( 1.0f );

        if (gGameOptions.Get3DSound())
        {
		    soundSource.mSource->SetVolume( mGeneralVolume * 100.0f );
            (where -= gCamera.GetPositionInGame()) *= 0.2f;
            soundSource.mSource->SetPosition(where.x, where.y, 0.f);
        }
        else
        {
            sf::Vector2f camPos = gCamera.GetPositionInGame();
            float dist = Maths::Length((where - camPos) * 0.001f);
            // dist+1, zeby w przypadku dzwieku "na glowie" postaci nie bylo glosnosci +inf
            soundSource.mSource->SetVolume(mGeneralVolume * 100.f / (dist + 1.f));
            //soundSource.mSource->SetPosition(camPos.x, camPos.y, 0.f);
        }

		soundSource.mSource->Play();

		sound->SetLastStarted( mTime );
		mSoundSources.push_back( soundSource );

		for ( unsigned i = 0; i < groupsToUpdate.size(); i++ )
			mSoundGroups[ groupsToUpdate[i] ].mPlayedSoundsCount++;
	}
}

void CSoundPlayer::FrameStarted(float secondsPassed)
{
    mTime += secondsPassed;

	for ( unsigned i = 0; i < mSoundSources.size(); )
	{
		if ( mSoundSources[i].mSource->GetStatus() != sf::Sound::Playing )
		{
			vector<unsigned>& groupsToUpdate = mSoundSources[i].mDescription->second.mGroupId;

			for ( unsigned j = 0; j < groupsToUpdate.size(); j++ )
				mSoundGroups[ groupsToUpdate[j] ].mPlayedSoundsCount--;

			delete mSoundSources[i].mSource;
			mSoundSources[i] = mSoundSources[ mSoundSources.size() - 1];
			mSoundSources.pop_back();
		}
		else i++;
	}
}

unsigned CSoundPlayer::GetSoundGroupId(const std::string &name)
{
	for ( unsigned i = 0; i < mSoundGroups.size(); i++ )
		if ( mSoundGroups[i].mName == name )
			return i;
	return 0;
}

} // namespace Audio
