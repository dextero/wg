#include "CDropList.h"
#include "CRoot.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../Utils/StringUtils.h"
#include <iostream>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

using namespace GUI;

void CDropList::SetImage( const std::string& normal, const std::string& mouseOver )
{
	mSelectedOption->SetImage( normal, mouseOver );
}

void CDropList::SetFont(const std::string &name, float size, guiUnit u)
{
	mSelectedOption->SetFont( name, size, u );
}

void CDropList::SetColor(const sf::Color color)
{
	mSelectedOption->SetColor( color );
}

void CDropList::SetCenter(bool center)
{
	mSelectedOption->SetCenter( center );
}

void CDropList::SetOptionImage(const std::string &normal, const std::string &mouseOver)
{
	mOptionData.normalImage = normal;
	mOptionData.mouseOverImage = mouseOver;
}

void CDropList::SetOptionFont(const std::string& name, float size, guiUnit u)
{
	mOptionData.font = name;
	mOptionData.fontSize[u] = size;
}

void CDropList::SetOptionColor(const sf::Color color)
{
	mOptionData.color = color;
}

void CDropList::SetOptionCenter(bool center)
{
	mOptionData.center = center;
}

void CDropList::AddOption(const std::wstring &option)
{
	/* jak dodamy opcje jak lista jest rozwinieta to sie wszystko $%^#$#^ */
	if ( !mShowList )
	{
		CButton * button = CreateButton( mName + StringUtils::ToString( mOptions.size() ), true, Z_GUI2 );
		CButton * lastButton = mOptions.size() > 0 ? mOptions[ mOptions.size() - 1 ] : mSelectedOption;
		
		button->SetPosition( 0.0f, (lastButton->GetPosition().y + lastButton->GetSize().y) * 100.0f, 100.0f, 100.0f );
		button->SetImage( mOptionData.normalImage, mOptionData.mouseOverImage );
		button->SetFont( mOptionData.font, mOptionData.fontSize[UNIT_PERCENT], UNIT_PERCENT );
		button->SetFont( mOptionData.font, mOptionData.fontSize[UNIT_PIXEL], UNIT_PIXEL );
		button->SetColor( mOptionData.color );
		button->SetCenter( mOptionData.center );
		button->SetText( option );
		button->GetClickParamCallback()->bind( this, &CDropList::SetSelectedOption );
		button->SetClickCallbackParams( option );

		mOptions.push_back( button );
	}
}

void CDropList::SetSelectedOption(const std::wstring &option)
{
	mSelectedOption->SetText( option );
}

std::wstring CDropList::GetSelectedOption()
{
	return mSelectedOption->GetText();		
}

size_t CDropList::GetSelectedIndex()
{
    for ( size_t i = 0; i < mOptions.size() ; i++ )
    {
        if ( mSelectedOption->GetText() == mOptions[i]->GetText() )
           return i;
    }
    return (size_t)(-1);
}

void CDropList::ShowList()
{
	if ( !mShowList )
	{
		/* rozszerzamy obiekt do wszystkich opcji */
		float singleOptionHeight = 100.0f / (float) (mOptions.size() + 1);
		mSelectedOption->SetPosition( 0.0f, 0.0f, 100.0f, singleOptionHeight );
		mSize[UNIT_PERCENT].y *= (mOptions.size() + 1);
		mSize[UNIT_PIXEL].y *= (mOptions.size() + 1);
		
		for ( unsigned i = 0; i < mOptions.size(); i++ )
			mOptions[i]->SetPosition( 0.0f, singleOptionHeight * float(i+1), 100.0f, singleOptionHeight );

		mShowList = true;
		gGUI.SetActiveObject( (CGUIObject*) this );
	}
}

void CDropList::HideList()
{
	if ( mShowList && mSelectedOption )
	{
		mSize[UNIT_PERCENT].y /= (float) (mOptions.size() + 1);
		mSize[UNIT_PIXEL].y /= (float) (mOptions.size() + 1);
		mSelectedOption->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
		mShowList = false;
	}
}

CDropList::CDropList(const std::string &name, GUI::CGUIObject *parent, unsigned int zindex)
:	CGUIObject( name, GUI_DROP_LIST, parent, zindex ), mShowList( false )
{
	mZIndexCount = 1;
	mSelectedOption = CreateButton( name + "_selected" );
	mSelectedOption->SetPosition( 0.0f, 0.0f, 100.0f, 100.0f );
	mSelectedOption->GetClickCallback()->bind( this, &CDropList::ShowList );
}

CDropList::~CDropList()
{
}

bool CDropList::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	switch ( e )
	{
	case MOUSE_UNCLICK:			
		HideList(); 
		break;
    default:
        break;
	}

	return false;
}

void CDropList::UpdateSprites( float secondsPassed )
{
	mSelectedOption->SetVisible( mIsVisible );

	for ( unsigned i = 0; i < mOptions.size(); i++ )
		mOptions[i]->SetVisible( mIsVisible && mShowList );

	PassLeftPaddingToChilds( UNIT_PERCENT );
	PassLeftPaddingToChilds( UNIT_PIXEL );
}

void CDropList::PassLeftPaddingToChilds(GUI::guiUnit u)
{
	float multiplier = ( u == UNIT_PERCENT ? 100.0f : 1.0f );
	if ( mPadding[u].Left != 0.0f )
	{
		mSelectedOption->SetPadding( mPadding[u].Left * multiplier, 0.0f, 0.0f, 0.0f, u );
		for ( unsigned i = 0; i < mOptions.size(); i++ )
			mOptions[i]->SetPadding( mPadding[u].Left * multiplier, 0.0f, 0.0f, 0.0f, u );
		mPadding[u].Left = 0.0f;
	}
}

