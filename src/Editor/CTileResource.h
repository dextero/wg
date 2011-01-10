#ifndef __CTILERESOURCE_H__
#define __CTILERESOURCE_H__

#include "IEditorResource.h"
#include <SFML/System/Vector2.hpp> 

namespace System
{
	namespace Resource
	{
        class CImage;
    }
}

class CTileResource : public IEditorResource {
public:
    CTileResource(const std::string &path);
    virtual ~CTileResource();
    
    /**
     * Konfiguruje obiekt wyswietlajacy symbol zasobu (np. ikone kafla) w GUI
     */
    virtual void ConfigureButton(GUI::CButton * btn);

    /**
     * Przygotowuje obiekt wyswietlajacy "preview" zasobu na mapie
     */
    virtual void PrepareDisplayable(CDisplayable *d);
    
    /**
     * Uaktualnia obiekt wyswietlajacy "preview" zasobu na mapie
     */
    virtual void UpdateDisplayable(CDisplayable *d, sf::Vector2f pos, float rotation, float scale, bool z);

    /**
     * Doloz ten zasob do mapy
     */
    virtual void Place(sf::Vector2f pos, float rotation, float scale, bool z);

	/**
	 * Czy zasob ma byc umieszczany na mapie podczas przesuwania mysza (nie tylko podczas klikniecia)
	 */
	virtual bool ContinuousPlacement() { return true; }
private:
    std::string mPath;
    System::Resource::CImage *mImage;

#ifdef __EDITOR__
    friend class CEditorWnd;
#endif
};


#endif//__CTILERESOURCE_H__
