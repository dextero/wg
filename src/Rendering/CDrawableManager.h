#ifndef CSPRITEMANAGER_H_
#define CSPRITEMANAGER_H_

/**
 * Zarzadca obiektow rowanych na ekranie. Odpowiada za wywolanie u nich Draw()
 * co klatke (docelowo w odpowiedniej kolejnosci - wg zIndex).
 */

#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

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
	void DestroyAllLights();

	inline void SetLighting(bool lighting)	{ mLightingEnabled = lighting; }
	inline bool LightingEnabled()			{ return mLightingEnabled; }

	inline void SetAmbient(const sf::Color& a)	{ mAmbient = a; }
	inline const sf::Color& GetAmbient()		{ return mAmbient; }

	inline void SetNormalMappingAmbient(const sf::Color& a)	{ mNormalMappingAmbient = a; }
	inline const sf::Color& GetNormalMappingAmbient()		{ return mNormalMappingAmbient; }

	// Domyslnie 1.0f
	inline void SetNormalMappingContrast(float contrast)	{ mNormalMappingContrast = contrast; }
	inline float GetNormalMappingContrast()					{ return mNormalMappingContrast; }

    virtual void FrameStarted(float secondsPassed) {};
	void DrawFrame( sf::RenderWindow* wnd = NULL );

    virtual bool FramesDuringPause(EPauseVariant pv){ return true; }
private:

	/* Struktura opisujaca wierzcholki uzywane do renderowania swiatel */
	struct vertexDesc
	{
		float u, v;
		unsigned char r, g, b, a;
		float x, y, z;
	};

	/* Znajdz [count] najmocniej swiecacych w punkcie [pos] swiatel
	 * i umiesc je w tablicy out */
	void GetStrongestLights(SLight** out, unsigned count, const sf::Vector2f& pos);
	void DrawWithNormalMapping(sf::RenderWindow* wnd, CDisplayable* displayable, const sf::Image* normalmap);
	void DrawWithPerPixelLighting(sf::RenderWindow* wnd, CDisplayable* displayable);

	void FillLightsUV();
	void DrawLightsGeometry();

    DrawableLists mLayers;
	sf::Color mAmbient;
	sf::Color mNormalMappingAmbient;
	std::vector<SLight*> mLights;
	vertexDesc* mLightsGeometry;
	unsigned mLightsGeometrySize; // zalozmy ze chodzi o ilosc swiatel - nie wierzcholkow
	float mNormalMappingContrast;
	bool mLightingEnabled;
};

#endif /*CSPRITEMANAGER_H_*/
