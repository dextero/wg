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
    std::string prefix = "data/abilities/";
    std::string ability;
    switch (gRand.Rnd(0, 40)) {
        default:
        case 0 : ability = "electric/ball-lightning.xml"; break;
        case 1 : ability = "electric/chain-lightning.xml"; break;
        case 2 : ability = "electric/devolution.xml"; break;
        case 3 : ability = "electric/electric-snare.xml"; break;
        case 4 : ability = "electric/electron.xml"; break;
        case 5 : ability = "electric/electrostatics.xml"; break;
        case 6 : ability = "electric/forked-lightning.xml"; break;
        case 7 : ability = "electric/laser.xml"; break;
        case 8 : ability = "electric/lightning-strike.xml"; break;
        case 9 : ability = "electric/magicstorm.xml"; break;
        case 10 : ability = "electric/transfusion.xml"; break;
        case 11 : ability = "fire/blazingfeet.xml"; break;
        case 12 : ability = "fire/elemental-revenge.xml"; break;
        case 13 : ability = "fire/exploding-corpse.xml"; break;
        case 14 : ability = "fire/firearrow.xml"; break;
        case 15 : ability = "fire/fireball.xml"; break;
        case 16 : ability = "fire/fire-circle.xml"; break;
        case 17 : ability = "fire/firepits.xml"; break;
        case 18 : ability = "fire/firewall.xml"; break;
        case 19 : ability = "fire/flaming-hands.xml"; break;
        case 20 : ability = "fire/flaming-mantle.xml"; break;
        case 21 : ability = "fire/quickshot.xml"; break;
        case 22 : ability = "fire/sparkrain.xml"; break;
        case 23 : ability = "support/healself.xml"; break;
        case 24 : ability = "support/superspeed.xml"; break;
        case 25 : ability = "special/acidcircus.xml"; break;
        case 26 : ability = "special/basher.xml"; break;
        case 27 : ability = "special/blades.xml"; break;
        case 28 : ability = "special/blizzard.xml"; break;
        case 29 : ability = "special/resonator.xml"; break;
        case 30 : ability = "old/entangle.xml"; break;
        case 31 : ability = "old/freezebomb.xml"; break;
        case 32 : ability = "old/frostnova.xml"; break;
        case 33 : ability = "old/hibernate.xml"; break;
        case 34 : ability = "old/icearrow.xml"; break;
        case 35 : ability = "old/icecircle.xml"; break;
        case 36 : ability = "old/ignite.xml"; break;
        case 37 : ability = "old/magic-missile.xml"; break;
        case 38 : ability = "old/meditation.xml"; break;
        case 39 : ability = "old/poison.xml"; break;
        case 40 : ability = "old/sting.xml"; break;
    }
    ability = prefix + ability;
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
