#include "CHudSprite.h"
#include "CCamera.h"
#include "../CGame.h"
#include "../CGameOptions.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#ifdef PLATFORM_MACOSX
#include <gl.h>
#else
#include <GL/gl.h>
#endif

CHudSprite::CHudSprite() :
    IDrawable(), 
	mSFSprite(NULL), 
	mClipPlaneEnabled(false),
	mClipRectEnabled( false )
{
    memset(mClipPlane, 0, 4 * sizeof(double));
    memset(mClipRect, 0, 4 * sizeof(int));
    mSFSprite = new sf::Sprite();
}

CHudSprite::~CHudSprite()
{
    delete mSFSprite;
}

void CHudSprite::Draw( sf::RenderWindow* renderWindow )
{
	float zoom = gCamera.GetZoom();
	gCamera.SetZoom(1.0f); 

	sf::Vector2f cameraPosition = gGame.GetRenderWindow()->GetDefaultView().GetCenter();
	gGame.GetRenderWindow()->GetDefaultView().SetCenter( gGame.GetRenderWindow()->GetDefaultView().GetHalfSize() );   
    
	if ( mClipPlaneEnabled )
	{
		glEnable( GL_CLIP_PLANE0 );
		glClipPlane( GL_CLIP_PLANE0, mClipPlane );
	}

	if ( mClipRectEnabled )
	{
		glEnable( GL_SCISSOR_TEST );
		glScissor( mClipRect[0], mClipRect[1], mClipRect[2], mClipRect[3] );
	}
	
	renderWindow->Draw( *mSFSprite );
		
	glDisable( GL_CLIP_PLANE0 );
	glDisable( GL_SCISSOR_TEST );
	gCamera.SetZoom( zoom );
	gGame.GetRenderWindow()->GetDefaultView().SetCenter( cameraPosition );
}

sf::Sprite* CHudSprite::GetSFSprite()
{
    return mSFSprite;
}

void CHudSprite::SetClipPlane( double a, double b, double c, double d )
{
	mClipPlaneEnabled = true;
	mClipPlane[0] = a;
	mClipPlane[1] = b;
	mClipPlane[2] = c;
	mClipPlane[3] = d;
}

void CHudSprite::SetClipRect(int left, int top, int right, int bottom)
{
	mClipRectEnabled = true;
	mClipRect[0] = left;
	mClipRect[1] = gGameOptions.GetHeight() - bottom;
	mClipRect[2] = right - left;
	mClipRect[3] = bottom - top;
}

void CHudSprite::SetClipRect(const sf::FloatRect &rect)
{
	SetClipRect( (int) rect.Left, (int) rect.Top, (int) rect.Right, (int) rect.Bottom );
}

