#include "CLootTemplate.h"
#include "CTemplateParam.h"
#include "../CPhysicalManager.h"
#include "../Loots/CLoot.h"
#include "../../Utils/CXml.h"
#include "../Effects/CEffectManager.h"
#include "../../Rendering/CDisplayable.h"
#include "../../GUI/Localization/CLocalizator.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"

std::string CLootTemplate::GetType()
{
    return "CLootTemplate";
}

bool CLootTemplate::Load(CXml &xml)
{
    if (!CPhysicalTemplate::Load(xml)) return false;
    if ( xml.GetString(0,"type") != "loot" )
		return false;

    mLoot.name = gLocalizator.Localize(xml.GetString("name", "name"));
    mLoot.dropRate = xml.GetFloat("droprate", "value");
    mLoot.lootLvl = xml.GetInt("level", "value");
    mLoot.image = xml.GetString("image", "filename");

    xml_node<>* node = xml.GetChild(0,"effect");
    if (node){
        mLoot.effect = gEffectManager.LoadEffect(xml,node);
    } else {
        mLoot.effect = NULL;
        fprintf ( stderr, "Item %ls doesn't have any effect\n", mLoot.name.c_str() );
    }

    return true;
}

void CLootTemplate::Drop()
{
    delete this;
}

void CLootTemplate::PrepareEditorPreviewer(CDisplayable* d)
{
	d->SetStaticImage( mLoot.image );
}

CLoot* CLootTemplate::Create(std::wstring id)
{
    CLoot *loot = gPhysicalManager.CreateLoot(id);
	loot->SetTemplate( this );
    loot->SetImage ( mLoot.image );
    loot->SetBoundingCircle(0.125f);
    loot->GetDisplayable()->SetScale(0.25f,0.25f);
    loot->BindTemplate(&mLoot);
    loot->SetGenre(mGenre);
    loot->SetAbility(mLoot.ability);

    gGraphicalEffects.ShowEffect("loot-circle-perpetual", loot); //todo: przeniesc do xml'a, zeby rozne
                                                                 //przedmioty mogly miec rozne effekty przypiete
    return loot;
}

CTemplateParam * CLootTemplate::ReadParam(CXml & xml, rapidxml::xml_node<> * node, CTemplateParam * orig) {
    if (orig == NULL) orig = new CTemplateParam();
    std::string ability = xml.GetString(node, "ability");
    if (!ability.empty())
        orig->stringValues["ability"] = ability;
    int level = xml.GetInt(node, "level");
    if (level != 0) {
        int * levelPtr = new int;
        *levelPtr = level;
        orig->values["level"] = levelPtr;
    }

    return CPhysicalTemplate::ReadParam(xml,node,orig);
}

void CLootTemplate::Parametrise(CPhysical * phys, CTemplateParam * param) {
    CLoot * loot = (CLoot*)phys;
    if (param->stringValues.count("ability") > 0) {
        loot->SetAbility(param->stringValues["ability"]);
    }
    if (param->values.count("level") > 0) {
        int * levelPtr = (int*) param->values["level"];
        loot->SetLevel(*levelPtr);
        delete levelPtr;
    }
    CPhysicalTemplate::Parametrise(loot, param);
}

bool operator<(const SLootTemplate& a, const SLootTemplate& b) { return a.lootLvl < b.lootLvl; }    
bool operator>(const SLootTemplate& a, const SLootTemplate& b) { return a.lootLvl > b.lootLvl; }    
bool operator==(const SLootTemplate& a, const SLootTemplate& b) { return a.lootLvl == b.lootLvl; }

bool operator<(const CLootTemplate* a, const CLootTemplate &b) { return a->mLoot < b.mLoot; }
bool operator>(const CLootTemplate* a, const CLootTemplate &b) { return a->mLoot > b.mLoot; }   
bool operator==(const CLootTemplate* a, const CLootTemplate &b) { return a->mLoot == b.mLoot; }

