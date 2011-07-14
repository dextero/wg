#ifndef __C_DISPLAYABLE_H__
#define __C_DISPLAYABLE_H__

/* Klasa reprezentujaca jakies cos w __swiecie gry__
 * domyslnie to cos bedzie chcialo zajmowac 64x64 piksle
 * na ekranie
 *
 * domyslnie ustawienie skali = 1.0 1.0 bedzie powodowac
 * rozciagniecie obrazka do 64x64 piksli na ekranie
 *
 * domyslnie wyswietlany obiekt bedzie starac sie renderowac
 * dookola wyznaczonej pozycji X,Y (w swiecie gry), a nie
 * tylko w prawo dol 
 */

// rozwazyc: opakowac te wszystkie klasy tutaj obok w namespace'a

#include "IDrawable.h"
#include "CWGSprite.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <string>

namespace sf{
    class Sprite;
    class RenderWindow;
}

namespace System{
    namespace Resource{
        class CImage;
    }
}
class CImagePart;

struct SAnimationState;
struct SAnimation;

namespace GUI{
    class CAnimatedImage;
}

class CDisplayable : public IDrawable
{
public:
    CDisplayable();
    virtual ~CDisplayable();

    virtual void Draw( sf::RenderWindow* renderWindow );

    void SetStaticImage( const std::string& imageName, int number = -1, float extraPixels = 0);
    void SetStaticImage( System::Resource::CImage *img, int number = -1, float extraPixels = 0);

    void SetStaticImageFromAtlas( const std::string& atlasName, int number);
    void SetStaticImageFromAtlas( CImagePart *img );

    void SetAnimation( const std::string& animationName );
    void SetAnimation( SAnimation *animationPtr );


    // tox, 4 luty
    // umawiamy sie, ze SetScale(1.0, 1.0) ma obrazek przeskalowac do wielkosci
    // jednego kafla niezaleznie od fizycznego rozmiaru obrazka, oks?
    void SetScale( float x, float y );
    void SetScale( float uniform );
    void SetScale( const sf::Vector2f& newValue );

	sf::Vector2f GetScale();
    // void SetScale( const sf::Vector2f& vec );

    // tox, 4 luty
    // umawiamy sie, ze SetPosition(x, y) stara sie przesunac na pozycje x,y 
    //   *srodek* obrazka/animacji (a nie jego lewy-gorny rog...)
    //   
    //  wspolrzedne: 0,0 oznaczalo powiedzmy lewy gorny rog pierwszego kafla, zas:
    //               1,0 oznaczalo prawy-gorny rog pierwszego kafla, zas:
    //               m,n oznaczalo lewy gorny rog kafla m,n-tego
    void SetPosition( float x, float y );
    void SetPosition( const sf::Vector2f& vec );
	sf::Vector2f GetPosition();

    void SetSubRect( int ul, int ur, int ll, int lr );
    void SetSubRect ( sf::IntRect rect );

    void SetRotation( float rot );
    float GetRotation();

    SAnimationState* GetAnimationState();
    const std::string& GetImageName();

	void SetCanDraw(bool draw);
	bool GetCanDraw();

    void SetColor( const sf::Color & color );
    void SetColor( float r, float g, float b, float a = 1.0f);
	void GetColor( float* r, float* g, float* b, float* a );

    // przycinanie
	void SetClipPlane( double a, double b, double c, double d );	/* rownanie plaszczyzny wzgledem sprite'a */
	void SetClipRect( int left, int top, int right, int bottom );	/* wspolrzedne bezwzgledne */
	void SetClipRect( const sf::FloatRect& rect ); 

    CWGSprite * GetSFSprite();
private:
    CWGSprite* mSFSprite;
    SAnimationState* mAnimationState;
    std::string mAnimationName; // <-- to jest komus potrzebne na zewnatrz?
    std::string mImageName; // do edytora
    sf::Vector2f mScale; // 1.0 1.0 oznacza wielkosc jednego kafla
	bool m_canDraw; // oznacza czy mozna rysowac tego kafla
    
    // przycinanie
	bool mClipPlaneEnabled;
	bool mClipRectEnabled;
	double mClipPlane[4];
	int mClipRect[4];

    friend class GUI::CAnimatedImage;
};

#endif /*__C_DISPLAYABLE_H__*/

