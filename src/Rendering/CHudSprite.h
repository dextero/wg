#ifndef __C_HUD_SPRITE_H__
#define __C_HUD_SPRITE_H__

/* Klasa reprezentujaca sprite'a uzywanego do HUDa 
 * (czyli takiego, ktory jest 'przyklejony' do kamery)
 */

#include "IDrawable.h"
#include <SFML/Graphics/Rect.hpp>

namespace sf{
    class Sprite;
    class RenderWindow;
}

class CHudSprite : public IDrawable
{
public:
    CHudSprite();
    virtual ~CHudSprite();

	sf::Sprite* GetSFSprite();
	void SetClipPlane( double a, double b, double c, double d );	/* rownanie plaszczyzny wzgledem sprite'a */
	void SetClipRect( int left, int top, int right, int bottom );	/* wspolrzedne bezwzgledne */
	void SetClipRect( const sf::FloatRect& rect ); 

    virtual void Draw( sf::RenderWindow* renderWindow );

private:
    sf::Sprite* mSFSprite;
	bool mClipPlaneEnabled;
	bool mClipRectEnabled;
	double mClipPlane[4];
	int mClipRect[4];
};

#endif /*__C_HUD_SPRITE_H__*/

