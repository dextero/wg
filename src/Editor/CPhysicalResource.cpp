#include "CPhysicalResource.h"
#include "../Map/CMapManager.h"
#include "../Map/CMap.h"
#include "../Utils/StringUtils.h"
#include "../Logic/Factory/CPhysicalTemplate.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Rendering/ZIndexVals.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CDisplayable.h"
#include "../GUI/CButton.h"
#include <SFML/Graphics/Sprite.hpp>
#include <vector>

CPhysicalResource::CPhysicalResource(const std::string &path)
:	mPath(path)
{
	mTemplate = gResourceManager.GetPhysicalTemplate(path);
}

CPhysicalResource::~CPhysicalResource()
{
}

void CPhysicalResource::ConfigureButton(GUI::CButton *btn)
{
	std::string s;
	std::vector<std::string> v;
	StringUtils::Exclude(mPath, '/', v);

	if (v.size() > 0)	StringUtils::Exclude(s = v[v.size()-1], '.', v);
	if (v.size() > 0)	StringUtils::Exclude(s = v[0], '-', v);

	s = "";
	for (unsigned i = 0; i < v.size(); i++)
		s += v[i] + "\n";

	btn->SetText( StringUtils::ConvertToWString(s) );
}

void CPhysicalResource::PrepareDisplayable(CDisplayable *d)
{
    if (mTemplate != NULL)
    {
        d->SetScale(mTemplate->GetMinScale());
        d->SetColor(mTemplate->GetColor());
    }
    else
    {
	    d->SetScale(1.0f, 1.0f);
        d->SetColor(1.f, 1.f, 1.f);
    }

	if (d->GetZIndex() != Z_PHYSICAL)
	{
		gDrawableManager.UnregisterDrawable(d);
		gDrawableManager.RegisterDrawable(d, Z_PHYSICAL);
	}

	if (mTemplate)
	{
        // jesli to invisible-wall, to pokazujemy
        if (mTemplate->GetFilename().find("invisible-wall") != std::string::npos)
            mTemplate->SetImageFilename("data/physicals/walls/invisible-wall.png");

		mTemplate->PrepareEditorPreviewer(d);
	}
}

void CPhysicalResource::UpdateDisplayable(CDisplayable *d, sf::Vector2f pos, float rotation, float scale, bool z)
{
	d->SetPosition(pos.x, pos.y);

    // invisible-walli sie nie obraca!
    if (!(mTemplate && mTemplate->GetFilename().find("invisible-wall") != std::string::npos))
	    d->SetRotation(rotation);
    else
        d->SetRotation(0.f);
}

void CPhysicalResource::Place(sf::Vector2f pos, float rotation, float scale, bool z)
{
	if (Map::CMap* map = gMapManager.GetCurrent())
	{
        // invisible-walli sie nie obraca!
        if (!(mTemplate && mTemplate->GetFilename().find("invisible-wall") == std::string::npos))
            rotation = 0.f;

		std::string code = map->GetOrCreateMapObjectCode(mTemplate);
		map->AddMapObject(code, pos.x, pos.y, rotation, true);
	}
}

