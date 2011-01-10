#include "CDoodahResource.h"
#include "../Map/CMapManager.h"
#include "../Map/CMap.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../GUI/CButton.h"
#include "../Rendering/CDisplayable.h"
#include "../Rendering/ZIndexVals.h"
#include "../Rendering/CDrawableManager.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>

CDoodahResource::CDoodahResource(const std::string &path)
:	mPath(path) {
    mImage = gResourceManager.GetImage(path);
}

CDoodahResource::~CDoodahResource(){
}

void CDoodahResource::ConfigureButton(GUI::CButton * btn){
    btn->SetImage(mPath);
}

void CDoodahResource::PrepareDisplayable(CDisplayable *d){
    d->SetStaticImage(mPath);
}

void CDoodahResource::UpdateDisplayable(CDisplayable *d, sf::Vector2f pos, float rotation, float scale, bool z){
    d->SetPosition(pos.x,pos.y);
	d->SetRotation(rotation);
	d->GetSFSprite()->SetScale( scale, scale );

	int newZ = (z ? Z_MAPSPRITE_FG : Z_MAPSPRITE_BG) - 1; // -1, zeby byl ponad juz postawionymi

	if (newZ != d->GetZIndex())
	{
		gDrawableManager.UnregisterDrawable( d );
		gDrawableManager.RegisterDrawable( d, newZ );
	}
}

void CDoodahResource::Place(sf::Vector2f pos, float rotation, float scale, bool z){
	if (Map::CMap *map = gMapManager.GetCurrent())
		map->AddDoodah(mPath,pos.x,pos.y,z ? Z_MAPSPRITE_FG : Z_MAPSPRITE_BG,rotation,scale);
}

