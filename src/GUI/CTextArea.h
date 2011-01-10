/* GUI - pole tekstowe (wszystkie opisy, dialogi etc. ) */

#ifndef _TEXT_AREA_H_
#define _TEXT_AREA_H_

#include "CGUIObject.h"
#include <string>
#include <SFML/Graphics/Color.hpp>

class CHudSprite;
class CHudStaticText;

namespace sf
{
    class String;
};

namespace GUI
{
	class CTextArea : public CGUIObject
	{
	public:
		void SetBackgroundImage( const std::string& filename );
		void SetFont( const std::string& name, float size, guiUnit u = UNIT_PIXEL );
		void SetColor(const sf::Color color);
		void SetText( const std::wstring& text );
		void SetAutoHeight( bool autoHeight );
		void SetCenter( bool center );
		void UpdateText();

		inline std::wstring GetText()			{ return mText; }
		inline float		GetTotalFontSize()	{ return mFontSize[UNIT_PERCENT] * mInternalRect.GetHeight() + mFontSize[UNIT_PIXEL]; }
        sf::String *		GetSFString();
	private:
		CTextArea( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CTextArea();

		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		CHudSprite*		mBackgroundSprite;
		CHudStaticText* mTextSprite;

		std::wstring	mText;
		float			mFontSize[UNIT_COUNT];
		bool			mAutoHeight;
		bool			mUpdateText;
		bool            mCenter;

		friend class	CGUIObject;
	};
};

#endif

