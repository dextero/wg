#include "CWindow.h"
#include "CRoot.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

using namespace GUI;

void CWindow::SetBackgroundImage( const std::string& filename )
{
	if (!mBackgroundSprite)
		mBackgroundSprite = gDrawableManager.CreateHudSprite( mZIndex );

	if (sf::Image* img = gResourceManager.GetImage(filename))
		mBackgroundSprite->GetSFSprite()->SetImage( *img );
}

CWindow::CWindow(const std::string &name, GUI::CGUIObject *parent, unsigned zindex)
: CGUIObject(name, GUI_WINDOW, parent, zindex), mBackgroundSprite(NULL)
{
	mZIndexCount = 1;
}

CWindow::~CWindow()
{
	if (mBackgroundSprite)
		gDrawableManager.DestroyDrawable( mBackgroundSprite );
}

bool CWindow::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	if (e == MOUSE_PRESSED_LEFT)
		gGUI.SetActiveObject(NULL);

	return false;
}

void CWindow::UpdateSprites( float secondsPassed )
{
	if (mBackgroundSprite) 
	{
		mBackgroundSprite->SetVisible(mIsVisible);

		if ( mIsVisible )
		{
			mBackgroundSprite->GetSFSprite()->SetPosition( mGlobalPosition  );
			mBackgroundSprite->GetSFSprite()->Resize( mGlobalSize ); 
			mBackgroundSprite->SetClipRect( mClippingRect );
		}
	}
}

