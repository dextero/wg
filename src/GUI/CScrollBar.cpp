#include "CScrollBar.h"
#include "CRoot.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

using namespace GUI;

void CScrollBar::SetBackgroundImage( const std::string& filename )
{
	if ( !mBackgroundSprite )
		mBackgroundSprite = gDrawableManager.CreateHudSprite( mZIndex );
	
	if ( sf::Image* img = gResourceManager.GetImage(filename) )
		mBackgroundSprite->GetSFSprite()->SetImage( *img );
}

void CScrollBar::SetHandleImage( const std::string& filename )
{
	if ( !mHandleSprite )
		mHandleSprite = gDrawableManager.CreateHudSprite( mZIndex - 1 );
	
	if ( sf::Image* img = gResourceManager.GetImage(filename) )
		mHandleSprite->GetSFSprite()->SetImage( *img );
}

CScrollBar::CScrollBar(const std::string &name, CGUIObject *parent, unsigned zindex)
:	CGUIObject(name, GUI_SCROLL_BAR, parent, zindex), mBackgroundSprite(NULL), mHandleSprite(NULL), 
	mState(0.0f), 
	mStatesCount(100), 
	mHandleSize(0.5f),
	mOrientation(ORIENTATION_X),
	mActive(false),
    mSlideCallbackParams(NULL)
{
	mZIndexCount = 2;
}

CScrollBar::~CScrollBar()
{
	if ( mBackgroundSprite )	gDrawableManager.DestroyDrawable( mBackgroundSprite );
	if ( mHandleSprite )		gDrawableManager.DestroyDrawable( mHandleSprite );
}

bool CScrollBar::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	switch ( e )
	{
	case MOUSE_PRESSED_LEFT:
		mActive = true;
		gGUI.SetActiveObject( (CGUIObject*) this );
	case MOUSE_MOVED:
		UpdateState( x, y );
		break;
	case MOUSE_RELEASED_LEFT:
	case MOUSE_UNCLICK:
		mActive = false;
		gGUI.SetActiveObject( NULL );
		break;
    default:
        break;
	}

	return false;
}

void CScrollBar::UpdateSprites( float secondsPassed )
{
	if ( mBackgroundSprite && mBackgroundSprite->GetSFSprite()->GetImage() )
	{
		mBackgroundSprite->SetVisible( mIsVisible );

		if ( mIsVisible ) 
		{
			sf::Vector2i imgSize(mBackgroundSprite->GetSFSprite()->GetImage()->GetWidth(),mBackgroundSprite->GetSFSprite()->GetImage()->GetHeight());
            
			mBackgroundSprite->GetSFSprite()->SetPosition( mGlobalPosition.x, mGlobalPosition.y );
			mBackgroundSprite->GetSFSprite()->SetScale( mGlobalSize.x / imgSize.x, mGlobalSize.y / imgSize.y);
			mBackgroundSprite->SetClipRect( mClippingRect );
		}
	}

	if ( mHandleSprite && mHandleSprite->GetSFSprite()->GetImage() )
	{
		mHandleSprite->SetVisible( mIsVisible );

		if ( mIsVisible ) 
		{
			sf::Vector2f position;
			sf::Vector2f size;
			GetHandleData( position, size );

			mHandleSprite->GetSFSprite()->SetPosition( position );
			mHandleSprite->GetSFSprite()->Resize( size );
			mHandleSprite->SetClipRect( mClippingRect );
		}
	}
}

void CScrollBar::UpdateState(float mouseX, float mouseY)
{
	float x = mouseX;
	float y = mouseY;

	if ( mActive )
	{
		if ( mOrientation == ORIENTATION_X )
		{
			float handleW = mInternalRect.GetWidth() * mHandleSize;
			mState = Maths::Clamp( (x - mInternalRect.Left - handleW * 0.5f) / (mInternalRect.GetWidth() - handleW), 0.0f, 1.0f );
		}
		else
		{
			float handleH = mInternalRect.GetHeight() * mHandleSize;
			mState = Maths::Clamp( (y - mInternalRect.Top - handleH * 0.5f) / (mInternalRect.GetHeight() - handleH), 0.0f, 1.0f );
		}

        if (!mSlideCallback.empty())
            mSlideCallback();
        if (!mSlideParamCallback.empty())
            mSlideParamCallback(mSlideCallbackParams, (unsigned)(GetState() * (GetStatesCount() - 1)));
	}
}

void CScrollBar::GetHandleData(sf::Vector2f &position, sf::Vector2f &size)
{
	if ( mOrientation == ORIENTATION_X )
	{
		size.x = mInternalRect.GetWidth() * mHandleSize;
		size.y = mInternalRect.GetHeight();
		position.x = Maths::Lerp( mInternalRect.Left, mInternalRect.Right - size.x, GetState() );
		position.y = mInternalRect.Top;
	}
	else
	{
		size.x = mInternalRect.GetWidth();
		size.y = mInternalRect.GetHeight() * mHandleSize;
		position.x = mInternalRect.Left;
		position.y = Maths::Lerp( mInternalRect.Top, mInternalRect.Bottom - size.y, GetState() );
	}
}

