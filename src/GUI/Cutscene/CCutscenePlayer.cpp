#include "CCutscenePlayer.h"
#include "CutsceneActions.h"
#include "../../Audio/CAudioManager.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../ResourceManager/CSound.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/MathsFunc.h"
#include "../CRoot.h"
#include "../CImageBox.h"
#include "../CTextArea.h"
#include "../../CGame.h"
#include "../Localization/CLocalizator.h"
#include <algorithm>

using namespace StringUtils;
using namespace GUI;
using namespace Maths;

template<> CCutscenePlayer* CSingleton<CCutscenePlayer>::msSingleton = 0;

CCutscenePlayer::CCutscenePlayer()
:	mTime( 0.0f ),
	mState( CUTSCENE_EMPTY ),
    mImageBox( NULL ),
    mTextArea( NULL )
{
	gGame.AddFrameListener( this );
	gGame.AddKeyListener( this );

    InitGuiControls();
}

CCutscenePlayer::~CCutscenePlayer()
{
}

void CCutscenePlayer::PlayCutscene(const std::string &cutscene)
{
	CXml xml( cutscene, "root" );
	xml_node<>* node;

	if ( mState != CUTSCENE_EMPTY )
	{
		StopCutscene();
	}

	if ( xml.GetString( 0, "type" ) == "cutscene" )
	{
		for ( node=xml.GetChild(0,"setImage"); node; node=xml.GetSibl(node,"setImage") )
		{
			CCutsceneImage * img = new CCutsceneImage;
			img->mTime = xml.GetFloat( node, "time" );
            img->mImagePath = StringUtils::ConvertToString(gLocalizator.Localize(xml.GetString( node, "path" )));
	
			mActionsToExecute.push_back( (CCutsceneAction*) img );
		}

		for ( node=xml.GetChild(0,"setCamera"); node; node=xml.GetSibl(node,"setCamera") )
		{
			CCutsceneCamera * cam = new CCutsceneCamera;
			cam->mTime = xml.GetFloat( node, "time" );
			cam->mSize = xml.GetFloat( node, "size" );
			cam->mPosition.x = xml.GetFloat( node, "positionx" );
			cam->mPosition.y = xml.GetFloat( node, "positiony" );
			cam->mDuration = xml.GetFloat( node, "duration" );
			
			mActionsToExecute.push_back( (CCutsceneAction*) cam );
		}

		for ( node=xml.GetChild(0,"playSound"); node; node=xml.GetSibl(node,"playSound") )
		{
			CCutsceneSound * snd = new CCutsceneSound;
			snd->mTime = xml.GetFloat( node, "time" );
            snd->mSoundPath = StringUtils::ConvertToString(gLocalizator.Localize(xml.GetString( node, "path" )));
	
			mActionsToExecute.push_back( (CCutsceneAction*) snd );
		}

		for ( node=xml.GetChild(0,"showText"); node; node=xml.GetSibl(node,"showText") )
		{
			CCutsceneText * txt = new CCutsceneText;
			txt->mTime = xml.GetFloat( node, "time" );
            txt->mContent = gLocalizator.Localize(xml.GetString( node, "content" ));
			txt->mDuration = xml.GetFloat( node, "duration" );
				
			mActionsToExecute.push_back( (CCutsceneAction*) txt );
		}

		std::sort( mActionsToExecute.begin(), mActionsToExecute.end(), CCutsceneSound::Compare );
		mState = CUTSCENE_PLAYING;
		mImageBox->SetVisible( true );
		mTextArea->SetVisible( true );
		gGame.SetFreezed( true );
	}
}

void CCutscenePlayer::StopCutscene()
{
	for ( unsigned i = 0; i < mActionsToExecute.size(); i++ )	delete mActionsToExecute[i];
	for ( unsigned i = 0; i < mActionsInProgress.size(); i++ )	delete mActionsInProgress[i];

	mActionsToExecute.clear();
	mActionsInProgress.clear();
	mTime = 0.0f;
	mState = CUTSCENE_EMPTY;
	mImageBox->ReleaseImages();
	mImageBox->SetVisible( false );
	mTextArea->SetText( L"" );
	mTextArea->SetVisible( false );
	gGame.SetFreezed( false );
}


void CCutscenePlayer::ResetGuiControls()
{
    mImageBox = NULL;
    mTextArea = NULL;
}

void CCutscenePlayer::InitGuiControls()
{
    if (!mImageBox)
    {
	    mImageBox = gGUI.CreateImageBox( "cutscene-image", true, Z_GUI2 );
	    mImageBox->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
	    mImageBox->SetVisible( false );
    }

    if (!mTextArea)
    {
        mTextArea = mImageBox->CreateTextArea( "cutscene-text" );
        mTextArea->SetPosition( 5.0f, 80.0f, 90.0f, 18.0f );
        mTextArea->SetPadding( 2.0f, 2.0f, 2.0f, 2.0f );
        mTextArea->SetFont( gLocalizator.GetFont(GUI::FONT_DIALOG), 14 );
        mTextArea->SetBackgroundImage( "data/cutscene/bg.png" );
        mTextArea->SetColor( sf::Color::White );
        mTextArea->SetVisible( false );
    }
}


void CCutscenePlayer::FrameStarted(float secondsPassed)
{
	CCutsceneImage * img;
	CCutsceneCamera * cam;
	CCutsceneSound * snd;
	CCutsceneText * txt;

	if ( mState == CUTSCENE_PLAYING )
	{
		mTime += secondsPassed;

		// wykonaj nowe akcje
		while ( mActionsToExecute.size() > 0 && mTime > mActionsToExecute.back()->mTime )
		{
			switch ( mActionsToExecute.back()->mType )
			{
			case CUTSCENE_IMAGE:
				img = static_cast<CCutsceneImage*> ( mActionsToExecute.back() );
				mImageBox->ReleaseImages();
				mImageBox->AddImageToSequence( img->mImagePath );
				break;
			case CUTSCENE_CAMERA:
				break;
			case CUTSCENE_SOUND:
				snd = static_cast<CCutsceneSound*> ( mActionsToExecute.back() );
				gAudioManager.GetSoundPlayer().Play( gResourceManager.GetSound( snd->mSoundPath ) );
				break;
			case CUTSCENE_TEXT:
				txt = static_cast<CCutsceneText*> ( mActionsToExecute.back() );
				mTextArea->SetText( txt->mContent );
				break;
			}

			mActionsInProgress.push_back( mActionsToExecute.back() );
			mActionsToExecute.pop_back();
		}

		// kontynuuj wykonywanie akcji 'dlugoterminowych'

		for ( unsigned i = 0; i < mActionsInProgress.size(); )
		{
			switch ( mActionsInProgress[i]->mType )
			{
			case CUTSCENE_CAMERA:
                {
				    cam = static_cast<CCutsceneCamera*> (mActionsInProgress[i]);
				    float lerpFactor = cam->mTime + cam->mDuration - mTime;
				    lerpFactor = ( lerpFactor <= 0.0f ? 1.0f : secondsPassed / lerpFactor );
				    mImageBox->SetViewSize( Lerp( mImageBox->GetViewSize(), cam->mSize, lerpFactor ) );
				    mImageBox->SetViewPosition( Lerp( mImageBox->GetViewPosition(), cam->mPosition, lerpFactor ) );
				    break;
                }
			case CUTSCENE_TEXT:
				txt = static_cast<CCutsceneText*> (mActionsInProgress[i]);
				if ( mTime > txt->mTime + txt->mDuration && mTextArea->GetText() == txt->mContent)
				{
					mTextArea->SetText( L"" );
				}
				break;
            default:
                break;
			}

			if ( mTime > (mActionsInProgress[i])->mTime + (mActionsInProgress[i])->mDuration )	
			{
				delete mActionsInProgress[i];
				mActionsInProgress.erase( mActionsInProgress.begin() + i );
			}
			else ++i;
		}

		// wykryj koniec cutscenki
		if ( mActionsToExecute.size() + mActionsInProgress.size() == 0 )
		{
			StopCutscene();
		}
	}
}

void CCutscenePlayer::KeyPressed(const sf::Event::KeyEvent &e)
{
	if ( (e.Code == sf::Key::Escape) && (mState != CUTSCENE_EMPTY) )
	{
		StopCutscene();
	}
}

