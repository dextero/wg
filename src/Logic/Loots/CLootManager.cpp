#include "CLootManager.h"
#include "CLoot.h"
#include "../../Utils/CRand.h"
#include "../../Map/CRandomMapGenerator.h"

//--------------------

CLoot * CLootManager::DropLootAt(const sf::Vector2f & pos)
{
    static float dropMod(0.f);
    CLoot * loot = NULL;

    static float goldMod(0.f);

    // algorytm losowy + zwiekszanie prawdopodobienstwa ze graczowi cos jednak wypadnie ;)
    if ( (dropMod) > gRand.Rndf(0.f,1.1f) ) {
        dropMod = 0.f;
        goldMod = 0.f;
        loot = gRandomMapGenerator.GenerateNextLoot(0, pos);
    } else if ((goldMod > gRand.Rndf(0.f,0.4f))) {
        goldMod = 0.f;
        //to moze chociaz zloto?
        loot = gRandomMapGenerator.GenerateNextLoot(0, pos, "gold");
    } else { // zwiekszenie prawdopodobienstwa wypadniecia jakiegos przedmiotu (dobrac eksperymentalnie)
        dropMod += 0.002f;
        goldMod += 0.009f;
    }

    if (!loot) return NULL;
    loot->SetPosition(pos);
    return loot;
}
