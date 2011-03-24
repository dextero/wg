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

CLoot * DropCrimsonModeLoot(const sf::Vector2f & pos, unsigned maxLvlLoot, const std::vector<CLootTemplate *> & lootTemplates) {
    // krotko i brutalnie:

//    if (lootTemplates.size() == 0)
//        return NULL;

//    if (gRand.Rndf(0, 1) > 0.06f)
//        return NULL;

    CLoot * tmp = lootTemplates.back()->Create();
    tmp->SetPosition(pos);

    std::string prefix = "data/player/";
    std::string ability;
    switch (gRand.Rnd(0, 13)) {
        default:
        case 0 : ability = "electric/ball-lightning.xml"; break;
        case 1 : ability = "electric/chain-lightning.xml"; break;
        case 2 : ability = "electric/debuff-mastery.xml"; break;
        case 3 : ability = "electric/devolution.xml"; break;
        case 4 : ability = "electric/electricity-mastery.xml"; break;
        case 5 : ability = "electric/electric-snare.xml"; break;
        case 6 : ability = "electric/electricution.xml"; break;
        case 7 : ability = "electric/electron.xml"; break;
        case 8 : ability = "electric/electrostatics.xml"; break;
        case 9 : ability = "electric/forked-lightning.xml"; break;
        case 10 : ability = "electric/laser.xml"; break;
        case 11 : ability = "electric/lightning-strike.xml"; break;
        case 12 : ability = "electric/magicstorm.xml"; break;
        case 13 : ability = "electric/transfusion.xml"; break;
    }
    ability = prefix + ability;
    CItem * item = new CItem();
    item->SetAbility(ability);
    tmp->SetItem(item);

    SEffectParamSet eps = gGraphicalEffects.Prepare("loot-circle");
    gGraphicalEffects.ShowEffect(eps,tmp);
    return tmp;
}

//--------------------
CLoot * CLootManager::DropLootAt(const sf::Vector2f & pos, unsigned maxLvlLoot)
{
    return DropCrimsonModeLoot(pos, maxLvlLoot, mLootTemplates);

    static float dropMod(0.f);

    if (mLootTemplates.size() == 0)
        return NULL;

    //// ograniczenie lootsow do okreslonego zakresu poziomow

    // Liosan, 18.05.09
    // Wiesz co, rAum... nie wiem jak u Ciebie, ale u mnie sie to nie kompiluje.
    // Co gorsza nie wiem co zrobic, zeby dzialalo
    // Wydaje mi sie, ze pisanie nowego zaprzyjaznionego operatora tylko po to, by miec lower_bound
    // to jest przegiecie. Wywalam - naprawde jestem w stanie napisac ten algorytm sam :P

   /* std::vector<CLootTemplate *>::iterator it = 
        std::lower_bound( mLootTemplates.begin(), mLootTemplates.end(), CLootTemplate(maxLvlLoot+1) );
    int choice = gRand.Rnd(0, static_cast<unsigned>( it - mLootTemplates.begin() ) ); */
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
        CLoot *tmp = t->Create();
        tmp->SetPosition(pos);
        SEffectParamSet eps = gGraphicalEffects.Prepare("loot-circle");
        gGraphicalEffects.ShowEffect(eps,tmp);
        fprintf (stderr, "Loot drop - %ls\n", tmp->GetLootName().c_str() );
        return tmp;
    }
    else // zwiekszenie prawdopodobienstwa wypadniecia jakiegos przedmiotu (dobrac eksperymentalnie)
        dropMod += 0.002f;

    return NULL;
}
