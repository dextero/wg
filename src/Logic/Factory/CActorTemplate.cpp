#include "CActorTemplate.h"
#include "CTemplateParam.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Rendering/CDisplayable.h"
#include "../AI/CAISchemeManager.h"
#include "../AI/CActorAI.h"
#include "../../Rendering/Animations/CAnimSet.h"
#include "../../Utils/CXml.h"
#include "../CPhysicalManager.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../ResourceManager/CSound.h"
#include "../Conditions/CCondition.h"
#include "../Abilities/SAbilityInstance.h"
#include "../CLogic.h"
#include "../../GUI/Localization/CLocalizator.h"

using namespace rapidxml;
using namespace StringUtils;
using namespace Maths;

std::string CActorTemplate::GetType()
{
	return "CActorTemplate";
}

bool CActorTemplate::Load(CXml &xml)
{
    if (!CPhysicalTemplate::Load(xml)) return false;
	xml_node<>*	node;
    mMinStats = new CStats(NULL);
    mMinStats->Load(xml,"min");

    mMaxStats = new CStats(NULL);
    mMaxStats->Load(xml,"max");
	
    mMinXPValue = xml.GetFloat( "xp", "min" );
	mMaxXPValue = xml.GetFloat( "xp", "max" );

// tox, 20 cze: a czemu skala i radius nie sa ladowane jakos poprzez CPhysicalTemplate::Load ?
	mMinScale = xml.GetFloat( "scale", "min" );
	mMaxScale = xml.GetFloat( "scale", "max" );

	mMinRadius = xml.GetFloat( "radius", "min" );
	mMaxRadius = xml.GetFloat( "radius", "max" );

    mColorRed = xml.GetFloat( "color", "r" , 1.0 );
    mColorGreen = xml.GetFloat( "color", "g", 1.0 );
    mColorBlue = xml.GetFloat( "color", "b", 1.0 );
    mColorAlpha = xml.GetFloat( "color", "a", 1.0 );

    if ((node = xml.GetChild(xml.GetRootNode(),"animset"))){
        for ( node=xml.GetChild(0,"animset"); node; node=xml.GetSibl(node,"animset") ){
            CAnimSet * animations = new CAnimSet();
            animations->Parse(xml,node);
            mAvailableAnimations.push_back(animations);
        }
    } else {
        CAnimSet * animations = new CAnimSet();
        animations->Parse(xml);
        mAvailableAnimations.push_back(animations);
    }
    if (mAvailableAnimations.size() > 0)
        mAnimations = mAvailableAnimations[0];
    else
        mAnimations = NULL;

    std::string tmp;
    for ( node=xml.GetChild(0,"ability"); node; node=xml.GetSibl(node,"ability") ){
        mAbilities.push_back(gResourceManager.GetAbility (xml.GetString(node,"name")));
        tmp = xml.GetString(node,"anim");
        mAbilityAnims.push_back(CAnimSet::ParseAnimClass(tmp));
        if (xml.GetInt(node,"default"))
            mDefaultAbility = (int)mAbilities.size()-1;
    }

	for ( node=xml.GetChild(0,"sound"); node; node=xml.GetSibl(node,"sound") )
			mSound[xml.GetString(node, "type")].push_back( gResourceManager.GetSound( xml.GetString(node,"filename") ) );

	return true;
}

void CActorTemplate::Drop()
{
	delete this;
}

void CActorTemplate::PrepareEditorPreviewer( CDisplayable* d )
{
	d->SetAnimation( mAnimations->GetDefaultAnim() );
}

void CActorTemplate::Fill(CActor *actor, float f)
{
    actor->SetTemplate(this);

    float xp =          Lerp( mMinXPValue,   mMaxXPValue, f);
	float radius =	    Lerp( mMinRadius,	 mMaxRadius,		f );
	float scale =	    Lerp( mMinScale,	 mMaxScale,		f );
    float shadow =      Lerp( mMinShadowOffset, mMaxShadowOffset, f);

    CStats minStats = *mMinStats, maxStats = *mMaxStats;
    
    // arcade mode, tutaj, bo to tylko tymczasowe
    // reszta zmian do arcade-mode: CMenuScreens::UpdateArcadeMode
    if (gLogic.IsInArcadeMode())
    {
        // mana * 2
        minStats.SetBaseAspect(aMaxMana, minStats.GetBaseAspect(aMaxMana) * 2.f);
        maxStats.SetBaseAspect(aMaxMana, maxStats.GetBaseAspect(aMaxMana) * 2.f);

        // mana-regen / 2
        minStats.SetBaseAspect(aManaRegen, minStats.GetBaseAspect(aManaRegen) * 0.5f);
        maxStats.SetBaseAspect(aManaRegen, maxStats.GetBaseAspect(aManaRegen) * 0.5f);
    }

    actor->GetStats()->LerpContent(&minStats,&maxStats,f);
    actor->GetStats()->SetMana(actor->GetStats()->GetBaseAspect(aMaxMana)); // mana na max
	actor->SetBoundingCircle(radius);
    actor->SetXPValue(xp);
    actor->SetShadowOffset(shadow);
	actor->SetGenre( mGenre );

    if (mAnimations){
        actor->SetAnimSet(mAnimations);    
        if (mAnimations->GetDefaultAnim()!=NULL)
            actor->SetAnimation(mAnimations->GetDefaultAnim());
    }

    actor->SetColor( mColorRed, mColorGreen, mColorBlue, mColorAlpha );

	if ( actor->GetDisplayable() )
    {
		actor->GetDisplayable()->SetScale( scale, scale );
    }
	if ( actor->GetShadow() )
    {
		actor->GetShadow()->SetScale( scale, scale );
    }

    actor->GetAbilityPerformer().Bind(&mAbilities,&mAbilityAnims);
    actor->GetAbilityPerformer().SetDefaultAttack(mDefaultAbility);

    // TODO! inne dzwieki
    for (std::map< std::string, std::vector<System::Resource::CSound*> >::iterator i = mSound.begin(); i != mSound.end(); ++i)
	    for (unsigned j = 0; j < i->second.size(); j++)
		    actor->AddSound(i->first, i->second[j]);
}

CTemplateParam *CActorTemplate::ReadParam(CXml &xml, rapidxml::xml_node<> *node, CTemplateParam *orig){
    CTemplateParam *p;
    if (orig == NULL)
        p = new CTemplateParam();
    else
        p = orig;
    rapidxml::xml_node<> *n = xml.GetChild(node,"cond");
    if (n != NULL){
        CCondition *cond = new CCondition();
        cond->Load(xml,n);
        p->values["cond"]=cond;
    }
    return CPhysicalTemplate::ReadParam(xml,node,p);
}

void CActorTemplate::Parametrise(CPhysical *phys, CTemplateParam *param){
    CActor *actor = (CActor*)phys;
    if (param->values.count("cond") > 0){
        CCondition *cond = (CCondition*)param->values["cond"];
        actor->SetCondition(cond);
    }
    CPhysicalTemplate::Parametrise(phys,param);
}

void CActorTemplate::SerializeParam(std::ostream &out, CTemplateParam *param){
    if (param != NULL){
        if (param->values.count("cond") > 0){
            CCondition *cond = (CCondition*)param->values["cond"];
            cond->Serialize(out);
        }
    }
    CPhysicalTemplate::SerializeParam(out,param);
}

const std::wstring CActorTemplate::GetStatsWString()
{
    std::wstringstream ss;
    size_t nameId = 5;  // ktory w kolejnosci jest aStrength
    for (EAspect a = aStrength; nameId < (size_t)Aspects::aspectCount; a = (EAspect)(((unsigned)a) << 1), ++nameId)
    {
        if (a == aPhysResist)
            ss << L"\n" << gLocalizator.GetText("ASPECT_RESISTS").c_str() << L":\n";
        if (a >= aPhysResist)
            ss << L" - ";

        ss << gLocalizator.GetText(("ASPECT_" + StringUtils::ToUpper(StringUtils::ConvertToString(Aspects::aspectNames[nameId]))).c_str()) << L":  ";
        if (mMinStats->GetBaseAspect(a) == mMaxStats->GetBaseAspect(a))
            ss << mMinStats->GetBaseAspect(a);
        else
            ss << mMinStats->GetBaseAspect(a) << " - " << mMaxStats->GetBaseAspect(a);
        ss << L"\n";
    }

    return ss.str();
}
