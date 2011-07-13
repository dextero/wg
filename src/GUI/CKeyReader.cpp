#include "CKeyReader.h"
#include "CRoot.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../Rendering/CHudStaticText.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../CGameOptions.h"
#include "../Utils/StringUtils.h"
#include "../Utils/KeyStrings.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/String.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Glyph.hpp>

using namespace GUI;

void CKeyReader::SetImage( const std::string& normal, const std::string& mouseOver, const std::string& active )
{
	if ( normal != "" )		mNormalImg = gResourceManager.GetImage( normal );
	if ( mouseOver != "" )	mMouseOverImg = gResourceManager.GetImage( mouseOver );
	if ( active != "" )		mActiveImg = gResourceManager.GetImage( active );
}

void CKeyReader::SetFont( const std::string& name, float size, guiUnit u )
{
	sf::Font* font = gResourceManager.GetFont( name );
	mTextSprite->GetSFString()->SetFont( *font );
	mFontSize[u] = size * ( u == UNIT_PERCENT ? .01f : 1.0f );
}

void CKeyReader::SetColor( const sf::Color color )
{
	mTextSprite->GetSFString()->SetColor( color );
}

void CKeyReader::SetKey( int key )
{
	std::string txt = KeyStrings::KeyToString( key );
	if ( txt != "" )
	{
		mTextSprite->GetSFString()->SetText( StringUtils::ConvertToWString( txt ) );
		mKey = key;
	}
	else
	{
		mTextSprite->GetSFString()->SetText( L"???" );
		mKey = -1;
	}
}

void CKeyReader::SetCenter( bool center )
{
	mCenter = center;
}

CKeyReader::CKeyReader( const std::string& name, CGUIObject* parent, unsigned zindex )
:	CGUIObject( name, GUI_KEY_READER, parent, zindex ),
	mMouseOver( false ),
    mActive( false ),
	mReadyToActive( false ),
	mCenter( false ),
	mNormalImg( NULL ),
    mMouseOverImg( NULL ),
    mActiveImg( NULL ),
	mKey( -1 ),
    mClickCallback( NULL ),
    mClickParamCallback( NULL )
{
	mBackgroundSprite = gDrawableManager.CreateHudSprite( mZIndex );
	mTextSprite = gDrawableManager.CreateHudStaticText( mZIndex - 1 );
	mTextSprite->GetSFString()->SetText( L"???" );
	mFontSize[UNIT_PERCENT] = mFontSize[UNIT_PIXEL] = 0.0f;
	mZIndexCount = 2;
}

CKeyReader::~CKeyReader()
{
	gDrawableManager.DestroyDrawable ( mBackgroundSprite );
	gDrawableManager.DestroyDrawable( mTextSprite );
}

void CKeyReader::SetKeyFromMouseButton(sf::Mouse::Button btn)
{
	SetKey( sf::Key::Count + btn );
	gGUI.SetActiveObject( NULL );
	mActive = false;
    
    // popnij zduplikowane binding-options
    if ( !mClickCallback.empty() )		mClickCallback();
}

bool CKeyReader::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	switch ( e )
	{
	case MOUSE_PRESSED_LEFT:
		if ( mActive )
            SetKeyFromMouseButton(sf::Mouse::Left);
		else
        {
            mReadyToActive = true;

            // pushnij binding-options jeszcze raz na stos
		    if ( !mClickParamCallback.empty() ) mClickParamCallback( mClickCallbackParams );
        }
		break;
	case MOUSE_PRESSED_RIGHT:
        if (mActive) SetKeyFromMouseButton(sf::Mouse::Right);
		break;
    case MOUSE_PRESSED_MIDDLE:
        if (mActive) SetKeyFromMouseButton(sf::Mouse::Middle);
        break;
    case MOUSE_PRESSED_X1:
        if (mActive) SetKeyFromMouseButton(sf::Mouse::XButton1);
        break;
    case MOUSE_PRESSED_X2:
        if (mActive) SetKeyFromMouseButton(sf::Mouse::XButton2);
        break;
	case MOUSE_RELEASED_LEFT:
		if ( mReadyToActive )
		{
			SetKey( -1 );
			gGUI.SetActiveObject( this );
			mActive = true;
			mReadyToActive = false;
		}
		break;
	case MOUSE_OVER:	mMouseOver = true; break;
	case MOUSE_OUT:		mMouseOver = false; break;
	default:            break;
	}

	return false;
}

void CKeyReader::OnKeyEvent( const sf::Event::KeyEvent& e, bool pressed )
{
	if ( mActive && pressed )
	{
		SetKey( e.Code );
		gGUI.SetActiveObject( NULL );
		mActive = false;

        // popnij zduplikowane binding-options
        if (e.Code != sf::Key::Escape)
		    if ( !mClickCallback.empty() )		mClickCallback();
	}
}

void CKeyReader::UpdateSprites( float secondsPassed )
{
	sf::Image* img;

	if ( mActive && mActiveImg != NULL )			img = mActiveImg;
	else if ( mMouseOver && mMouseOverImg != NULL )	img = mMouseOverImg;
	else											img = mNormalImg;

	mBackgroundSprite->SetVisible( mIsVisible );
	if ( mIsVisible )
	{
		if ( (img != NULL) && (mBackgroundSprite->GetSFSprite()->GetImage() != img) )
			mBackgroundSprite->GetSFSprite()->SetImage( *img );
		mBackgroundSprite->GetSFSprite()->SetPosition( mGlobalPosition );
		mBackgroundSprite->GetSFSprite()->Resize( mGlobalSize );
		mBackgroundSprite->SetClipRect( mClippingRect );
	}

	if ( mTextSprite )
	{
		mTextSprite->SetVisible( mIsVisible );
		if ( mIsVisible )
		{
			mTextSprite->GetSFString()->SetSize( mFontSize[UNIT_PERCENT] * mGlobalSize.y + mFontSize[UNIT_PIXEL] );
			mTextSprite->SetClipRect( mClippingRect );

			const sf::FloatRect& txt = mTextSprite->GetSFString()->GetRect();
			sf::Vector2f pos( mInternalRect.Left, (mInternalRect.Top + mInternalRect.Bottom - txt.GetHeight()) * 0.5f );
			if ( mCenter )
				pos.x = ( mInternalRect.Left + mInternalRect.Right - txt.GetWidth() ) * 0.5f;

			mTextSprite->GetSFString()->SetPosition( pos );
		}
	}
}

