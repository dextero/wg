#ifndef CSPRITEMANAGER_H_
#define CSPRITEMANAGER_H_

/**
 * Zarzadca obiektow rowanych na ekranie. Odpowiada za wywolanie u nich Draw()
 * co klatke (docelowo w odpowiedniej kolejnosci - wg zIndex).
 */

#include <vector>
#include <SFML/System/Vector2.hpp>

#include "../IFrameListener.h"
#include "../Utils/CSingleton.h"
#include "ZIndexVals.h"

namespace sf {
	class RenderWindow;
	class Image;
}

class IDrawable;
class CHudSprite;
class CHudStaticText;
class CDisplayable;
class SLight;

typedef std::vector< IDrawable* > DrawableList;
typedef std::vector< DrawableList > DrawableLists;

#define gDrawableManager CDrawableManager::GetSingleton()

class CDrawableManager: public CSingleton<CDrawableManager>, IFrameListener  {
public:
    CDrawableManager();
    ~CDrawableManager();

    CHudSprite* CreateHudSprite( int zIndex = Z_GUI1 );
    CHudSprite* CloneHudSprite(CHudSprite* sprite, int zIndex = -1);
    CHudStaticText* CreateHudStaticText( int zIndex = Z_GUI1 );
    CDisplayable* CreateDisplayable( int zIndex = Z_PHYSICAL );
    void DestroyDrawable( IDrawable* drawable );
    
    // rejestruje drawable; jesli wczesniej byl rejestrowany,
    // to go odrejestruje
    void RegisterDrawable( IDrawable* drawable, int zIndex );
    // derejestruje drawable bez usuwania
    void UnregisterDrawable( IDrawable *drawable );

	SLight* CreateLight();
	void DestroyLight(SLight* light);

	inline void SetLighting(bool lighting)	{ mLightingEnabled = lighting; }
	inline bool LightingEnabled()			{ return mLightingEnabled; }

    virtual void FrameStarted(float secondsPassed) {};
	void DrawFrame( sf::RenderWindow* wnd = NULL );

    virtual bool FramesDuringPause(EPauseVariant pv){ return true; }
private:
	/* Znajdz [count] najmocniej swiecacych w punkcie [pos] swiatel
	 * i umiesc je w tablicy out */
	void GetStrongestLights(SLight** out, unsigned count, const sf::Vector2f& pos);
	void DrawWithNormalMapping(sf::RenderWindow* wnd, CDisplayable* displayable, const sf::Image* normalmap);
	void DrawWithPhongLighting(sf::RenderWindow* wnd, CDisplayable* displayable);

    DrawableLists mLayers;
	std::vector<SLight*> mLights;
	bool mLightingEnabled;
};

#endif /*CSPRITEMANAGER_H_*/
