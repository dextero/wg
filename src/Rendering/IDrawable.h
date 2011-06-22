#ifndef __IDRAWABLE_H__
#define __IDRAWABLE_H__

/**
 * Klasa reprezentujaca obiekt rysowalny (np "duszka" :)), 
 */

namespace sf{
    class RenderWindow;
};

class IDrawable 
{
public:
    IDrawable():mIsVisible(true),mZVectorIndex(-1),mZIndex(-1) {};
    virtual ~IDrawable();

    virtual void Draw( sf::RenderWindow* renderWindow ) = 0;

    void SetVisible( bool newValue )
    {
        mIsVisible = newValue;
    };

    bool IsVisible() const
    {
        return mIsVisible;
    };

	int GetZIndex() const
	{
		return mZIndex;
	}

private:
    friend class CDrawableManager;

    bool mIsVisible;
    int mZVectorIndex;
    int mZIndex;
};

#endif /*__IDRAWABLE_H__*/
