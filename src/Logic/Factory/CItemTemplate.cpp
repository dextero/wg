#include "CItemTemplate.h"
#include "../CPhysicalManager.h"
#include "../Items/CItem.h"
#include "../../Utils/CXml.h"
#include "../Effects/CEffectManager.h"
#include "../../Rendering/CDisplayable.h"
#include "../../GUI/Localization/CLocalizator.h"

std::string CItemTemplate::GetType()
{
    return "CItemTemplate";
}

bool CItemTemplate::Load(CXml &xml)
{
    if (!CPhysicalTemplate::Load(xml)) return false;
    if ( xml.GetString(0,"type") != "item" )
		return false;

    item.name = gLocalizator.Localize(xml.GetString("name", "name"));
    item.dropRate = xml.GetFloat("droprate", "value");
    item.itemLvl = xml.GetInt("level", "value");
    item.image = xml.GetString("image", "filename");

    xml_node<>* node = xml.GetChild(0,"effect");
    if (node){
        item.effect = gEffectManager.LoadEffect(xml,node);
    } else {
        item.effect = NULL;
        fprintf ( stderr, "Item %ls doesn't have any effect\n", item.name.c_str() );
    }

    return true;
}

void CItemTemplate::Drop()
{
    delete this;
}

void CItemTemplate::PrepareEditorPreviewer(CDisplayable* d)
{
	d->SetStaticImage( item.image );
}

CItem* CItemTemplate::Create(std::wstring id)
{
    CItem *item = gPhysicalManager.CreateItem(id);
	item->SetTemplate( this );
    item->SetImage ( this->item.image );
    item->SetBoundingCircle(0.125f);
    item->GetDisplayable()->SetScale(0.25f,0.25f);
    item->BindTemplate(&this->item);
    item->SetGenre(mGenre);
    return item;
}

bool operator<(const SItemTemplate& a, const SItemTemplate& b) { return a.itemLvl < b.itemLvl; }    
bool operator>(const SItemTemplate& a, const SItemTemplate& b) { return a.itemLvl > b.itemLvl; }    
bool operator==(const SItemTemplate& a, const SItemTemplate& b) { return a.itemLvl == b.itemLvl; }

bool operator<(const CItemTemplate* a, const CItemTemplate &b) { return a->item < b.item; }
bool operator>(const CItemTemplate* a, const CItemTemplate &b) { return a->item > b.item; }   
bool operator==(const CItemTemplate* a, const CItemTemplate &b) { return a->item == b.item; }

