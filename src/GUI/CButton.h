/*	GUI - przycisk
*
*	Przypisanie metody zwrotnej:
*	button->GetClickCallback()->bind( &obiekt, &metoda )
*	obiekt - obiekt dowolnej klasy; metoda - metoda tej¿e klasy np. &klasa::metoda
*
*	W przypadku funkcji globalnej zwrotnej: (...)->bind( &funkcja );
*
*	Ponadto mo¿na podpi¹æ funkcjê z parametrami:
*	button->GetClickParamCallback()->( jw )
*	button->SetClickCallbackParams( "1.0f 2.0f" );
*/

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "CGUIObject.h"
#include <string>
#include <FastDelegate.h>
#include <SFML/Graphics/Color.hpp>

class CHudSprite;
class CHudStaticText;
typedef fastdelegate::FastDelegate0<void> btn_callback;
typedef fastdelegate::FastDelegate1<const std::wstring&,void> btn_param_callback;

namespace GUI
{
	class CButton : public CGUIObject
	{
	public:
        void SetImage( const std::string& normal, const std::string& mouseOver="", const std::string& mouseClick="" );
		void SetFont( const std::string& name, float size, guiUnit u = UNIT_PIXEL );
		void SetColor( const sf::Color color );
		void SetText( const std::wstring& text );
		void SetText( const SLocalizedText& text, std::string font );
		void SetCenter( bool center );
		std::wstring GetText();
        CHudSprite* GetNormalSprite() { return mNormalSprite; }
        bool IsMouseOver() { return mMouseOver; }

        // ClickCallback rozni sie od eventow CGUIObject - potrzebuje nacisniecia I zwolnienia lewego przycisku
		inline btn_callback* GetClickCallback()	{ return &mClickCallback; }
		inline btn_param_callback* GetClickParamCallback() { return &mClickParamCallback; }
		inline void SetClickCallbackParams( const std::wstring& params ) { mClickCallbackParams = params; }

	private:
		CButton( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CButton();
		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		bool mMouseOver;
        bool mMousePressed;
		bool mCenter;
		CHudSprite*	mNormalSprite;
		CHudSprite* mMouseOverSprite;
        CHudSprite* mMouseClickSprite;
		CHudStaticText* mTextSprite;

		float mFontSize[UNIT_COUNT];
		btn_callback mClickCallback;
		btn_param_callback mClickParamCallback;
		std::wstring mClickCallbackParams;

		friend class CGUIObject;
	};
};

#endif//_BUTTON_H_

