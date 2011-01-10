#include "CCheckBox.h"
#include "CRoot.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../Audio/CAudioManager.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

using namespace GUI;

void CCheckBox::SetImage( const std::string& imgTrue, const std::string& imgFalse )
{
	if ( !mTrueSprite )
		mTrueSprite = gDrawableManager.CreateHudSprite( mZIndex );
	
	if ( sf::Image* img = gResourceManager.GetImage(imgTrue) )
		mTrueSprite->GetSFSprite()->SetImage( *img );

	if ( !mFalseSprite )
		mFalseSprite = gDrawableManager.CreateHudSprite( mZIndex );
	
	if ( sf::Image* img = gResourceManager.GetImage(imgFalse) )
		mFalseSprite->GetSFSprite()->SetImage( *img );
}

CCheckBox::CCheckBox(const std::string &name, CGUIObject *parent, unsigned zindex)
:	CGUIObject(name, GUI_CHECK_BOX, parent, zindex), 
	mTrueSprite(NULL), 
	mFalseSprite(NULL), 
	mChecked(false)
{
	mZIndexCount = 1;
}

CCheckBox::~CCheckBox()
{
	if ( mTrueSprite )	gDrawableManager.DestroyDrawable( mTrueSprite );
	if ( mFalseSprite ) gDrawableManager.DestroyDrawable( mFalseSprite );
}

bool CCheckBox::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	switch ( e )
	{
	case MOUSE_PRESSED_LEFT:
		gGUI.SetActiveObject( NULL );
		mChecked = !mChecked;
		gAudioManager.GetSoundPlayer().Play(gResourceManager.GetSound("data/sounds/GUI_klik.ogg"));
		break;
    default:
        break;
	}

	return false;
}

void CCheckBox::UpdateSprites( float secondsPassed )
{
	if ( mTrueSprite && mTrueSprite->GetSFSprite()->GetImage() )
	{
		mTrueSprite->SetVisible( mIsVisible && mChecked );
		if ( mIsVisible && mChecked )
		{
			mTrueSprite->GetSFSprite()->SetPosition( mGlobalPosition );
			mTrueSprite->GetSFSprite()->Resize( mGlobalSize );
			mTrueSprite->SetClipRect( mClippingRect );
		}
	}

	if ( mFalseSprite && mFalseSprite->GetSFSprite()->GetImage() )
	{
		mFalseSprite->SetVisible( mIsVisible && !mChecked );
		if ( mIsVisible && !mChecked )
		{
			mFalseSprite->GetSFSprite()->SetPosition( mGlobalPosition );
			mFalseSprite->GetSFSprite()->Resize( mGlobalSize );
			mFalseSprite->SetClipRect( mClippingRect );
		}
	}
}

