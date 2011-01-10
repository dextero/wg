#include "CProgressBar.h"
#include "CRoot.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../Rendering/CCamera.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <iostream>

using namespace GUI;

void CProgressBar::SetBorderImage( const std::string& filename )
{
	if ( !mBorderSprite )
		mBorderSprite = gDrawableManager.CreateHudSprite( mZIndex - 1 );

	if ( sf::Image* img = gResourceManager.GetImage(filename) )
		mBorderSprite->GetSFSprite()->SetImage( *img );
}

void CProgressBar::SetProgressImage( const std::string& filename )
{
	if (!mProgressSprite)
		mProgressSprite = gDrawableManager.CreateHudSprite( mZIndex );

	if ( sf::Image* img = gResourceManager.GetImage(filename) )
		mProgressSprite->GetSFSprite()->SetImage( *img );
}

CProgressBar::CProgressBar( const std::string &name, GUI::CGUIObject *parent, unsigned zindex ) :
	CGUIObject(name, GUI_PROGRESS_BAR, parent, zindex),
	mBorderSprite(NULL),
    mProgressSprite(NULL),
	mMinCPNormalAngle(0.0f),
	mMaxCPNormalAngle(0.0f),
	mMinCPPoint(0.0f,0.0f),
	mMaxCPPoint(0.0f,0.0f),
    mProgress(1.0f),
	mAlpha(1.0f)
{
	mZIndexCount = 2;
}

CProgressBar::~CProgressBar()
{
	if (mBorderSprite)		gDrawableManager.DestroyDrawable( mBorderSprite );
	if (mProgressSprite)	gDrawableManager.DestroyDrawable( mProgressSprite );
}

bool CProgressBar::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	if ( e == MOUSE_PRESSED_LEFT )
		gGUI.SetActiveObject(NULL);

	return false;
}

void CProgressBar::UpdateSprites( float secondsPassed )
{
	sf::Vector2i imgSize;
	sf::Vector2f progressSize;
	sf::Vector2f progressPosition;

	if (mBorderSprite) 
	{
		mBorderSprite->SetVisible( mIsVisible );

		if ( mIsVisible )
		{
			mBorderSprite->GetSFSprite()->SetPosition( mGlobalPosition );
			mBorderSprite->GetSFSprite()->Resize( mGlobalSize );
			mBorderSprite->SetClipRect( mClippingRect );
		}
	}

	if (mProgressSprite) 
	{
		mProgressSprite->SetVisible( mIsVisible );

		if ( mIsVisible )
		{
			imgSize.x = mProgressSprite->GetSFSprite()->GetImage()->GetWidth();
			imgSize.y = mProgressSprite->GetSFSprite()->GetImage()->GetHeight();

			progressPosition.x = mInternalRect.Left;
			progressPosition.y = mInternalRect.Top;

			progressSize.x = mInternalRect.GetWidth();
			progressSize.y = mInternalRect.GetHeight();

			float normalAngle =	Maths::Lerp( mMinCPNormalAngle, mMaxCPNormalAngle, mProgress ) * 3.14f / 180.0f;
			sf::Vector2f normal = Maths::VectorOf( sinf( normalAngle ), -cosf( normalAngle ) );
			sf::Vector2f point = progressPosition +
								 Maths::Lerp( sf::Vector2f( mMinCPPoint.x * mGlobalSize.x, mMinCPPoint.y * mGlobalSize.y ),
											  sf::Vector2f( mMaxCPPoint.x * mGlobalSize.x, mMaxCPPoint.y * mGlobalSize.y ), mProgress );
			
			mProgressSprite->GetSFSprite()->SetPosition( progressPosition );
			mProgressSprite->GetSFSprite()->Resize( progressSize );
			mProgressSprite->SetClipPlane( normal.x, normal.y, 0.0f, -Maths::Dot(normal, point) );
			mProgressSprite->SetClipRect( mClippingRect );
			sf::Color color = mProgressSprite->GetSFSprite()->GetColor();
            mProgressSprite->GetSFSprite()->SetColor(sf::Color(color.r,color.g,color.b,(sf::Uint8)(mAlpha * 255)));
		}
	}
}

