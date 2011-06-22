#ifndef __IEDITORRESOURCE_H__
#define __IEDITORRESOURCE_H__

#include <SFML/System/Vector2.hpp> 
#include <string>

namespace GUI{
class CButton;
}
class CDisplayable;

class IEditorResource {
public:    
    /**
     * Konfiguruje obiekt wyswietlajacy symbol zasobu (np. ikone kafla) w GUI
     */
    virtual void ConfigureButton(GUI::CButton * btn) = 0;
    
    /**
     * Przygotowuje obiekt wyswietlajacy "preview" zasobu na mapie
     */
    virtual void PrepareDisplayable(CDisplayable *d) = 0;

    /**
     * Uaktualnia obiekt wyswietlajacy "preview" zasobu na mapie
     */
	virtual void UpdateDisplayable(CDisplayable *d, sf::Vector2f pos, float rotation, float scale, bool z) = 0;

    /**
     * Doloz ten zasob do mapy
     */
    virtual void Place(sf::Vector2f pos, float rotation, float scale, bool z) = 0;

	/**
	 * Czy zasob ma byc umieszczany na mapie podczas przesuwania mysza (nie tylko podczas klikniecia)
	 */
	virtual bool ContinuousPlacement() = 0;
};


#endif//__IEDITORRESOURCE_H__
