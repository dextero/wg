#include "CHudSprite.h"
#include "CCamera.h"
#include "CClippedSprite.h"
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
	mSprite(NULL), 
	mClipRectEnabled( false )
{
    memset(mClipRect, 0, 4 * sizeof(int));
    mSprite = new CClippedSprite;
}

CHudSprite::~CHudSprite()
{
    delete mSprite;
}

void CHudSprite::Draw( sf::RenderWindow* renderWindow )
{
	float zoom = gCamera.GetZoom();
	gCamera.SetZoom(1.0f); 

	sf::Vector2f cameraPosition = gGame.GetRenderWindow()->GetDefaultView().GetCenter();
	gGame.GetRenderWindow()->GetDefaultView().SetCenter( gGame.GetRenderWindow()->GetDefaultView().GetHalfSize() );   

	if ( mClipRectEnabled )
	{
		glEnable( GL_SCISSOR_TEST );
		glScissor( mClipRect[0], mClipRect[1], mClipRect[2], mClipRect[3] );
	}
	
	renderWindow->Draw( *mSprite );
		
	glDisable( GL_SCISSOR_TEST );
	gCamera.SetZoom( zoom );
	gGame.GetRenderWindow()->GetDefaultView().SetCenter( cameraPosition );
}

sf::Sprite* CHudSprite::GetSFSprite() 
{ 
	return mSprite; 
} 

CClippedSprite* CHudSprite::GetClippedSprite() 
{ 
	return mSprite; 
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

