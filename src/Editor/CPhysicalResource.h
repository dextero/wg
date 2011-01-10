#ifndef PHYSICAL_RESOURCE_H
#define PHYSICAL_RESOURCE_H

#include "IEditorResource.h"
#include <SFML/System/Vector2.hpp> 

class CPhysicalTemplate;

class CPhysicalResource : public IEditorResource
{
public:
	CPhysicalResource(const std::string &path);
	virtual ~CPhysicalResource();

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
	virtual bool ContinuousPlacement() { return false; }

private:
	std::string mPath;
    CPhysicalTemplate* mTemplate;

#ifdef __EDITOR__
    friend class CEditorWnd;
#endif
};

#endif