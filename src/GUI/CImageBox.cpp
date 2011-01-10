#include "CImageBox.h"
#include "CRoot.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../Utils/MathsFunc.h"
#include <iostream>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

using namespace GUI;

void CImageBox::AddImageToSequence(const std::string &filename)
{
	if ( filename != "" )
	{
		mImages.push_back( filename );
		mUpdate = true;
	}
}

void CImageBox::SetSequenceState(float state)
{
	float newState = Maths::Clamp( state, 0.0f, 0.999999f );
	if ( mState != newState )
		mUpdate = true;
	mState = newState;
}

void CImageBox::SetSequenceState(int img)
{
	SetSequenceState( float(img)/float(mImages.size()-1) );
}

void CImageBox::ReleaseImages()
{
    // chrzanic DropResource(), obrazki z GUI mi przez to znikaja
	//for ( unsigned i = 0; i < mImages.size(); i++ )
	//	gResourceManager.DropResource( mImages[i] );
	mImages.clear();
	mUpdate = true;
}

CImageBox::CImageBox(const std::string &name, GUI::CGUIObject *parent, unsigned zindex)
:	CGUIObject(name, GUI_IMAGE_BOX, parent, zindex), 
	mState( 0.0f ), 
	mRotationSpeed( 0.0f ), 
	mViewSize( 1.0f ),
	mViewPosition( 0.0f, 0.0f ),
	mUpdate( true )
{
	mZIndexCount = 2;
	mBackgroundSprite = gDrawableManager.CreateHudSprite( mZIndex );
	mForegroundSprite = gDrawableManager.CreateHudSprite( mZIndex - 1 );
}

CImageBox::~CImageBox()
{
	if (mBackgroundSprite)	gDrawableManager.DestroyDrawable( mBackgroundSprite );
	if (mForegroundSprite)	gDrawableManager.DestroyDrawable( mForegroundSprite );
	ReleaseImages();
}

bool CImageBox::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	if (e == MOUSE_PRESSED_LEFT)
		gGUI.SetActiveObject( NULL );

	return false;
}

void CImageBox::UpdateSprites( float secondsPassed )
{
	if ( mBackgroundSprite && mForegroundSprite )
	{
		if ( mUpdate && mImages.size() > 0 )
		{
			unsigned bgImage = (unsigned) ( mState * (mImages.size()-1) );
			unsigned fgImage = bgImage + 1 < mImages.size() ? bgImage + 1 : mImages.size() - 1;
			float alpha = mState * (mImages.size()-1) - bgImage;

			mBackgroundSprite->GetSFSprite()->SetImage( *gResourceManager.GetImage( mImages[bgImage] ) );
			mForegroundSprite->GetSFSprite()->SetImage( *gResourceManager.GetImage( mImages[fgImage] ) );
			mForegroundSprite->GetSFSprite()->SetColor( sf::Color( 255, 255, 255,sf::Uint8(255*alpha) ) ); 
			mUpdate = false;
		}

		mBackgroundSprite->SetVisible( mIsVisible );
		mForegroundSprite->SetVisible( mIsVisible );

		if ( mIsVisible )
		{
			if ( mBackgroundSprite->GetSFSprite()->GetImage() ) 
			{
				sf::Vector2i imgSize( mBackgroundSprite->GetSFSprite()->GetImage()->GetWidth(), mBackgroundSprite->GetSFSprite()->GetImage()->GetHeight() );
				sf::Vector2f viewSize( (float) imgSize.x * mViewSize, (float) imgSize.y * mViewSize );

				sf::IntRect subRect;
				subRect.Left =	(int) ( ( (float) imgSize.x - viewSize.x) * mViewPosition.x );
				subRect.Top =	(int) ( ( (float) imgSize.y - viewSize.y) * mViewPosition.y );
				subRect.Right = (int) viewSize.x + subRect.Left;
				subRect.Bottom =(int) viewSize.y + subRect.Top;
				
				mBackgroundSprite->GetSFSprite()->SetSubRect( subRect );
				mBackgroundSprite->GetSFSprite()->SetCenter( viewSize * 0.5f );
				mBackgroundSprite->GetSFSprite()->SetPosition( mGlobalPosition + mGlobalSize * 0.5f );
				mBackgroundSprite->GetSFSprite()->Resize( mGlobalSize );
				mBackgroundSprite->GetSFSprite()->SetRotation( mBackgroundSprite->GetSFSprite()->GetRotation() + mRotationSpeed * secondsPassed );
				mBackgroundSprite->SetClipRect( mClippingRect );
			}
					
			if ( mForegroundSprite->GetSFSprite()->GetImage() ) 
			{
				sf::Vector2i imgSize(mForegroundSprite->GetSFSprite()->GetImage()->GetWidth(),mForegroundSprite->GetSFSprite()->GetImage()->GetHeight());
				sf::Vector2f viewSize( (float) imgSize.x * mViewSize, (float) imgSize.y * mViewSize );

				sf::IntRect subRect;
				subRect.Left =	(int) ( ( (float) imgSize.x - viewSize.x) * mViewPosition.x );
				subRect.Top =	(int) ( ( (float) imgSize.y - viewSize.y) * mViewPosition.y );
				subRect.Right = (int) viewSize.x + subRect.Left;
				subRect.Bottom =(int) viewSize.y + subRect.Top;
				
				mForegroundSprite->GetSFSprite()->SetSubRect( subRect );
				mForegroundSprite->GetSFSprite()->SetCenter( viewSize * 0.5f );
				mForegroundSprite->GetSFSprite()->SetPosition( mGlobalPosition + mGlobalSize * 0.5f );
				mForegroundSprite->GetSFSprite()->Resize( mGlobalSize );
				mForegroundSprite->GetSFSprite()->SetRotation( mBackgroundSprite->GetSFSprite()->GetRotation() );
				mForegroundSprite->SetClipRect( mClippingRect );
			}
		}
	}
}

