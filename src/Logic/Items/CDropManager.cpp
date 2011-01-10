#include "CDropManager.h"
#include "CItem.h"
#include "../../Utils/CRand.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../Utils/CXml.h"
#include <algorithm> // dla std::sort && std::lower_bound
#include "../CPhysicalManager.h"
#include "../Factory/CItemTemplate.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Utils/ToxicUtils.h"
#include "../../Utils/StringUtils.h"

template<> CDropManager* CSingleton<CDropManager>::msSingleton = 0;

void CDropManager::Clear()
{
    dropItemTemplates.clear();
}

bool CDropManager::LoadItems(const std::string &filename)
{
    if (this->dropItemTemplates.size() > 0)
        return true;

    CXml xml(filename, "root");
    
    if (xml.GetString(0,"type") != "droplist")
        return false;

    fprintf ( stderr, "CDropManage loading items data from %s ...", filename.c_str() );

    size_t counter = 0;
    CItemTemplate *tmp = NULL;
    std::string file;
    xml_node<>* node;
    for (node = xml.GetChild(0,"item"); node; node = xml.GetSibl(node, "item"))
    {
        ++counter;

        file = xml.GetString(node, "filename");
        tmp = dynamic_cast<CItemTemplate *> ( gResourceManager.GetPhysicalTemplate( file ) );
        
        if (tmp != NULL) {
            dropItemTemplates.push_back(tmp);
        }
        else {
            fprintf (stderr, "CDropManager::LoadItems - can't locate item file (%s)\n", file.c_str());
        }
    }

    /// sortowanie wzglêdem poziomu przedmiotów - u³atwi to losowanie :>
    std::sort(dropItemTemplates.begin(), dropItemTemplates.end());

    fprintf( stderr, "Loading items data finished, loaded %d items (skipped %d items)\n", (int)dropItemTemplates.size(), (int)(counter - dropItemTemplates.size()) );

    return true;
}

CItem * DropCrimsonModeItem(const sf::Vector2f & pos, unsigned maxLvlItem, const std::vector<CItemTemplate *> & itemTemplates) {
    // krotko i brutalnie:

    if (itemTemplates.size() == 0)
        return NULL;

    if (gRand.Rndf(0, 1) > 0.06f)
        return NULL;

    CItem * tmp = itemTemplates.back()->Create();
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

    SEffectParamSet eps = gGraphicalEffects.Prepare("item-circle");
    gGraphicalEffects.ShowEffect(eps,tmp);
    return tmp;
}

//--------------------
CItem *CDropManager::DropItemAt(const sf::Vector2f & pos, unsigned maxLvlItem)
{
    if (ToxicUtils::isGameInCrimsonMode) {
        return DropCrimsonModeItem(pos, maxLvlItem, dropItemTemplates);
    }

    static float dropMod(0.f);

    if (dropItemTemplates.size() == 0)
        return NULL;

    //// ograniczenie itemow do okreslonego zakresu poziomow

    // Liosan, 18.05.09
    // Wiesz co, rAum... nie wiem jak u Ciebie, ale u mnie siê to nie kompiluje.
    // Co gorsza nie wiem co zrobic, zeby dzialalo
    // Wydaje mi sie, ze pisanie nowego zaprzyjaznionego operatora tylko po to, by miec lower_bound
    // to jest przegiêcie. Wywalam - naprawde jestem w stanie napisac ten algorytm sam :P

   /* std::vector<CItemTemplate *>::iterator it = 
        std::lower_bound( dropItemTemplates.begin(), dropItemTemplates.end(), CItemTemplate(maxLvlItem+1) );
    int choice = gRand.Rnd(0, static_cast<unsigned>( it - dropItemTemplates.begin() ) ); */
    unsigned int max = 0;
    CItemTemplate refItemTempl(maxLvlItem+1);
    for (unsigned int i = 0; i < dropItemTemplates.size(); i++)
        if (dropItemTemplates[i] < refItemTempl)
            max = i;
        else
            break;
    int choice = gRand.Rnd(0, max);
    CItemTemplate *t = dropItemTemplates[choice];

    // algorytm losowy + zwiêkszanie prawdopodobieñstwa ¿e graczowi coœ jednak wypadnie ;)
    if ( (t->item.dropRate + dropMod) > gRand.Rndf(0.f,1.1f) ) {
        dropMod = 0.f;
        CItem *tmp = t->Create();
        tmp->SetPosition(pos);
        SEffectParamSet eps = gGraphicalEffects.Prepare("item-circle");
        gGraphicalEffects.ShowEffect(eps,tmp);
        fprintf (stderr, "Item drop - %ls\n", tmp->GetItemName().c_str() );
        return tmp;
    }
    else // zwiêkszenie prawdopodobieñstwa wypadniêcia jakiegoœ przedmiotu (dobraæ eksperymentalnie)
        dropMod += 0.002f;

    return NULL;
}
