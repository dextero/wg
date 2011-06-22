#ifndef __C_HUD_STATIC_TEXT_H__
#define __C_HUD_STATIC_TEXT_H__

/* Klasa reprezentujaca tekst rysowany na HUDzie 
 * (czyli taki, ktory jest 'przyklejony' do kamery, 
 * np jest elementem GUI, konsoli itp)
 */

#include "IDrawable.h"
#include <SFML/Graphics/Rect.hpp>
#include <string>

namespace sf{
    class String;
    class RenderWindow;
}

class CMultiColorString;

class CHudStaticText : public IDrawable
{
public:
    CHudStaticText();
    virtual ~CHudStaticText();
    virtual void Draw( sf::RenderWindow* renderWindow );

    sf::String* GetSFString();
    CMultiColorString* GetMultiColorString();

	/* Co prawda mozna ustawiac czcionke i pozycje bezposrednio w obiekcie sf::String
	 * ale ponizsze funkcje maja na celu upiekszenie wyswietlanego tekstu */

	void SetFont( const std::string& font, unsigned size );
	void SetFontSize( unsigned size );
	void SetPosition( float x, float y );

	void SetClipRect( int left, int top, int right, int bottom );	/* wspolrzedne bezwzgledne */
	void SetClipRect( const sf::FloatRect& rect ); 

private:
    CMultiColorString* mSFString;

	std::string mFont;
	unsigned mFontSize;

	bool mClipRectEnabled;
	int mClipRect[4];
};

#endif /*__C_HUD_STATIC_TEXT_H__*/

