#include "CLootManager.h"
#include "CLoot.h"
#include "../../Utils/CRand.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../Utils/CXml.h"
#include <algorithm> // dla std::sort && std::lower_bound
#include "../CPhysicalManager.h"
#include "../Factory/CLootTemplate.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../Items/CItem.h"
#include "../../Utils/StringUtils.h"
#include "../../Map/CRandomMapGenerator.h"
#include "../../Map/CMapManager.h"

template<> CLootManager* CSingleton<CLootManager>::msSingleton = 0;

void CLootManager::Clear()
{
    mLootTemplates.clear();
}

bool CLootManager::LoadLoots(const std::string &filename)
{
    if (mLootTemplates.size() > 0)
        return true;

    CXml xml(filename, "root");
    
    if (xml.GetString(0,"type") != "droplist")
        return false;

    fprintf ( stderr, "CLootManager loading loots data from %s ...", filename.c_str() );

    size_t counter = 0;
    CLootTemplate *tmp = NULL;
    std::string file;
    xml_node<>* node;
    for (node = xml.GetChild(0,"loot"); node; node = xml.GetSibl(node, "loot"))
    {
        ++counter;

        file = xml.GetString(node, "filename");
        tmp = dynamic_cast<CLootTemplate *> ( gResourceManager.GetPhysicalTemplate( file ) );
        
        if (tmp != NULL) {
            mLootTemplates.push_back(tmp);
        }
        else {
            fprintf (stderr, "CLootManager::LoadItems - can't locate loot file (%s)\n", file.c_str());
        }
    }

    /// sortowanie wzgledem poziomu przedmiotow - ulatwi to losowanie :>
    std::sort(mLootTemplates.begin(), mLootTemplates.end());

    fprintf( stderr, "Loading loots data finished, loaded %u loots (skipped %u loots)\n", mLootTemplates.size(), counter - mLootTemplates.size() );

    return true;
}

void CLootManager::BindRandomWeaponToLoot(CLoot * loot) {
    std::string ability = gRandomMapGenerator.GetRandomWeaponFile(gMapManager.GetLevel());
    CItem * item = new CItem();
    item->SetAbility(ability);
    loot->SetItem(item);
}

//--------------------
CLoot * CLootManager::DropLootAt(const sf::Vector2f & pos, unsigned maxLvlLoot)
{
    static float dropMod(0.f);

    if (mLootTemplates.size() == 0)
        return NULL;

    unsigned int max = 0;
    CLootTemplate refLootTempl(maxLvlLoot+1);
    for (unsigned int i = 0; i < mLootTemplates.size(); i++)
        if (mLootTemplates[i] < refLootTempl)
            max = i;
        else
            break;
    int choice = gRand.Rnd(0, max);
    CLootTemplate *t = mLootTemplates[choice];

    // algorytm losowy + zwiekszanie prawdopodobienstwa ze graczowi cos jednak wypadnie ;)
    if ( (t->mLoot.dropRate + dropMod) > gRand.Rndf(0.f,1.1f) ) {
        dropMod = 0.f;
        CLoot * loot = t->Create(); //wewnatrz zostanie zawolany BindRandomWeaponToLoot(loot)
        loot->SetPosition(pos);
        fprintf (stderr, "Loot drop - %ls\n", loot->GetLootName().c_str() );
        return loot;
    }
    else // zwiekszenie prawdopodobienstwa wypadniecia jakiegos przedmiotu (dobrac eksperymentalnie)
        dropMod += 0.002f;

    return NULL;
}
