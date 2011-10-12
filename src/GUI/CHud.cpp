#include "CHud.h"

#include "CGUIObject.h"
#include "CWindow.h"
#include "CProgressBar.h"
#include "CImageBox.h"
#include "CTextArea.h"
#include "CRoot.h"
#include "../Utils/StringUtils.h"
#include "../Utils/Maths.h"
#include "../Utils/KeyStrings.h"

#include "../Logic/CPlayerManager.h"
#include "../Logic/CPlayer.h"
#include "../Logic/Abilities/CAbility.h"
#include "../Logic/Abilities/CAbilityPerformer.h"
#include "../Input/CPlayerController.h"
#include "../Logic/Factory/CPlayerTemplate.h"
#include "../ResourceManager/CResourceManager.h"
#include "../CGameOptions.h"
#include "Localization/CLocalizator.h"
#include "Localization/GeneralKeys.h"
#include "../Input/CBindManager.h"

#undef CreateWindow

using namespace GUI;

CHud::CHud()
:	mPlayerNumber( 0 ),
	mHud	( NULL ),
	mHP		( NULL ),
	mMana	( NULL ),
	mManaFlr( NULL ),
	mXP		( NULL ),
	mAvatar	( NULL ),
	mHudFg	( NULL ),
	mManaFlaringTime( 0.0f )
{
}

CHud::~CHud()
{
}

void CHud::Init(size_t playerNumber)
{
	std::string prefix = std::string("pl") + StringUtils::ToString(playerNumber) + "-";

	mPlayerNumber = playerNumber;

	mHud = gGUI.CreateWindow( prefix + "hud" );

    UpdatePosition();

    SHudDesc hudDesc;
    CPlayer* player = gPlayerManager.GetPlayerByNumber(playerNumber);
    if (!player)
    {
        //fprintf(stderr, "WARNING: CHud::Init: invalid player number: %i (not yet initialized?)\n", playerNumber);
        return;
    }
    else
        hudDesc = player->GetHudDesc();

	CImageBox * circle1 = mHud->CreateImageBox( prefix + "hud-circle1" );
	circle1->AddImageToSequence( hudDesc.path[HUDDESC_CIRCLE1] );
	circle1->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
	circle1->SetRotationSpeed( 200.0f );

    CImageBox * hudBg = mHud->CreateImageBox( prefix + "hud-bg" );
	hudBg->AddImageToSequence( hudDesc.path[HUDDESC_BG] );
	hudBg->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

    mAvatar = hudBg->CreateImageBox("avatar");
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR0] );
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR1] );
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR2] );
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR3] );
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR4] );
    mAvatar->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	CImageBox * circle2 = mHud->CreateImageBox( prefix +  "hud-circle2" );
	circle2->AddImageToSequence( hudDesc.path[HUDDESC_CIRCLE2] );
	circle2->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
	circle2->SetRotationSpeed( -100.0f );

	CImageBox * circle3 = mHud->CreateImageBox( prefix + "hud-circle3" );
	circle3->AddImageToSequence( hudDesc.path[HUDDESC_CIRCLE3] );
	circle3->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
	circle3->SetRotationSpeed( 100.0f );		
	
	mHP = hudBg->CreateProgressBar( prefix + "hp" );
	mHP->SetProgressImage( hudDesc.path[HUDDESC_HP] );
	mHP->SetMaxClippingPlane( 240.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mHP->SetMinClippingPlane( 60.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mHP->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	mMana = hudBg->CreateProgressBar( prefix + "mana" );
	mMana->SetProgressImage( hudDesc.path[HUDDESC_MANA] );
	mMana->SetMaxClippingPlane( 240.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mMana->SetMinClippingPlane( 60.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mMana->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	mManaFlr = hudBg->CreateProgressBar( prefix + "mana-flared" );
	mManaFlr->SetProgressImage(hudDesc.path[HUDDESC_MANA_FLARED] );
	mManaFlr->SetMaxClippingPlane( 240.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mManaFlr->SetMinClippingPlane( 60.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mManaFlr->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	mXP = hudBg->CreateProgressBar( prefix +  "xp" );
	mXP->SetProgressImage( hudDesc.path[HUDDESC_XP] );
	mXP->SetMaxClippingPlane( 85.0f, sf::Vector2f( 50.0f, 50.0f ) );
	mXP->SetMinClippingPlane( 225.0f, sf::Vector2f( 50.0f, 50.0f ) );
	mXP->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	mHudFg = mHP->CreateImageBox( prefix + "hud-fg" );
	mHudFg->AddImageToSequence( hudDesc.path[HUDDESC_FG] );
	mHudFg->AddImageToSequence( hudDesc.path[HUDDESC_FG_FLARED] );
	mHudFg->SetPosition(0.0f, 0.0f, 100.0f, 100.0f);

	mHudFGText = mHP->CreateTextArea( prefix + "hud-fg-text");
	mHudFGText->SetPosition( 0.0f, 0.0f, 100.0f, 30.0f );
	mHudFGText->SetCenter(true);
	mHudFGText->SetFont(gGUI.GetFontSetting("FONT_LEVEL_TEXT"));
	mHudFGText->SetColor(sf::Color::Green);
	mHudFGText->SetVisible(false);

	//for ( unsigned i = 0; i < 10; i++ )
	//{
	//	CImageBox* img = mHud->CreateImageBox( prefix + "seq-" + StringUtils::ToString(i) );
	//	img->SetVisible( false );

	//	img->AddImageToSequence( hudDesc.path[HUDDESC_SEQ_Q] );
	//	img->AddImageToSequence( hudDesc.path[HUDDESC_SEQ_W] );
 //       img->AddImageToSequence( hudDesc.path[HUDDESC_SEQ_E] );
 //       img->AddImageToSequence( hudDesc.path[HUDDESC_SEQ_R] );

	//	mSeqViewer.push_back( img );
	//}
}

void CHud::Update(float dt)
{
	if ( CPlayer * player = gPlayerManager.GetPlayerByNumber(mPlayerNumber) )
	{
		float manaAlpha;
		float halfMaxManaFlaringTime = maxManaFlaringTime * 0.5f;

		if (mManaFlaringTime > 0.0f)
		{
			mManaFlaringTime -= dt;
			manaAlpha = fabs(mManaFlaringTime - halfMaxManaFlaringTime) / halfMaxManaFlaringTime;
			if ( manaAlpha > 1.0f )
				manaAlpha = 1.0f;
		} 
		else 
			manaAlpha = 1.0f;

		mMana->SetProgress( player->GetStats()->GetMana() / player->GetStats()->GetCurrAspect(aMaxMana) );
		mMana->SetAlpha(manaAlpha);
		mManaFlr->SetAlpha(1.0f - manaAlpha);
		mAvatar->SetSequenceState( 1.0f - player->GetHP() / player->GetStats()->GetCurrAspect(aMaxHP) );
		mHP->SetProgress( player->GetHP() / player->GetStats()->GetCurrAspect(aMaxHP) );
		mXP->SetProgress( (player->GetTotalXP() - player->XPPreviouslyRequired()) / (player->XPRequired() - player->XPPreviouslyRequired()) );

		bool isLevelUp = player->GetSkillPoints() > 0;
		if (isLevelUp){
			mHudFg->SetSequenceState(1.0f);
			if (mHudFg->IsVisible()) mHudFGText->SetVisible(true);
			const std::map<std::string,int>& keys = gBindManagerByPlayer((unsigned int)mPlayerNumber)->GetKeyBindings();
			mHudFGText->SetText(StringUtils::ConvertToWString(KeyStrings::KeyToString( keys.find("Abilities")->second )));
		} else {
			mHudFg->SetSequenceState(0.0f);
			mHudFGText->SetVisible(false);
			mHudFGText->SetText(L"");
		}

        //float idleTime = player->GetController()->GetCurrentSequenceIdleTime();
        //static bool last = false;
//		CAbilityPerformer::EActorAbilityState abiState = player->GetAbilityPerformer().GetAbilityState();
//		CAbility * activeAbility = NULL; 
//		if (player->GetAbilityPerformer().GetActiveAbility() != NULL)
//			activeAbility = player->GetAbilityPerformer().GetActiveAbility()->ability;
//		bool useLast;
//		if (player->GetController()->GetCurrentKeySequence().size() > 0){
//			useLast = false;
//		} else if ((activeAbility == player->GetController()->GetLastAbility()) &&
//			(abiState == CAbilityPerformer::asCasting||
//			abiState == CAbilityPerformer::asDelay)){
//			useLast = true;
//		} else if (abiState == CAbilityPerformer::asIdle ||
//			abiState == CAbilityPerformer::asReadying){
//			useLast = false;
//		} else {
//			useLast = true;
//		}
//		if (useLast){
			//if (!last){
			//	fprintf(stderr,"switching to last\n");
			//}
			//last = true;
//			mSeq = player->GetController()->GetLastKeySequence();
//		} else {		
			//if (last) fprintf(stderr,"switching to current\n");
			//last = false;
//			int prevSize = mSeq.size();
//			mSeq = player->GetController()->GetCurrentKeySequence();
//			if ((prevSize > 0) && (mSeq.size()==0)){
				EAbilityResult result = player->GetController()->GetLastAbilityResult();
				switch (result){
					case arNoMana:
						mManaFlaringTime = maxManaFlaringTime;
						//fprintf(stderr,"manaFlaringTime set!\n");
						break;
					default:
						break;
				}
//			}
//		}
	}
	else
		Hide();
}

void CHud::Show()
{
    UpdatePosition();
	mHud->SetVisible( true );
	//ResetSequences();
}

void CHud::Hide()
{
	mHud->SetVisible( false );
}

void CHud::UpdatePosition()
{
	if ( mPlayerNumber == 0 )
        mHud->SetPosition( 0.0f, 0.0f, 180.0f, 180.0f, UNIT_PIXEL, false );
	else
        mHud->SetPosition( gGameOptions.GetWidth() - 160.0f, 0.0f, 180.0f, 180.0f, UNIT_PIXEL, false );
}
