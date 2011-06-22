#include "CPhysicalTemplate.h"
#include "CTemplateParam.h"
#include "../../ResourceManager/CResourceManager.h"

#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"

#include "../Effects/CEffectHandle.h"
#include "../Effects/CEffectManager.h"

using namespace rapidxml;

bool CPhysicalTemplate::Load(std::map<std::string,std::string>& argv){
    CXml xml( argv["name"], "root" );
    return Load(xml);
}

bool CPhysicalTemplate::Load(CXml &xml)
{
    mFilename = xml.GetFilename();
    // tylko w pelni konkretne podklasy sprawdzaja, czy ten xml jest ok
    // my jestesmy ufni :)

	mMinScale = xml.GetFloat( "scale", "min" );
	mMaxScale = xml.GetFloat( "scale", "max" );

	mMinRadius = xml.GetFloat( "radius", "min" );
	mMaxRadius = xml.GetFloat( "radius", "max" );


    mMinShadowOffset = xml.GetFloat( "shadow-offset", "min", 0.05f);
    mMaxShadowOffset = xml.GetFloat( "shadow-offset", "max", 0.05f);

	if ( xml_node<>* genre = xml.GetChild( NULL, "genre" ) )
		mGenre = StringUtils::ReinterpretFromUTF8( xml.GetString( genre ) );

    // prefetching - czasem trzeba zaladowac inny szablon, zanim zaladujemy nasz
    // uzasadnienie - umiejki "summon" korzystaja z szablonu przywolywanego potwora
    //
    // EDIT: w zasadzie, to udalo sie to inaczej zalatwic... ale kod tu zostawie
    /*for ( rapidxml::xml_node<> *node=xml.GetChild(0,"prefetch"); node; node=xml.GetSibl(node,"prefetch") ){
        gResourceManager.LoadPhysicalTemplate(xml.GetString(node,"value"));
    }*/

    return true;
}

CTemplateParam *CPhysicalTemplate::ReadParam(CXml &xml, rapidxml::xml_node<> *node, CTemplateParam *p){
    if (p == NULL)
        p = new CTemplateParam();
	rapidxml::xml_node<> *n = xml.GetChild(node,"perm-effect");
    if (n != NULL){
        CEffectHandle *effect = gEffectManager.LoadEffect(xml,n);
        p->values["perm-effect"]=effect;
    }
    return p;
}

void CPhysicalTemplate::Parametrise(CPhysical *obj, CTemplateParam *param){
	if (param != NULL){
		if (param->values["perm-effect"] > 0){
            CEffectHandle * e = (CEffectHandle*)param->values["perm-effect"];
			e->ApplyPermanent(obj);
		}
	}
}

void CPhysicalTemplate::SerializeParam(std::ostream &out, CTemplateParam *param, int indent){
    if (param != NULL){
        if (param->values["perm-effect"] > 0){
            CEffectHandle * e = (CEffectHandle*)param->values["perm-effect"];
            e->Serialize(out, indent);
        }
    }
}
