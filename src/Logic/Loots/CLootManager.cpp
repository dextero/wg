#include "CLootManager.h"
#include "CLoot.h"
#include "../../Utils/CRand.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../Utils/CXml.h"
#include <algorithm> // dla std::sort && std::lower_bound
#include "../CPhysicalManager.h"
#include "../Factory/CLootTemplate.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
//#include "../../Utils/ToxicUtils.h"
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

    if (lootTemplates.size() == 0)
        return NULL;

    if (gRand.Rndf(0, 1) > 0.06f)
        return NULL;

    CLoot * tmp = lootTemplates.back()->Create();
    tmp->SetPosition(pos);

    //znow brutalnie, byleby miec co potestowac:
    std::string trigger = "a";
    switch (gRand.Rnd(0, 4)) {
        default:
        case 0 : break;
        case 1 : trigger = "b"; break;
        case 2 : trigger = "c"; break;
        case 3 : trigger = "d"; break;
    }
    std::string prefix = "data/player/";
    std::string ability = "firearrow.xml";
    switch (gRand.Rnd(0, 25)) {
        default:
        case 0 : ability = "acidcircus.xml"; break;
        case 1 : ability = "acidspray.xml"; break;
        case 2 : ability = "ball-lightning.xml"; break;
        case 3 : ability = "entangle.xml"; break;
        case 4 : ability = "firearrow.xml"; break;
        case 5 : ability = "fireball.xml"; break;
        case 6 : ability = "fireburst.xml"; break;
        case 7 : ability = "firecircle.xml"; break;
        case 8 : ability = "flaming-hands.xml"; break;
        case 9 : ability = "freezebomb.xml"; break;
        case 10 : ability = "frostnova.xml"; break;
        case 11 : ability = "healself.xml"; break;
        case 12 : ability = "hibernate.xml"; break;
        case 13 : ability = "icearrow.xml"; break;
        case 14 : ability = "iceball.xml"; break;
        case 15 : ability = "icecircle.xml"; break;
        case 16 : ability = "ignite.xml"; break;
        case 17 : ability = "invisibility.xml"; break;
        case 18 : ability = "lightning-strike.xml"; break;
        case 19 : ability = "magicstorm.xml"; break;
        case 20 : ability = "meditation.xml"; break;
        case 21 : ability = "poison.xml"; break;
        case 22 : ability = "powerslash.xml"; break;
        case 23 : ability = "pushback.xml"; break;
        case 24 : ability = "quickshot.xml"; break;
        case 25 : ability = "sting.xml"; break;
        case 26 : ability = "superspeed.xml"; break;
        case 27 : ability = "wrath.xml"; break;
    }
    tmp->SetCommandOnTake(StringUtils::ConvertToWString("set-ability player0 " + (prefix+ability) + " " + trigger));

    SEffectParamSet eps = gGraphicalEffects.Prepare("loot-circle");
    gGraphicalEffects.ShowEffect(eps,tmp);
    return tmp;
}

//--------------------
CLoot * CLootManager::DropLootAt(const sf::Vector2f & pos, unsigned maxLvlLoot)
{
//    if (ToxicUtils::isGameInCrimsonMode) {
//        return DropCrimsonModeItem(pos, maxLvlLoot, mLootTemplates);
//    }

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
