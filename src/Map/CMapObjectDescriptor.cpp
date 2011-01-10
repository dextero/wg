#include "CMapObjectDescriptor.h"
#include "SceneManager/CSceneNode.h"
#include "../Logic/Factory/CPhysicalTemplate.h"
#include "../Logic/Boss/CBossManager.h"
#include "../Utils/StringUtils.h"
#include "../Logic/CActor.h"
#include "../Logic/CActorController.h"

Map::CMapObjectDescriptor::CMapObjectDescriptor() :
    code(""),
    name(""),
    templ(NULL),
    param(NULL),
    x(0.f),
    y(0.f),
    rot(0),
    bossPlaylist(""),
    bossAI(""),
    bossRadius(0.f)
{
	static int nextId = 1;
	uniqueId = nextId++;
}

void Map::CMapObjectDescriptor::Create(){
    CPhysical *phys = templ->Create(StringUtils::ConvertToWString(name));
    if (phys!=0){
        phys->SetPosition(sf::Vector2f(x,y));
        phys->SetRotation(rot);
		phys->GetSceneNode()->SetMapObjectDescriptorId( uniqueId );

        // jesli mamy radius, to jest to boss i trzeba go sledzic
        // bo jeszcze nagrandzi i nie bedzie mial kto sprzatac
        if (bossRadius > 0.f)
            gBossManager.Trace(phys->GetUniqueId(), bossAI, bossRadius, bossPlaylist);

		CActor * actor = dynamic_cast<CActor*>(phys);
		if (actor){
			if (actor->GetActorController()){
				actor->GetActorController()->SetTrueRot(phys->GetRotation());
			}
		}
    }
    if (param!=NULL)
        templ->Parametrise(phys,param);
}

bool Map::CMapObjectDescriptor::CoreMapObject() const
{
    return templ->CoreMapObject();
}
