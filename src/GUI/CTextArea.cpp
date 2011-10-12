#include "CTextArea.h"
#include "CRoot.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../Rendering/CHudStaticText.h"
#include "../Rendering/CMultiColorString.h"
#include "../ResourceManager/CResourceManager.h"
#include "../CGameOptions.h"
#include "../ResourceManager/CImage.h"
#include "../Utils/Maths.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/String.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Glyph.hpp>

using namespace GUI;

void CTextArea::SetBackgroundImage( const std::string& filename )
{
	if ( !mBackgroundSprite )
		mBackgroundSprite = gDrawableManager.CreateHudSprite( mZIndex );

	if ( sf::Image* img = gResourceManager.GetImage(filename) )
		mBackgroundSprite->GetSFSprite()->SetImage( *img );
}

void CTextArea::SetFont(const GUI::FontSetting & fs)
{
    SetFont(fs.name, fs.size, fs.unit);
}

void CTextArea::SetFont(const std::string &name, float size, guiUnit u)
{
	mFontSize[u] = size * ( u == UNIT_PERCENT ? .01f : 1.0f );
	mTextSprite->SetFont( name, (unsigned) GetTotalFontSize() );
	mUpdateText = true;
}

void CTextArea::SetColor(const sf::Color color)
{
    mTextSprite->GetSFString()->SetColor( color );
}

void CTextArea::SetText(const std::wstring &text)
{
	mText = text;
	mUpdateText = true;
}

void CTextArea::SetAutoHeight( bool autoHeight )
{
	mAutoHeight = true;
	mUpdateText = true;
}

void CTextArea::SetCenter( bool center )
{
	mCenter = center; 
}

void CTextArea::UpdateText()
{
	std::wstring txt	( mText );
	float textAreaWidth	( mInternalRect.GetWidth() );
	float curLineWidth	( 0.0f );
	float curWordWidth	( 0.0f );
	float curGlyphScale	( mTextSprite->GetSFString()->GetSize() / mTextSprite->GetSFString()->GetFont().GetCharacterSize() );
	int curLineBegin	( 0 );
	int curWordBegin	(-1 );
	int lineCount		( 1 );
	
	for ( unsigned i = 0; i < txt.length(); i++ )
	{
		if ( txt[i] == L' ' )			{ curWordBegin = -1; }
		else if ( txt[i] == L'\n' )		{ curWordBegin = -1; curLineBegin=i+1; curLineWidth = 0.0f; ++lineCount; }
		else if ( curWordBegin < 0 )	{ curWordBegin = i; curWordWidth = 0.0f; }
		
		curLineWidth += curGlyphScale * mTextSprite->GetSFString()->GetFont().GetGlyph( txt[i] ).Advance;
		curWordWidth += curGlyphScale * mTextSprite->GetSFString()->GetFont().GetGlyph( txt[i] ).Advance;
		
		if (curLineWidth > textAreaWidth) 
		{
			if (curWordBegin > curLineBegin) 
			{
				txt.insert(curWordBegin, L"\n");
				curLineWidth = curWordWidth;
				curLineBegin = curWordBegin;
				++lineCount;
				++i;
			}
			else if (curWordBegin == curLineBegin) 
			{
				txt.insert(i,L"\n");
				curLineBegin = i;
				curLineWidth = 0;
				++lineCount;
				++i;
			}
		}
	}

	mTextSprite->GetSFString()->SetText( txt );

	if ( mAutoHeight )
	{
		float textHeight =		GetSFString()->GetSize() * lineCount + 
								curGlyphScale * GetSFString()->GetFont().GetGlyph( 'j' ).Rectangle.Bottom;
		float pixelPadding =	mPadding[UNIT_PIXEL].Top + mPadding[UNIT_PIXEL].Bottom;
		float percentPadding =	mPadding[UNIT_PERCENT].Top + mPadding[UNIT_PERCENT].Bottom;

        mSize[UNIT_PERCENT].y = 0.f;
		mSize[UNIT_PIXEL].y =	( textHeight + pixelPadding ) / ( 1.0f - percentPadding );
	}
}

sf::String *CTextArea::GetSFString()
{
    return mTextSprite->GetSFString();
}

const std::wstring CTextArea::GetNextColorString(sf::Color col)
{
    return CMultiColorString::NextColor(col);
}

CTextArea::CTextArea(const std::string &name, GUI::CGUIObject *parent, unsigned zindex) :
	CGUIObject( name, GUI_TEXT_AREA, parent, zindex),
    mBackgroundSprite(NULL),
	mAutoHeight( false ),
    mUpdateText( true ),
    mCenter( false )
{
	mFontSize[UNIT_PERCENT] = mFontSize[UNIT_PIXEL] = 0.0f;
	mTextSprite = gDrawableManager.CreateHudStaticText( mZIndex - 1 );
	mTextSprite->GetSFString()->SetColor( sf::Color::White );
	mZIndexCount = 2;
}

CTextArea::~CTextArea()
{
	if (mBackgroundSprite)	gDrawableManager.DestroyDrawable( mBackgroundSprite );
	if (mTextSprite)		gDrawableManager.DestroyDrawable( mTextSprite );
}

bool CTextArea::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	if ( e == MOUSE_PRESSED_LEFT )
		gGUI.SetActiveObject(NULL);

	return true;
}

void CTextArea::UpdateSprites( float secondsPassed )
{
	if (mBackgroundSprite) 
	{
		mBackgroundSprite->SetVisible( mIsVisible );

		if ( mIsVisible )
		{
			mBackgroundSprite->GetSFSprite()->SetPosition( mGlobalPosition );
			mBackgroundSprite->GetSFSprite()->Resize( mGlobalSize );
			mBackgroundSprite->SetClipRect( mClippingRect );
		}
	}

	if (mTextSprite)
	{
		mTextSprite->SetVisible( mIsVisible );

		if ( mIsVisible )
		{
			float newSize = floor( GetTotalFontSize() );

			mUpdateText = Maths::Abs( newSize - mTextSprite->GetSFString()->GetSize() ) < 0.01f;
			mTextSprite->SetClipRect( mClippingRect );
			mTextSprite->SetFontSize( (unsigned) newSize );

            if (mCenter)
            {
			    const sf::FloatRect& txt = mTextSprite->GetSFString()->GetRect();
			    mTextSprite->SetPosition(
					( mInternalRect.Left + mInternalRect.Right - txt.GetWidth() ) * 0.5f,
					( mInternalRect.Top + mInternalRect.Bottom - txt.GetHeight() ) * 0.5f
				);
            }
            else
				mTextSprite->SetPosition( mInternalRect.Left, mInternalRect.Top );
		}

		if ( mUpdateText ) 
		{
			UpdateText();
			mUpdateText = false;
		}
	}
}

