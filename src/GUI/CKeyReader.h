#ifndef KEY_READER_H
#define KEY_READER_H

#include "CGUIObject.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>
#include <FastDelegate.h>
#include <string>

class CHudSprite;
class CHudStaticText;
typedef fastdelegate::FastDelegate0<void> btn_callback;
typedef fastdelegate::FastDelegate1<const std::wstring&,void> btn_param_callback;
namespace sf { class Image; };

namespace GUI
{
	class CKeyReader : public CGUIObject
	{
	public:
		void SetImage( const std::string& normal, const std::string& mouseOver, const std::string& active );
		void SetFont( const std::string& name, float size, guiUnit u = UNIT_PIXEL );
		void SetColor( const sf::Color color );
		void SetKey( int key );
		void SetCenter( bool center );
		
		inline int GetKey()	{ return mKey; }

		inline btn_callback* GetClickCallback()	{ return &mClickCallback; }
		inline btn_param_callback* GetClickParamCallback() { return &mClickParamCallback; }
		inline void SetClickCallbackParams( const std::wstring& params ) { mClickCallbackParams = params; }
        
		virtual void UpdateSprites( float secondsPassed );

	private:
		CKeyReader( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CKeyReader();
		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void OnKeyEvent( const sf::Event::KeyEvent& e, bool pressed );

        void SetKeyFromMouseButton(int btn);

		bool mMouseOver;
        bool mActive;
		bool mReadyToActive;
		bool mCenter;

		CHudSprite*		mBackgroundSprite;
		sf::Image*		mNormalImg;
		sf::Image*		mMouseOverImg;
		sf::Image*		mActiveImg;

		CHudStaticText* mTextSprite;
		float			mFontSize[UNIT_COUNT];

		int mKey;
        
		btn_callback mClickCallback;
		btn_param_callback mClickParamCallback;
		std::wstring mClickCallbackParams;

		friend class CGUIObject;
	};
};

#endif
