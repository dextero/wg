/* GUI - pole tekstowe (wszystkie opisy, dialogi etc. ) */

#ifndef _TEXT_AREA_H_
#define _TEXT_AREA_H_

#include "CGUIObject.h"
#include "CRoot.h" // FontSetting
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
        void SetFont(const GUI::FontSetting & fs);
        void SetFont(const GUI::FontSetting & fs, float scale);
		void SetFont( const std::string& name, float size, guiUnit u = UNIT_PIXEL );
		void SetColor(const sf::Color color);
		void SetText( const std::wstring& text );
		void SetLocalization( const std::string& localization );
		void SetAutoHeight( bool autoHeight );
		void SetCenter( bool center );
		void UpdateText();

		inline std::wstring GetText()			{ return mText; }
		inline std::string GetLocalization()	{ return mLocalization; }
		inline float		GetTotalFontSize()	{ return mFontSize[UNIT_PERCENT] * mInternalRect.GetHeight() + mFontSize[UNIT_PIXEL]; }
        sf::String *		GetSFString();

        static const std::wstring GetNextColorString(sf::Color col);
	private:
		CTextArea( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CTextArea();

		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		CHudSprite*		mBackgroundSprite;
		CHudStaticText* mTextSprite;

		std::wstring	mText;
		std::string	    mLocalization;
		float			mFontSize[UNIT_COUNT];
		bool			mAutoHeight;
		bool			mUpdateText;
		bool            mCenter;

		friend class	CGUIObject;
	};
};

#endif

