#include "CLootManager.h"
#include "CLoot.h"
#include "../../Utils/CRand.h"
#include "../../Map/CRandomMapGenerator.h"

//--------------------
CLoot * CLootManager::DropLootAt(const sf::Vector2f & pos)
{
    static float dropMod(0.f);

    // algorytm losowy + zwiekszanie prawdopodobienstwa ze graczowi cos jednak wypadnie ;)
    if ( (dropMod) > gRand.Rndf(0.f,1.1f) ) {
        dropMod = 0.f;
        CLoot * loot = gRandomMapGenerator.GenerateNextLoot();
        if (!loot) return NULL;
        loot->SetPosition(pos);
        return loot;
    }
    else // zwiekszenie prawdopodobienstwa wypadniecia jakiegos przedmiotu (dobrac eksperymentalnie)
        dropMod += 0.002f;

    return NULL;
}
