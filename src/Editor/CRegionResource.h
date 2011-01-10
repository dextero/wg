#ifndef __CREGIONRESOURCE_H__
#define __CREGIONRESOURCE_H__

#include "IEditorResource.h"
#include "../Rendering/Primitives/CPrimitiveClass.h"
#include "../Rendering/Primitives/CPrimitivesDrawer.h"
#include "../Map/CMapManager.h"
#include "../Map/CRegionDescriptor.h"
#include <SFML/System/Vector2.hpp> 

#include <cmath>

class CRegionResource : public IEditorResource
{
public:
    CRegionResource() { mClass = gPrimitivesDrawer.GetClass("region"); };
    virtual ~CRegionResource() {};

	/**
     * Konfiguruje obiekt wyswietlajacy symbol zasobu (np. ikone kafla) w GUI
     */
    virtual void ConfigureButton(GUI::CButton * btn) {};

    /**
     * Przygotowuje obiekt wyswietlajacy "preview" zasobu na mapie
     */
    virtual void PrepareDisplayable(CDisplayable *d) {};
    
    /**
     * Uaktualnia obiekt wyswietlajacy "preview" zasobu na mapie
     */
    virtual void UpdateDisplayable(CDisplayable *d, sf::Vector2f pos, float rotation, float scale, bool z)
    {
        float a = scale / 2.f * sqrtf(2.f);
        gPrimitivesDrawer.DrawRect(mClass, sf::Vector2f(pos.x - a, pos.y - a), sf::Vector2f(pos.x + a, pos.y + a), 0.05f);
    };

    /**
     * Doloz ten zasob do mapy
     */
    virtual void Place(sf::Vector2f pos, float rotation, float scale, bool z)
    {
        gMapManager.GetCurrent()->AddRegion(new Map::CRegionDescriptor("", pos, scale), true);
    };

	/**
	 * Czy zasob ma byc umieszczany na mapie podczas przesuwania mysza (nie tylko podczas klikniecia)
	 */
	virtual bool ContinuousPlacement() { return false; }

private:
    CPrimitiveClass* mClass;

#ifdef __EDITOR__
    friend class CEditorWnd;
#endif
};


#endif // __CREGIONRESOURCE_H__