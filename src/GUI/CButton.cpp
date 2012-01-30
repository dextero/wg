#include "CButton.h"
#include "CRoot.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../Rendering/CHudStaticText.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../CGameOptions.h"
#include "../Audio/CAudioManager.h"
#include "../Input/CBindManager.h"
#include "../Input/CPlayerController.h"
#include "../Utils/KeyStrings.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/CPlayer.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/String.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Glyph.hpp>

using namespace GUI;

void CButton::SetImage( const std::string& normal, const std::string& mouseOver, const std::string& mouseClick )
{
    sf::Image* img;

	if ( !mNormalSprite )
		mNormalSprite = gDrawableManager.CreateHudSprite( mZIndex );

	if (( img = gResourceManager.GetImage(normal) ))
		mNormalSprite->GetSFSprite()->SetImage( *img );

	if ( mouseOver != "" )
	{
		if ( !mMouseOverSprite )
			mMouseOverSprite = gDrawableManager.CreateHudSprite( mZIndex );

		if (( img = gResourceManager.GetImage(mouseOver) ))
			mMouseOverSprite->GetSFSprite()->SetImage( *img );
	}

    if ( mouseClick != "" )
    {
        if ( !mMouseClickSprite )
            mMouseClickSprite = gDrawableManager.CreateHudSprite( mZIndex );

        if (( img = gResourceManager.GetImage(mouseClick) ))
            mMouseClickSprite->GetSFSprite()->SetImage( *img );
    }
}

void CButton::SetFont(const GUI::FontSetting & fs)
{
    SetFont(fs.name, fs.size, fs.unit);
}

void CButton::SetFont( const std::string& name, float size, guiUnit u )
{
	if ( !mTextSprite )
		mTextSprite = gDrawableManager.CreateHudStaticText( mZIndex - 1 );

	mFontSize[u] = size * ( u == UNIT_PERCENT ? .01f : 1.0f );
	mTextSprite->SetFont( name, unsigned( mFontSize[UNIT_PERCENT] * mGlobalSize.y + mFontSize[UNIT_PIXEL] ) );

}

void CButton::SetColor( const sf::Color color )
{
	if ( !mTextSprite )
		mTextSprite = gDrawableManager.CreateHudStaticText( mZIndex - 1 );

	mTextSprite->GetSFString()->SetColor( color );
}

void CButton::SetText( const std::wstring& text )
{
	if ( !mTextSprite )
		mTextSprite = gDrawableManager.CreateHudStaticText( mZIndex - 1 );

	mTextSprite->GetSFString()->SetText( text );
}

void CButton::SetText(const std::wstring& text, int playerNumber, int buttonNo)
{
    System::Input::CBindManager * bindManager = gBindManagerByPlayer(playerNumber);
    if (bindManager->GetPointNClickMove()) {
        return SetText(text);
    }
    std::string keyKey = "Abi-" + StringUtils::ToString(buttonNo);
    std::string keyXKey = "AbiX-" + StringUtils::ToString(buttonNo);
    std::string keyLabel;
    
    const std::map<std::string, int> & keys = bindManager->GetKeyBindings();
    if (keys.count(keyKey) > 0) {
        keyLabel = KeyStrings::KeyToString(keys.find(keyKey)->second);
    } else if (keys.count(keyXKey) > 0) {
        keyLabel = KeyStrings::KeyToString(keys.find(keyXKey)->second);
    }
    SetText(text + L" (press " + StringUtils::ConvertToWString(keyLabel) + L")");
}

std::wstring CButton::GetText()
{
	if ( mTextSprite )	return mTextSprite->GetSFString()->GetText();
	else				return std::wstring(L"");
}

//@Deprecated:
void CButton::SetText(const SLocalizedText& text, std::string font)
{
    SetText(text.text);
    SetFont(font, 100.0f, GUI::UNIT_PIXEL);
    SetPadding(text.padding);
}

void CButton::SetCenter( bool center )
{
	mCenter = center;
}

CButton::CButton(const std::string &name, CGUIObject *parent, unsigned zindex) :
	CGUIObject(name, GUI_BUTTON, parent, zindex),
    mMouseOver(false),
    mMousePressed(false),
	mCenter(false),
	mNormalSprite(NULL),
    mMouseOverSprite(NULL),
    mMouseClickSprite(NULL),
    mTextSprite(NULL)
{
	mFontSize[UNIT_PERCENT] = mFontSize[UNIT_PIXEL] = 0.0f;
	mZIndexCount = 2;
}

CButton::~CButton()
{
	if ( mNormalSprite )	gDrawableManager.DestroyDrawable( mNormalSprite );
	if ( mMouseOverSprite ) gDrawableManager.DestroyDrawable( mMouseOverSprite );
    if ( mMouseClickSprite) gDrawableManager.DestroyDrawable ( mMouseClickSprite );
	if ( mTextSprite )		gDrawableManager.DestroyDrawable( mTextSprite );
}

static void ForceReleaseMouseButton()
{
    for (int i = 0 ; i <= 1; i++) {
        System::Input::CBindManager::GetActualBindManager(i)->ForceMouseLeftReleased();
        gBindManagerByPlayer(i)->SetKeyState("PointNClick", false);
        if (CPlayer *p = gPlayerManager.GetPlayerByNumber(i)) {
            if (CPlayerController *pc = p->GetController()) {
                pc->SetWalkTarget(false, p->GetPosition(), true);
            }
        }
    }
}

bool CButton::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	switch ( e )
	{
	case MOUSE_PRESSED_LEFT:
        mMousePressed = true;
        ForceReleaseMouseButton();
        gAudioManager.GetSoundPlayer().Play(gResourceManager.GetSound("data/sounds/GUI_klik.ogg"));
		break;
    case MOUSE_RELEASED_LEFT:
        if (mMousePressed)
        {
            ForceReleaseMouseButton();
            gGUI.SetActiveObject( NULL );
		    if ( !mClickCallback.empty() )		mClickCallback();
		    if ( !mClickParamCallback.empty() ) mClickParamCallback( mClickCallbackParams );
            if ( !mClickIntCallback.empty() )   mClickIntCallback(mClickIntCallbackParam);
        }
        mMousePressed = false;
        break;
	case MOUSE_OVER:
		mMouseOver = true;
		break;
	case MOUSE_OUT:
		mMouseOver = false;
        mMousePressed = false;
		break;
    default:
        break;
	}

	return false;
}

void CButton::UpdateSprites( float secondsPassed )
{
	/* Optymalizacja wyswietlania */
	bool visible = mIsVisible && mClippingRect.Intersects(
		sf::Rect<float>(mGlobalPosition.x,mGlobalPosition.y,mGlobalPosition.x+mGlobalSize.x,mGlobalPosition.y+mGlobalSize.y) );

	if ( mNormalSprite && mNormalSprite->GetSFSprite()->GetImage() )
	{
		mNormalSprite->SetVisible( visible && ( !mMouseOver || !mMouseOverSprite ) && ( !mMousePressed || !mMouseClickSprite ) );
		if ( visible && !mMouseOver )
		{
			mNormalSprite->GetSFSprite()->SetPosition( mGlobalPosition );
			mNormalSprite->GetSFSprite()->Resize( mGlobalSize );
			mNormalSprite->SetClipRect( mClippingRect );
		}
	}

	if ( mMouseOverSprite && mMouseOverSprite->GetSFSprite()->GetImage() )
	{
		mMouseOverSprite->SetVisible( visible && mMouseOver && (!mMousePressed || !mMouseClickSprite ) );
		if ( visible && mMouseOver )
		{
			mMouseOverSprite->GetSFSprite()->SetPosition( mGlobalPosition );
			mMouseOverSprite->GetSFSprite()->Resize( mGlobalSize );
			mMouseOverSprite->SetClipRect( mClippingRect );
		}
	}

    if ( mMouseClickSprite && mMouseClickSprite->GetSFSprite()->GetImage() )
    {
        mMouseClickSprite->SetVisible( visible && mMousePressed);

        if ( visible && mMousePressed )
        {
            mMouseClickSprite->GetSFSprite()->SetPosition( mGlobalPosition );
            mMouseClickSprite->GetSFSprite()->Resize( mGlobalSize );
            mMouseClickSprite->SetClipRect( mClippingRect );
        }
    }

	if ( mTextSprite )
	{
		mTextSprite->SetVisible( visible );
		if ( visible )
		{
			mTextSprite->SetFontSize( unsigned( mFontSize[UNIT_PERCENT] * mGlobalSize.y + mFontSize[UNIT_PIXEL] ) );
			mTextSprite->SetClipRect( mClippingRect );

			const sf::FloatRect& txt = mTextSprite->GetSFString()->GetRect();
			sf::Vector2f pos( mInternalRect.Left, (mInternalRect.Top + mInternalRect.Bottom - txt.GetHeight()) * 0.5f );
			if ( mCenter )
				pos.x = ( mInternalRect.Left + mInternalRect.Right - txt.GetWidth() ) * 0.5f;

			mTextSprite->SetPosition( pos.x, pos.y );
		}
	}
}
