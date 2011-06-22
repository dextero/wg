/* Prawzorzec wszystkich wzorcow :P wspiera rowniez kilka wspolnych cech physicali */

#ifndef _CPHYSICAL_TEMPLATE_H_
#define _CPHYSICAL_TEMPLATE_H_

#include "../../ResourceManager/IResource.h"
#include "../CPhysical.h"
#include "../../Utils/CXml.h"
#include <string>
#include <ostream>

#include <SFML/Graphics/Color.hpp>

class CXml;
class CTemplateParam;

class CPhysicalTemplate : public IResource
{
public:
	/* IResource, ot tak dla przypomnienia */
	std::string GetType() = 0;
	void Drop() = 0;
    
    // metoda Load jest niewirtualna
    // wywoluje przeciazana, XMLowa wersje
	bool Load(std::map<std::string,std::string>& argv);

    // ladowanie z XMLa - wolane przez podklasy by zaladowac radius itp
    virtual bool Load(CXml &xml);

	// Uniwersalna metoda tworzenia physicali
    // implementowana przez w pelni konkretne podklasy
	virtual CPhysical* Create(std::wstring id = L"") = 0;

	// Metoda do przypisywania obiektowi CDisplayable domyslnego sprite'a badz animacji
	// co ma zastosowanie w edytorze
	virtual void PrepareEditorPreviewer( CDisplayable* d ) {}

    // Jezeli zwraca true dany Physical traktowany jest jako niezbedny i nie bedzie
    // pomijany w przypadku wczytywania mapy. Jezeli zwraca false oznacza to, 
    // ze danego physicala mozna pominac podczas wszytywania mapy
    virtual bool CoreMapObject() const = 0;

    // Ustawia sciezke do obrazka, jezeli physical nie uzywa animacji - do invisible-walli
    virtual void SetImageFilename(const std::string& path) {}

protected:
    std::string mFilename;

	float mMinScale,	 mMaxScale;
	float mMinRadius,	 mMaxRadius;
    float mColorRed, mColorGreen, mColorBlue, mColorAlpha;
    float mMinShadowOffset, mMaxShadowOffset;

	std::wstring mGenre;
public:
    inline const std::string & GetFilename() const { return mFilename; }

    inline float GetMaxRadius(){ return mMaxRadius; }
    inline sf::Color GetColor() { return sf::Color((sf::Uint8)(mColorRed * 255), (sf::Uint8)(mColorGreen * 255), (sf::Uint8)(mColorBlue * 255), (sf::Uint8)(mColorAlpha * 255)); }
    inline float GetMinScale() { return mMinScale; }

    virtual CTemplateParam *ReadParam(CXml &xml, xml_node<> *node, CTemplateParam *orig = NULL);
    virtual void Parametrise(CPhysical *obj,CTemplateParam *param);
    virtual void SerializeParam(std::ostream &out, CTemplateParam *param, int indent = 0);
};

#endif // _CPHYSICAL_TEMPLATE_H_

