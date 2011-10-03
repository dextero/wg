#include "CTileResource.h"
#include "../Map/CMapManager.h"
#include "../Map/CMap.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Rendering/ZIndexVals.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CDisplayable.h"
#include "../GUI/CButton.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>

#ifdef __EDITOR__
#   include "../Map/CTile.h"
#   include "src/CActionsList.h"
#   include "CEditor.h"
#   include "../Map/CRandomMapGenerator.h"
#endif

CTileResource::CTileResource(const std::string &path):
mPath(path){
    mImage = gResourceManager.GetImage(path);
}

CTileResource::~CTileResource(){
}

void CTileResource::ConfigureButton(GUI::CButton * btn){
    btn->SetImage(mPath);
}

void CTileResource::PrepareDisplayable(CDisplayable *d){
    d->SetStaticImage(mPath);
}

void CTileResource::UpdateDisplayable(CDisplayable *d, sf::Vector2f pos, float rotation, float scale, bool z){
    float x = floorf(pos.x + 0.5f);
    float y = floorf(pos.y + 0.5f);
    d->SetPosition(x,y);
	d->SetRotation(0.0f);
	d->SetScale(0.3f, 0.3f);

	if (d->GetZIndex() != Z_TILE)
	{
		gDrawableManager.UnregisterDrawable(d);
		gDrawableManager.RegisterDrawable(d, Z_TILE);
	}
}

void CTileResource::Place(sf::Vector2f pos, float rotation, float scale, bool z){
	if (Map::CMap *map = gMapManager.GetCurrent())
	{
		std::string code = map->GetOrCreateTileCode(mPath);

#ifdef __EDITOR__
        int index = (int)pos.y * map->mMapHeader->Width + (int)pos.x;

        unsigned int maskCount = gRandomMapGenerator.GetTileMaskCount();
        // pierwsza tablica to powinny byc numery masek, TODO
        STileChanged tc = { { 
            map->GetTileMaskId((int)pos.x, (int)pos.y),
            map->GetTileMaskId((int)pos.x, (int)pos.y + 1),
            map->GetTileMaskId((int)pos.x + 1, (int)pos.y),
            map->GetTileMaskId((int)pos.x + 1, (int)pos.y + 1) },
            { rand() % maskCount, rand() % maskCount, rand() % maskCount, rand() % maskCount, rand() % maskCount }
        };

        if (map->mFields->at(index) && (code != map->mFields->at(index)->GetCode()))
            gActionsList.Push(SAction::ChangeTile(pos.x, pos.y, map->mFields->at(index)->GetCode(), code, tc));

        map->SetTileCorner(pos.x, pos.y, mPath);
#endif

	}
}
