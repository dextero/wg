#include "CHud.h"

#include "CGUIObject.h"
#include "CWindow.h"
#include "CProgressBar.h"
#include "CImageBox.h"
#include "CTextArea.h"
#include "CRoot.h"
#include "../Utils/StringUtils.h"
#include "../Utils/MathsFunc.h"
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
using namespace std;

/* ================ CHud ================= */

CHud::CHud(unsigned playerNumber, SHudDesc& hudDesc)
{
	mPlayerNumber = playerNumber;
	mGuiNamePrefix = std::string("pl") + StringUtils::ToString(playerNumber) + "-";
	mHud = gGUI.CreateWindow( mGuiNamePrefix + "hud" );

	mHud->SetPosition(
		hudDesc.position[GUI::UNIT_PERCENT].x,
		hudDesc.position[GUI::UNIT_PERCENT].y,
		hudDesc.size[GUI::UNIT_PERCENT].x,
		hudDesc.size[GUI::UNIT_PERCENT].y,
		GUI::UNIT_PERCENT
	);

	mHud->SetPosition(
		hudDesc.position[GUI::UNIT_PIXEL].x,
		hudDesc.position[GUI::UNIT_PIXEL].y,
		hudDesc.size[GUI::UNIT_PIXEL].x,
		hudDesc.size[GUI::UNIT_PIXEL].y,
		GUI::UNIT_PIXEL
	);
}

CHud::~CHud()
{
}

void CHud::Show()
{
	mHud->SetVisible(true);
}

void CHud::Hide()
{
	mHud->SetVisible(false);
}

void CHud::Release()
{
	mHud->Remove();
	delete this;
}

CHud* CHud::CreateHud(unsigned playerNumber, SHudDesc& hudDesc)
{
	if (hudDesc.hudType == "classic") return new CClassicHud(playerNumber, hudDesc);
	else return new CModernHud(playerNumber, hudDesc);
}

/* ================ CClassicHud ================= */

CClassicHud::CClassicHud(unsigned playerNumber, SHudDesc& hudDesc)
:	CHud(playerNumber, hudDesc)
{
	mHP = mHud->CreateProgressBar( mGuiNamePrefix + "hp" );
	mHP->SetBorderImage( hudDesc.path[HUDDESC_HP_BG] );
	mHP->SetProgressImage( hudDesc.path[HUDDESC_HP] );
	mHP->SetPosition( 0.0f, 0.0f, 100.0f, 25.0f );
	mHP->SetMinClippingPlane( 270.0f, sf::Vector2f(0.0f,0.0f) );
	mHP->SetMaxClippingPlane( 270.0f, sf::Vector2f(100.0f,0.0f) );

	mMana = mHud->CreateProgressBar( mGuiNamePrefix + "mana" );
	mMana->SetBorderImage( hudDesc.path[HUDDESC_MANA_BG] );
	mMana->SetProgressImage( hudDesc.path[HUDDESC_MANA] );
	mMana->SetPosition( 0.0f, 37.5f, 100.0f, 25.0f );
	mMana->SetMinClippingPlane( 270.0f, sf::Vector2f(0.0f,0.0f) );
	mMana->SetMaxClippingPlane( 270.0f, sf::Vector2f(100.0f,0.0f) );

	mXP = mHud->CreateProgressBar( mGuiNamePrefix + "xp" );
	mXP->SetBorderImage( hudDesc.path[HUDDESC_XP_BG] );
	mXP->SetProgressImage( hudDesc.path[HUDDESC_XP] );
	mXP->SetPosition( 0.0f, 75.0f, 100.0f, 25.0f );
	mXP->SetMinClippingPlane( 270.0f, sf::Vector2f(0.0f,0.0f) );
	mXP->SetMaxClippingPlane( 270.0f, sf::Vector2f(100.0f,0.0f) );
}

CClassicHud::~CClassicHud()
{
}

void CClassicHud::Update(float dt)
{
	if (CPlayer* player = gPlayerManager.GetPlayerByNumber(mPlayerNumber))
	{
		mHP->SetProgress( player->GetHP() / player->GetStats()->GetCurrAspect(aMaxHP) );
		mMana->SetProgress( player->GetStats()->GetMana() / player->GetStats()->GetCurrAspect(aMaxMana) );
		mXP->SetProgress( (player->GetTotalXP() - player->XPPreviouslyRequired()) / (player->XPRequired() - player->XPPreviouslyRequired()) );
	}
}

/* ================ CModernHud ================= */

CModernHud::CModernHud(unsigned playerNumber, SHudDesc& hudDesc)
:	CHud(playerNumber, hudDesc),
	mHP		( NULL ),
	mMana	( NULL ),
	mManaFlr( NULL ),
	mXP		( NULL ),
	mAvatar	( NULL ),
	mHudFg	( NULL ),
	mManaFlaringTime( 0.0f )
{
	CImageBox * circle1 = mHud->CreateImageBox( mGuiNamePrefix + "hud-circle1" );
	circle1->AddImageToSequence( hudDesc.path[HUDDESC_CIRCLE1] );
	circle1->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
	circle1->SetRotationSpeed( 200.0f );

    CImageBox * hudBg = mHud->CreateImageBox( mGuiNamePrefix + "hud-bg" );
	hudBg->AddImageToSequence( hudDesc.path[HUDDESC_BG] );
	hudBg->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

    mAvatar = hudBg->CreateImageBox( mGuiNamePrefix + "avatar" );
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR0] );
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR1] );
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR2] );
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR3] );
    mAvatar->AddImageToSequence( hudDesc.path[HUDDESC_AVATAR4] );
    mAvatar->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	CImageBox * circle2 = mHud->CreateImageBox( mGuiNamePrefix +  "hud-circle2" );
	circle2->AddImageToSequence( hudDesc.path[HUDDESC_CIRCLE2] );
	circle2->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
	circle2->SetRotationSpeed( -100.0f );

	CImageBox * circle3 = mHud->CreateImageBox( mGuiNamePrefix + "hud-circle3" );
	circle3->AddImageToSequence( hudDesc.path[HUDDESC_CIRCLE3] );
	circle3->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
	circle3->SetRotationSpeed( 100.0f );		
	
	mHP = hudBg->CreateProgressBar( mGuiNamePrefix + "hp" );
	mHP->SetProgressImage( hudDesc.path[HUDDESC_HP] );
	mHP->SetMaxClippingPlane( 240.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mHP->SetMinClippingPlane( 60.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mHP->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	mMana = hudBg->CreateProgressBar( mGuiNamePrefix + "mana" );
	mMana->SetProgressImage( hudDesc.path[HUDDESC_MANA] );
	mMana->SetMaxClippingPlane( 240.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mMana->SetMinClippingPlane( 60.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mMana->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	mManaFlr = hudBg->CreateProgressBar( mGuiNamePrefix + "mana-flared" );
	mManaFlr->SetProgressImage(hudDesc.path[HUDDESC_MANA_FLARED] );
	mManaFlr->SetMaxClippingPlane( 240.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mManaFlr->SetMinClippingPlane( 60.0f, sf::Vector2f( 55.0f, 45.0f ) );
	mManaFlr->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	mXP = hudBg->CreateProgressBar( mGuiNamePrefix + "xp" );
	mXP->SetProgressImage( hudDesc.path[HUDDESC_XP] );
	mXP->SetMaxClippingPlane( 85.0f, sf::Vector2f( 50.0f, 50.0f ) );
	mXP->SetMinClippingPlane( 225.0f, sf::Vector2f( 50.0f, 50.0f ) );
	mXP->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );

	mHudFg = mHP->CreateImageBox( mGuiNamePrefix + "hud-fg" );
	mHudFg->AddImageToSequence( hudDesc.path[HUDDESC_FG] );
	mHudFg->AddImageToSequence( hudDesc.path[HUDDESC_FG_FLARED] );
	mHudFg->SetPosition(0.0f, 0.0f, 100.0f, 100.0f);

	mHudFGText = mHP->CreateTextArea( mGuiNamePrefix + "hud-fg-text");
	mHudFGText->SetPosition( 0.0f, 0.0f, 100.0f, 30.0f );
	mHudFGText->SetCenter(true);
	mHudFGText->SetFont(gLocalizator.GetFont(GUI::FONT_MESSAGE),20);
	mHudFGText->SetColor(sf::Color::Green);
	mHudFGText->SetVisible(false);

	for ( unsigned i = 0; i < 10; i++ )
	{
		CImageBox* img = mHud->CreateImageBox( mGuiNamePrefix + "seq-" + StringUtils::ToString(i) );
		img->SetVisible( false );

		img->AddImageToSequence( hudDesc.path[HUDDESC_SEQ_Q] );
		img->AddImageToSequence( hudDesc.path[HUDDESC_SEQ_W] );
        img->AddImageToSequence( hudDesc.path[HUDDESC_SEQ_E] );
        img->AddImageToSequence( hudDesc.path[HUDDESC_SEQ_R] );

		mSeqViewer.push_back( img );
	}
}

CModernHud::~CModernHud()
{
}

void CModernHud::Update(float dt)
{
	if ( CPlayer * player = gPlayerManager.GetPlayerByNumber(mPlayerNumber) )
	{
		const float maxManaFlaringTime = 0.3f;
		float halfMaxManaFlaringTime = maxManaFlaringTime * 0.5f;
		float manaAlpha;

		if (mManaFlaringTime > 0.0f)
		{
			mManaFlaringTime -= dt;
			manaAlpha = fabs(mManaFlaringTime - halfMaxManaFlaringTime) / halfMaxManaFlaringTime;
			if (manaAlpha > 1.0f)
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
			const std::map<std::string,int>& keys = gBindManagerByPlayer(mPlayerNumber)->GetKeyBindings();
			mHudFGText->SetText(StringUtils::ConvertToWString(KeyStrings::KeyToString( keys.find("Abilities")->second )));
		} else {
			mHudFg->SetSequenceState(0.0f);
			mHudFGText->SetVisible(false);
			mHudFGText->SetText(L"");
		}

        float idleTime = player->GetController()->GetCurrentSequenceIdleTime();
        //static bool last = false;
		CAbilityPerformer::EActorAbilityState abiState = player->GetAbilityPerformer().GetAbilityState();
		CAbility * activeAbility = NULL; 
		if (player->GetAbilityPerformer().GetActiveAbility() != NULL)
			activeAbility = player->GetAbilityPerformer().GetActiveAbility()->ability;
		bool useLast;
		if (player->GetController()->GetCurrentKeySequence().size() > 0){
			useLast = false;
		} else if ((activeAbility == player->GetController()->GetLastAbility()) &&
			(abiState == CAbilityPerformer::asCasting||
			abiState == CAbilityPerformer::asDelay)){
			useLast = true;
		} else if (abiState == CAbilityPerformer::asIdle ||
			abiState == CAbilityPerformer::asReadying){
			useLast = false;
		} else {
			useLast = true;
		}
		if (useLast){
			//if (!last){
			//	fprintf(stderr,"switching to last\n");
			//}
			//last = true;
			mSeq = player->GetController()->GetLastKeySequence();
		} else {		
			//if (last) fprintf(stderr,"switching to current\n");
			//last = false;
			int prevSize = mSeq.size();
			mSeq = player->GetController()->GetCurrentKeySequence();
			if ((prevSize > 0) && (mSeq.size()==0)){
				EAbilityResult result = player->GetController()->GetLastAbilityResult();
				switch (result){
					case arNoMana:
						mManaFlaringTime = maxManaFlaringTime;
						//fprintf(stderr,"manaFlaringTime set!\n");
						break;
					default:
						break;
				}
			}
		}

        const float startRot = 60.0f;
        const float step = 15.0f;
        const float imgHalfSize = 5.0f;
        const float radius = 50.0f;
		if (idleTime < 1.5f){
			bool isSingleKeyCombination = ((abiState == CAbilityPerformer::asCasting) || (abiState == CAbilityPerformer::asReadying)
				|| (abiState == CAbilityPerformer::asDelay)) && (mSeq.size() == 1);
			for (unsigned int i = 0; i < 10; i++){
				if (i >= mSeq.size() || isSingleKeyCombination)
					mSeqViewer[i]->SetVisible(false);
				else if (mHud->IsVisible()) {
					mSeqViewer[i]->SetVisible(true);
					mSeqViewer[i]->SetSequenceState(mSeq[i]);
					float x = 50.0f;
					float y = 50.0f;
					float s = step * idleTime / CPlayerController::MaxSequenceIdleTime;
					sf::Vector2f r = radius * RotationToVector(startRot - s - step * (mSeq.size() - i));
					mSeqViewer[i]->SetPosition(x - r.x - imgHalfSize, y - r.y - imgHalfSize, imgHalfSize * 2.0f, imgHalfSize * 2.0f);
				}
			}
		} else {
			for (unsigned int i = 0; i < 10; i++)
				mSeqViewer[i]->SetVisible(false);
		}
	}
	else
		Hide();
}

void CModernHud::Show()
{
	mHud->SetVisible( true );
	mSeq.clear();
}
