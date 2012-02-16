
#include "CAbility.h"
#include "../CBullet.h"
#include "../CActor.h"
#include "../../Utils/Directions.h"
#include "../Effects/CEffectManager.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../Utils/Maths.h"
#include "../Effects/CEffectHandle.h"
#include "../Factory/CBulletTemplate.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "CBulletEmitter.h"
#include "../CDetector.h"
#include "../Effects/CComputedValueFactory.h"
#include "../AI/CAIAbilityHint.h"
#include "../../GUI/Localization/CLocalizator.h"

#ifndef INT_MAX
#   define INT_MAX  2147483647
#endif

CAbility::CAbility() :
    name(L""),
    description(L""),
    icon(""),
    unavailableIcon(""),
    cooldown(0.0f),
    delay(0.0f),
    abilityType(atNone),
    effect(0),
    selfEffect(0),
    mCastingEffect(NULL),
    mDelayEffect(NULL),
    mBulletEmitter(NULL),
    range(0.0f),
    angularWidth(0.0f),
    isFocus(false),
    focusManaCost(0.0f),
    focusMaxDuration(0.0f),
    mAIHint(NULL),
    mMaxLevel((unsigned)-1),
    mCanBuy(true)
{
    ;
}

bool CAbility::Load(std::map<std::string,std::string>& argv)
{
    CXml		xml( argv["name"], "root" );
	xml_node<> *node;

	if ( xml.GetString(0,"type") != "ability" )
		return false;

    name = gLocalizator.Localize(xml.GetString("name"));

    icon = xml.GetString("icon");
    unavailableIcon = xml.GetString("unavailable-icon");

    description = gLocalizator.Localize(xml.GetString("description"));

    node = xml.GetChild(xml.GetRootNode(), "stats");
    effectDescription = xml.GetString(node, "desc");
    for (node = xml.GetChild(node, "param"); node; node = xml.GetSibl(node, "param"))
        effectDescriptionParameters.insert(std::make_pair(xml.GetString(node, "name"), xml.GetString(node, "calc")));

	node = xml.GetChild(xml.GetRootNode(),"manacost");
	if (node)
		mManaCost = CComputedValueFactory::Parse(xml,node);
	node = xml.GetChild(xml.GetRootNode(),"delay");
	if (node)
		delay = CComputedValueFactory::Parse(xml,node);
	node = xml.GetChild(xml.GetRootNode(),"cooldown");
	if (node)
		cooldown = CComputedValueFactory::Parse(xml,node);
	node = xml.GetChild(xml.GetRootNode(),"casttime");
	if (node)
		casttime = CComputedValueFactory::Parse(xml,node);

    node = xml.GetChild(0,"effect");
    if (node){
        effect = gEffectManager.LoadEffect(xml,node);
    } else
        effect = NULL;

    node = xml.GetChild(0,"self-effect");
    if (node){
        selfEffect = gEffectManager.LoadEffect(xml,node);
    } else
        selfEffect = NULL;
    
    node = xml.GetChild(0,"cast-time-effect");
    if (node){
        mCastingEffect = gEffectManager.LoadEffect(xml,node);
    } else
        mCastingEffect = NULL;
    
    node = xml.GetChild(0,"delay-effect");
    if (node){
        mDelayEffect = gEffectManager.LoadEffect(xml,node);
    } else
        mDelayEffect = NULL;
    
    node = xml.GetChild(0,"focus-mana-cost");
    if (node){
        isFocus = true;
        focusManaCost = CComputedValueFactory::Parse(xml,node);
        node = xml.GetChild(0,"focus-max-duration");
        if (node)
            focusMaxDuration = CComputedValueFactory::Parse(xml,node);
    } else
        isFocus = false;

    node = xml.GetChild(0,"range");
    range = CComputedValueFactory::Parse(xml,node);
    node = xml.GetChild(0,"ai-hint");
    if (node){
        mAIHint = new CAIAbilityHint(this);
        mAIHint->Load(xml,node);
    }

    mMaxLevel = xml.GetInt(xml.GetChild(xml.GetRootNode(), "max-level"), "value", INT_MAX);
    mCanBuy = (xml.GetChild(xml.GetRootNode(), "no-buy") == 0);
    if (xml.GetFilename().find("firebolt") != std::string::npos)
        fprintf(stderr, "");
    mPower = CComputedValueFactory::Parse(xml, xml.GetChild(0, "power"));

    std::string abiClass = xml.GetString(0,"class");
    if (abiClass == "melee"){
        abilityType = atMelee;
        angularWidth = xml.GetFloat("width","value");
        trigger = xml.GetString("trigger");
        return true;
    } else if (abiClass == "self"){
        abilityType = atSelf;
        trigger = xml.GetString("trigger");
        return true;
    } else if (abiClass == "bullet"){
        abilityType = atBullet;
        trigger = xml.GetString("trigger");
        mBulletEmitter = new CBulletEmitter();
        mBulletEmitter->Load(xml);
        mBulletEmitter->mEffect = effect;
        return true;
    } else if (abiClass == "passive"){
        abilityType = atPassive;
        trigger = "";
        return true;
	} else if (abiClass == "export"){
        abilityType = atExport;
		trigger = "";
		return true;
	}

	return false;
}

void CAbility::Perform(CActor *actor,ExecutionContextPtr context, FocusObjectPtr focus){
    if (actor == NULL) return;
    switch(abilityType){
        case atBullet:
			if (selfEffect)
				gEffectManager.ApplyOnce(actor, selfEffect->GetOffset(), (new CEffectSource(this))->SetSecondary(actor),context,focus);
            mBulletEmitter->Shoot(actor,context,(new CEffectSource(this))->SetSecondary(actor));
            break;
        case atMelee:
			if (selfEffect)
				gEffectManager.ApplyOnce(actor, selfEffect->GetOffset(), (new CEffectSource(this))->SetSecondary(actor),context,focus);
            PerformMelee(actor,context);
            break;
        case atSelf:
            gEffectManager.ApplyOnce(actor, effect->GetOffset(), (new CEffectSource(this))->SetSecondary(actor),context,focus);
            break;
        default:
            break;
    }
}

void CAbility::PerformMelee(CActor *actor,ExecutionContextPtr context, FocusObjectPtr focus){
    if (effect == NULL)
        return;
    CDetector *detector = gPhysicalManager.CreateDetector();
    detector->SetPosition( actor->GetPosition() );
	detector->SetBoundingCircle( range.Evaluate(context) );
    int victimCategory = PHYSICAL_HOSTILES;
    if (actor->GetCategory() == PHYSICAL_MONSTER)
        victimCategory = PHYSICAL_PLAYER;
	detector->SetCollidingPhysicals( victimCategory );
	detector->SetBehaviour( CHECK_MELEE_RANGE | APPLY_EFFECT );
	detector->SetMeleeAngle(angularWidth);
    detector->SetRotation(actor->GetRotation());
    detector->SetEffectOffset( effect->GetOffset() );
    detector->SetFramesToDeath( 1 );
    detector->SetExecutionContext(context);
    detector->SetSource((new CEffectSource(this))->SetSecondary(actor));
}

std::string CAbility::GetType()
{
	return "CAbility";
}

void CAbility::Drop()
{
    if (mBulletEmitter)
        delete mBulletEmitter;
	delete this;
}

bool CAbility::InMeleeRange(CPhysical *attacker, CPhysical *victim, ExecutionContextPtr context){
    float d = Maths::LengthSQ(attacker->GetPosition() - victim->GetPosition());
	float r = range.Evaluate(context);
	float rq = r * r;
    if (d > rq + victim->GetCircleRadius())
        return false;
    return Maths::CircleInAngle(attacker->GetPosition(),RotationToVector((float)attacker->GetRotation()),
        angularWidth,victim->GetPosition(),victim->GetCircleRadius());
}

const std::string CAbility::GetEffectDescription(CActor* performer)
{
    std::string out = effectDescription;

    int abiIndex = performer->GetAbilityPerformer().FindAbilityIndex(this);

    if (abiIndex == -1)
    {
        SAbilityInstance abiInst(this);
        abiIndex = performer->GetAbilityPerformer().AddAbility(abiInst);
    }

    ExecutionContextPtr context = performer->GetAbilityPerformer().GetContext(abiIndex);
    context->abilityPower = mPower.Evaluate(context);

    for (std::map<std::string, std::string>::iterator it = effectDescriptionParameters.begin(); it != effectDescriptionParameters.end(); ++it)
    {
        CComputedValue val(it->second);

        std::string computed = StringUtils::ToString(val.Evaluate(context));
        out = StringUtils::ReplaceAllOccurrences(out, "$" + it->first, computed);

    }

    return out;
}

const std::string CAbility::GetManaCostString(CActor* performer)
{
    int abiIndex = performer->GetAbilityPerformer().FindAbilityIndex(this);

    if (abiIndex == -1)
    {
        SAbilityInstance abiInst(this);
        abiIndex = performer->GetAbilityPerformer().AddAbility(abiInst);
    }

    ExecutionContextPtr context = performer->GetAbilityPerformer().GetContext(abiIndex);
    context->abilityPower = mPower.Evaluate(context);

    std::string ret = StringUtils::ToString(mManaCost.Evaluate(context));

    if (isFocus)
        ret += " + " + StringUtils::ToString(focusManaCost.Evaluate(context));

    return ret;
}
