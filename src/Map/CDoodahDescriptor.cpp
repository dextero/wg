#include "CDoodahDescriptor.h"
#include "../Rendering/Animations/CAnimationManager.h"
#include "../Rendering/ZIndexVals.h"
#include "../Rendering/CDisplayable.h"
#include "../Rendering/CDrawableManager.h"
#include "SceneManager/CSceneNode.h"
#include "SceneManager/CQuadTreeSceneManager.h"
#include "CMapManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Image.hpp>

Map::CDoodahDescriptor::CDoodahDescriptor()
:	x(0.0f),
	y(0.0f), 
	scale(1.0f),
	rotation(0.0f),
	zindex(Z_MAPSPRITE_FG)
{
	static int nextId = 1;
	uniqueId = nextId++;
}

void Map::CDoodahDescriptor::Create()
{
	CDisplayable * sprite = gDrawableManager.CreateDisplayable( zindex );
    if (!anim.empty())
    {
        sprite->SetAnimation( anim );
        sprite->SetScale(scale);
    }
    else
    {
        sprite->SetStaticImage( file );
        sf::Sprite * sfSprite = sprite->GetSFSprite();
        if (sfSprite->GetImage() == NULL)
            return; // error - plik nie istnieje, nie dopuszczamy do crasha...
        sfSprite->Resize( sfSprite->GetImage()->GetWidth() * scale, sfSprite->GetImage()->GetHeight() * scale );
    }
	sprite->SetPosition( x, y );
	sprite->SetRotation( rotation );

	CSceneNode * node = new CSceneNode;
	node->SetDisplayable( sprite );
	node->SetDoodahDescriptorId( uniqueId );
	gScene.AddSceneNode( node );
}

