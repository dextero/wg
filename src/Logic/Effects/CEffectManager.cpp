#include "CEffectManager.h"

#include "CEffectHandle.h"
#include "CAppliedEffect.h"
#include "../CPhysicalManager.h"
#include "../CPhysical.h"
#include "../CBullet.h"
#include "../CActor.h"
#include "../CDetector.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/Directions.h"
#include "../../Rendering/Primitives/CPrimitivesDrawer.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"
#include "../../Rendering/Effects/EffectTypes.h"
#include "../../Commands/CCommands.h"
#include "../../ResourceManager/CSound.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../Audio/CAudioManager.h"
#include "../../Map/CMapManager.h"
#include "../Abilities/CBulletEmitter.h"
#include "../Abilities/CTargetChooser.h"
#include "../Abilities/CSummoner.h"
#include "../Abilities/CPinnedAbilityBatch.h"
#include "../Abilities/CAbility.h"
#include "../Stats/CStats.h"
#include "CAppliedEffectContainer.h"
#include "CExprParser.h"
#include "CComputedValue.h"
#include "CComputedValueFactory.h"
#include <cstdio>
#include <map>
#include <cassert>

template<> CEffectManager* CSingleton< CEffectManager >::msSingleton = 0;

using namespace EffectTypes;

CEffectManager::CEffectManager(){
    fprintf(stderr,"CEffectManager::CEffectManager()\n");
    EffectTypes::BuildData();
	effectNodes.reserve(2000);
}

CEffectManager::~CEffectManager(){
    fprintf(stderr,"CEffectManager::~CEffectManager()\n");
}

float CEffectManager::DefaultRecast(EffectType et){
    switch(et){
        case betModify:
        case betAIState:
        case betStun:
		case betHealthcast:
        case etGraphicalEffect:
		case betSound:
            return std::numeric_limits<float>::infinity();
        case betDamage:
        case betHeal:
        case betRestoreMana:
        case betDrainMana:
        case betTransferLife:
        case betTransferMana:
        case betHealTarget:
            return 0.0f;
        default:
            return 1.0f;
    }
}

bool CEffectManager::RequiresDuration(EffectType et){
    switch(et){
        case betModify:
        case betAIState:
        case etTimedEffect:
        case etRecastableEffect:
        case etDelayedEffect:
        case betStun:
        case etGraphicalEffect:
            return true;
        default:
            return false;
    }
}

int CEffectManager::Compile(int offset){
    //fprintf(stderr,"compiling %ls at offset %d...\n",effectName(effectNodes[offset].effectType).c_str(),offset);
    int count, start;
    EffectType et = effectNodes[offset].effectType;
    switch(et){
        // lista - iteracyjno-rekurencyjnie
        case etEffectList:
            start = offset;
            offset++;
            count = effectNodes[offset].iParam;
            offset++;
            for (int i = 0; i <count; i++){
                Compile(offset);
                offset = effectNodes[offset].endOffset+1;
            }
            return (effectNodes[start].endOffset = offset-1);
        // konsola - specyficzne traktowanie ze wzgledu na licznik linii
            // (ktory nie jest parametrem, ale laduje w effectNode'ach)
        case betConsole:
            return (effectNodes[offset].endOffset = offset + 2);
			// dzwiek - licznik na poczatku, wiele name'ow
		case betSound:
			return (effectNodes[offset].endOffset = offset + 1 + effectNodes[offset+1].iParam);
        default:
            if (hasParam(et,ptSubEffect)){ // meta-efekty - rekurencyjnie
                return (effectNodes[offset].endOffset = Compile(offset+paramOffset(et,ptSubEffect)));
            } else { // bazowe - proste
                return (effectNodes[offset].endOffset = offset + totalStaticOffset(et));
            }
    }
}

void CEffectManager::Apply(CPhysical *physical, int offset, float duration, EffectSourcePtr source, ExecutionContextPtr context,
                           FocusObjectPtr focus)
{
    EffectType et = effectNodes[offset].effectType;
	//fprintf(stderr,"applying effect %ls at offset %d to physical %ls at %p\n",effectName(et).c_str(),offset,(physical ? physical->GetUniqueId().c_str() : L"null"),physical);
    CAppliedEffectPtr ae;
    int count;
    CAppliedEffectContainer *aec = NULL; 
    if (physical != NULL)
        aec = physical->GetAppliedEffectContainer();
    switch(et){
        case etTimedEffect:
        {
			float t = GetParamNode(offset,ptTime).fValue.Evaluate(context);
            Apply(physical,offset+paramOffset(et,ptSubEffect),t,source,context,focus);
            break;
        }
        case etRecastableEffect:
            if (aec == NULL)
                break;
            ae = CAppliedEffect::Alloc();
            ae->source=source;
			ae->reapplyTime=GetParamNode(offset,ptRecast).fValue.Evaluate(context); 
            ae->durationLeft=GetParamNode(offset,ptTime).fValue.Evaluate(context);
            // tox, 29 sierpnia , notka:
            // spytac liosana kiedys co zrobic, jesli chcialoby sie, zeby pierwszy recast nastepowal nie po
            // uplywie duration, tylko od razu na dziendobry... innymi slowy skoro aktualnie mamy skladnie:
            //     wait; cast; wait; cast; ... 
            // to jak to zrobic (i gdzie, zeby bylo wygonie i nie popsulo nic) zeby miec:
            //     cast; wait; cast; wait; ...
            if (ae->durationLeft==0.0f)
                ae->durationLeft = duration;
            ae->offset=offset+paramOffset(et,ptSubEffect);
            ae->context = context;
            ae->focus = focus;
            aec->AddEffect(ae);
            break;
        case etDelayedEffect:
            if (aec == NULL)
                break;
            ae = CAppliedEffect::Alloc();
            ae->source=source;
            ae->durationLeft=GetParamNode(offset,ptTime).fValue.Evaluate(context);
            ae->reapplyTime=GetParamNode(offset,ptTime).fValue.Evaluate(context);
            ae->offset=offset+paramOffset(et,ptSubEffect);
            ae->context = context;
            ae->focus = focus;
            aec->AddEffect(ae);
            break;
        case etEffectList:
            offset++;
            count = effectNodes[offset].iParam;
            offset++;
            for (int i = 0; i <count; i++){
                Apply(physical,offset,duration,source,context,focus);
                offset = effectNodes[offset].endOffset+1;
            }
            break;
        case etChanceEffect:
			if (gRand.Rndf() <= GetParamNode(offset,ptChance).fValue.Evaluate(context)){
                Apply(physical,offset+paramOffset(et,ptSubEffect),std::numeric_limits<float>::quiet_NaN(),source,context,focus);
            }
            break;
        case etNonCumulative:
            if (aec == NULL)
                break;
			// Liosan, 29.08 
			// takie zabezpieczenie - zeby efekt, ktory sie wlasnie wykonuje, nie dokonal autodestrukcji gdy przyjdzie
			// mu do glowy podobna aplikacja do tego samego potwora
			if (aec->RemoveEffects(offset+paramOffset(et,ptSubEffect),effectNodes[offset].endOffset))
				Apply(physical,offset+paramOffset(et,ptSubEffect),std::numeric_limits<float>::quiet_NaN(),source,context,focus);
            break;
        case etCheckLevel:
            if (GetParamNode(offset,ptAmount).fValue.Evaluate(context) <= context->values[aLevel]){
                //fprintf(stderr,"check-level succeeded - level is %f, check against %f \n",context->values[aLevel],effectNodes[offset+paramOffset(et,ptAmount)].fParam);
                Apply(physical,offset+paramOffset(et,ptSubEffect),std::numeric_limits<float>::quiet_NaN(),source,context,focus);
            } else {
                //fprintf(stderr,"check-level failed - level is %f, check against %f \n",context->values[aLevel],effectNodes[offset+paramOffset(et,ptAmount)].fParam);
            }
            break;
        default:
            if ((duration!=duration) && RequiresDuration(et)){
                Apply(physical,offset,1.0f,source,context,focus);
                break;
            }
            ae = CAppliedEffect::Alloc();
            ae->durationLeft=duration;
            ae->source=source;
            ae->offset=offset;
            ae->reapplyTime=DefaultRecast(et);
            ae->context = context;
            ae->focus = focus;
            if (!ae->IsApplied()){ // <-> duration == NaN <-> czy to jednokrotna aplikacja
                Perform(physical,ae,1.0f);
            } else {
                if (aec!=NULL)
                    aec->AddEffect(ae);
                if ((DefaultRecast(et)==std::numeric_limits<float>::infinity()) || (aec == NULL))
                    Perform(physical,ae,1.0f);
            }
            break;
    }
}

void CEffectManager::Perform(CPhysical *physical, CAppliedEffectPtr ae,float dt){
	std::vector<CPhysical*> choosedPhysicals;
    EffectType et = effectNodes[ae->offset].effectType;
    //fprintf(stderr,"performing effect %d, which is a %ls, to physical %ls at %p\n",
	//	ae->offset,effectName(et).c_str(),(physical ? physical->GetUniqueId().c_str() : L"null"),physical);
    float v;
    int tmp;
    sf::Vector2f pos, rot;
    if((physical!=NULL) && (ae->IsApplied()) && (ae->meAsSource == NULL) && (physical->GetAppliedEffectContainer() != NULL)){
        fprintf(stderr, "ERROR: AppliedEffect is attached to actor, but it's meAsSource field is NULL\n");
        fprintf(stderr, "DEBUG data: actor: %p, ae: %p, offset: %d, et: %ls,reapply: %f\n",physical,&(*ae),ae->offset,effectName(et).c_str(),ae->reapplyTime);
        fprintf(stderr, "ABORTING EXECUTION (happily)\n");
        return;
    }
    EffectSourcePtr source;
    if (ae->IsApplied()){
        source = ae->meAsSource;
    }else
        source = ae->source;
    CStats *stats = NULL;
    if (physical != NULL)
        stats = physical->GetStats();
    CActor *actor;
    switch(et){
        case betDamage: 
			v = dt * GetParamNode(ae->offset,ptAmount).fValue.Evaluate(ae->context);
            //fprintf(stderr,"monster %ls has %f hp left and gets %f damage!\n",physical->GetUniqueId().c_str(),physical->GetHP(),v);
            actor = dynamic_cast<CActor*>(physical);
            if (actor != NULL)
                actor->DoDamage(v,GetParamNode(ae->offset,ptDamageType).damageType,source);
            else
                if (stats!=NULL)
                    stats->DoDamage(v);
            break;
        case betHeal:            
            v = dt* GetParamNode(ae->offset,ptAmount).fValue.Evaluate(ae->context); 
            actor = dynamic_cast<CActor*>(physical);
            if (actor != NULL && (GetParamNode(ae->offset,ptShowAmount).iParam != 0))
                actor->DoHeal(v);
			else if (stats != NULL) {
                stats->DoHeal(v);
				//fprintf(stderr,"monster %ls has %f hp left and gets healed for %f per s (which is %f this frame).\n",physical->GetUniqueId().c_str(),physical->GetStats()->GetHP(),v / dt,v);
			}
            break;
        case betDrainMana:
            v = dt* GetParamNode(ae->offset,ptAmount).fValue.Evaluate(ae->context);  
            //fprintf(stderr,"monster %ls has %f mana left and gets drained for %f.\n",physical->GetUniqueId().c_str(),physical->GetMana(),v);
            if (stats != NULL)
                stats->DrainMana(v);
            break;
        case betRestoreMana:
            v = dt* GetParamNode(ae->offset,ptAmount).fValue.Evaluate(ae->context); 
            //fprintf(stderr,"monster %ls gets %f mana restored.\n",physical->GetUniqueId().c_str(),v);
            if (stats != NULL)
                stats->RestoreMana(v);
            break;
        case betModify:
            RecalculateModifier(physical,effectNodes[ae->offset+paramOffset(et,ptAspect)].aspect);
            break;
        case betAIState:
            // TODO
            break;
        case betConsole:{ // specjalne traktowanie - najpierw licznik, potem text
        	std::map<std::wstring, std::wstring> params;
        	params[L"$X"] = StringUtils::ConvertToWString(StringUtils::ToString(physical->GetPosition().x));
        	params[L"$Y"] = StringUtils::ConvertToWString(StringUtils::ToString(physical->GetPosition().y));
        	params[L"$THIS"] = physical->GetUniqueId();
            for (int i = 0; i < effectNodes[ae->offset+1].iParam; i++)
                gCommands.ParseCommand(*(effectNodes[ae->offset+2].wsParam+i),params);
            break;
        }
        case betBullet:
            tmp = ae->offset + paramOffset(et,ptBulletEmitter);
            effectNodes[tmp].bulletEmitter->Shoot(physical,ae->context,source);
            break;
		case betSound:
		{
			int size = effectNodes[ae->offset+1].iParam;
			if (size == 0) break;
			int r = gRand.Rnd(0,size);
			System::Resource::CSound *s = gResourceManager.GetSound(*effectNodes[ae->offset+2+r].sParam);
			if (s!= NULL)
				gAudioManager.GetSoundPlayer().Play(s, physical->GetPosition());
			break;
		}
        case etAreaEffect:
            PerformAt(physical->GetPosition(),ae->offset,source,ae->context);
            break;
		case etRay:
			rot = RotationToVector((float)physical->GetRotation());
			pos = physical->GetPosition() + Maths::VectorLeftOf(rot) * GetParamNode(ae->offset,ptOffsetX).fValue.Evaluate(ae->context) + 
				rot * GetParamNode(ae->offset,ptOffsetY).fValue.Evaluate(ae->context);
			v = GetParamNode(ae->offset,ptArea).fValue.Evaluate(ae->context);
			tmp = effectNodes[ae->offset+paramOffset(etRay,ptFilter)].iParam;
			gScene.GetPhysicalsBetweenPoints(pos, pos + rot * v, 0.1f, tmp, choosedPhysicals);

			for (unsigned i=0; i < choosedPhysicals.size(); i++)
				gEffectManager.ApplyOnce(choosedPhysicals[i], ae->offset+paramOffset(etRay,ptSubEffect),
					(new CEffectSource(estGenericPhysical,physical))->SetSecondary(source),	ae->context );
			break;
        case etGraphicalEffect:
            {
                SEffectParamSet eps = gGraphicalEffects.Prepare( *effectNodes[ae->offset+paramOffset(et,ptName)].sParam );
                eps.duration = GetParamNode(ae->offset,ptTime).fValue.Evaluate(ae->context);
                ae->displayableEffectHandle = gGraphicalEffects.ShowEffect( eps, physical );
				ae->durationLeft = eps.duration;
            }
            break;
        case betStun:
            actor = dynamic_cast<CActor*>(physical);
            if (actor!=NULL)
                actor->SetStun(true);
            break;
        case betIncreaseNextAbilityLevel:
            actor = dynamic_cast<CActor*>(physical);
			if (actor!=NULL){
				float v = GetParamNode(ae->offset,ptAmount).fValue.Evaluate(ae->context);
				//warning C4244: 'argument' : conversion from 'float' to 'int', possible loss of data
                actor->IncreaseNextAbilityLevel((int)v);
			}
            break;
		case betHealthcast:
            actor = dynamic_cast<CActor*>(physical);
            if (actor!=NULL)
                actor->SetHealthcast(true);
            break;
        case betKnockback:
            if ((source != NULL)&&(source->IsValid())&&(physical->GetSideAndCategory().category & PHYSICAL_MOVING)){
                tmp = (int)false; // nie chce mi sie deklarowac nowej zmiennej ;)
                if (!(source->IsValid())){
                    fprintf(stderr,"WARNING: knockback effect at offset %d with an invalid source - unable to execute, aborting\n",ae->offset);
                    break;
                }
                switch(source->primary.type){
                    case estBullet:
                        tmp = (int)true;
                        pos = source->primary.bullet->GetPosition();
                        break;
                    case estExplosion:
                        tmp = (int)true;
                        pos = source->primary.detector->GetPosition();
                        break;
                    case estActor:
                        tmp = (int)true;
                        pos = source->primary.actor->GetPosition();
                        break;
					case estGenericPhysical:
						tmp = (int)true;
						pos = source->primary.physical->GetPosition();
						break;
                    default:
						switch (source->secondary.type){ // Kurwa, zrobilem potrojnie zagniezdzonego switcha.
							case estBullet:
								tmp = (int)true;
								pos = source->secondary.bullet->GetPosition();
								break;
							case estExplosion:
								tmp = (int)true;
								pos = source->secondary.detector->GetPosition();
								break;
							case estActor:
								tmp = (int)true;
								pos = source->secondary.actor->GetPosition();
								break;
							default:
								fprintf(stderr,"WARNING: knockback effect at offset %d with a source, which doesn't define a position - unable to execute; printing damaged source and aborting\n",ae->offset);
								source->PrintToStderr();
								break;
						}
                }
                if (tmp == (int)true){
                    pos = physical->GetPosition() - pos;

                    v = Maths::Length(pos);
            
                    if (v < 1.f) {
                        v = 1.f;
                    }
                    // effectNodes[ae->offset+paramOffset(et,ptAmount)].fParam
                    ((CActor*)physical)->ApplyForce( Maths::Normalize(pos) * 
						GetParamNode(ae->offset,ptAmount).fValue.Evaluate(ae->context) / (v*v) );
                }
            }
            break;
        case etRetarget:
            effectNodes[ae->offset+paramOffset(et,ptTargetChooser)].targetChooser->Perform(physical,ae->offset+paramOffset(et,ptSubEffect),source,ae->context);
            break;
        case betLinkedGraphicalEffect:
            {
                CPhysical *phys = NULL;
                if (source->primary.ContainsPhysical()){
                    phys = source->primary.physical;
                } else if ((source->primary.type != estNextSource) && (source->secondary.ContainsPhysical())){
                    phys = source->secondary.physical;
                }
                if (phys){
                    SEffectParamSet eps = gGraphicalEffects.Prepare( *effectNodes[ae->offset+paramOffset(et,ptName)].sParam);
                    eps.duration = GetParamNode(ae->offset,ptTime).fValue.Evaluate(ae->context); 
		    		eps.offsetX = GetParamNode(ae->offset,ptOffsetX).fValue.Evaluate(ae->context); 
	    			eps.offsetY = GetParamNode(ae->offset,ptOffsetY).fValue.Evaluate(ae->context); 
                    ae->displayableEffectHandle = gGraphicalEffects.ShowEffect( eps, phys, physical);
                } else {
                    fprintf(stderr,"linked graphical effect with damaged source; aborting");
                    source->PrintToStderr();
                }
            }
            break;
        case betJump:
			ApplyOnce(physical,GetParamNode(ae->offset,ptName).iParam,source,ae->context);
			break;
        case betSummon:
            effectNodes[ae->offset+paramOffset(et,ptSummoner)].summoner->Summon(physical,ae->context);
            //fprintf(stderr,"[WARNING] Summoning is unfinished, please do not use it in abilities files!\n");
            break;
		case betInject:
			if ((ae->context != NULL) && (physical != NULL)){
				CPinnedAbilityBatch *pab = ae->context->GetPinnedAbilityBatch();
				if (pab != NULL){
					std::string *tree = GetParamNode(ae->offset,ptTree).sParam;
					std::string *code = GetParamNode(ae->offset,ptCode).sParam;
					std::string *tag = GetParamNode(ae->offset,ptName).sParam;
					if (pab->GetAbilityLevel(*tree,*code) >= 1){
						ExecutionContextPtr newCtx = pab->MorphContext(*tree,*code, ae->context);
						if (mTags.count(*tag) > 0)
							ApplyOnce(physical,mTags[*tag],source,newCtx,ae->focus);
					}
				}
			}
			break;
		case betAnnihilate:
			physical->MarkForDelete();
			break;
        case etEffectList:
        case etDelayedEffect:
        case etTimedEffect:
        case etRecastableEffect:
        case etChanceEffect:
        case etNonCumulative:
        case etCheckLevel:
            Apply(physical,ae->offset,std::numeric_limits<float>::quiet_NaN(),source,ae->context,ae->focus);
            break;
        case betTransferLife: 
			v = dt * GetParamNode(ae->offset,ptAmount).fValue.Evaluate(ae->context);
            actor = dynamic_cast<CActor*>(physical);
            if (actor!=NULL)
            {
                actor->DoDamage(v,DamageTypes::dtVorpal,source);
                CActor* caster = dynamic_cast<CActor*>(source->DeterminePhysicalSource());
                if (caster)
                    caster->DoHeal(v);
            }
            else
                if (stats!=NULL)
                    stats->DoDamage(v);
            break;
        case betTransferMana:  
            v = dt * GetParamNode(ae->offset,ptAmount).fValue.Evaluate(ae->context); 
			actor = dynamic_cast<CActor*>(physical);
            if (actor!=NULL)
            {
                actor->GetStats()->DrainMana(v);
                CActor* caster = dynamic_cast<CActor*>(source->DeterminePhysicalSource());
                if (caster)
                    caster->GetStats()->RestoreMana(v);
            }
            else
                if (stats!=NULL)
                    stats->DrainMana(v);
            break;
        case betHealTarget: 
			v = dt * GetParamNode(ae->offset,ptAmount).fValue.Evaluate(ae->context);
            actor = dynamic_cast<CActor*>(physical);
            if (actor!=NULL)
            {
                actor->DoHeal(v);
            }
            else
                if (stats!=NULL)
                    stats->DoHeal(v);
            break;
        case etNoEffect:
    	    break;
        default:
            fprintf(stderr,"WARNING: monster %ls - performing unknown effect \"%d\" at offset %d loaded from file %s\n",physical->GetUniqueId().c_str(),et,ae->offset, effectNodes[ae->offset].debugInfo.c_str());
            break;
    }
}


void CEffectManager::Unapply(CPhysical *physical, CAppliedEffectPtr ae){
    switch(effectNodes[ae->offset].effectType){
        case betModify:
            RecalculateModifier(physical,effectNodes[ae->offset+paramOffset(betModify,ptAspect)].aspect);
            break;
        case betStun:
            ((CActor*)physical)->SetStun(false);
            break;
		case betHealthcast:
			((CActor*)physical)->SetHealthcast(false);
			break;
        default:
            break;
    }
    if (ae->displayableEffectHandle != NULL){
        gGraphicalEffects.RemoveEffect(ae->displayableEffectHandle);
    }
}

void CEffectManager::RecalculateModifier(CPhysical *physical, EAspect aspect){
    if (physical == NULL) return;
    CAppliedEffectContainer *aec = physical->GetAppliedEffectContainer();
    CStats *stats = physical->GetStats();
    if ((aec == NULL) || (stats == NULL)) return;
    float multiplier = 1.0f;
    std::vector<CAppliedEffectContainer::SAppliedEffectData>::iterator it;
    std::vector<CAppliedEffectContainer::SAppliedEffectData> &effects = aec->appliedEffects;
    for (it = effects.begin(); it != effects.end(); it++)
        if (effectNodes[(*it).effect->offset].effectType == betModify)
            if (effectNodes[(*it).effect->offset+paramOffset(betModify,ptAspect)].aspect == aspect){
				multiplier *= GetParamNode((*it).effect->offset,ptAmount).fValue.Evaluate((*it).effect->context);
            }
    stats->SetCurrAspect(aspect,multiplier*physical->GetStats()->GetBaseAspect(aspect));
	//fprintf(stderr,"monster's %ls aspect %ls has been modified by %f\n",physical->GetUniqueId().c_str(),
	//	aspectName(aspect).c_str(),multiplier);
}

void CEffectManager::PerformAt(const sf::Vector2f &pos, int offset, EffectSourcePtr source, ExecutionContextPtr context, 
                               FocusObjectPtr focus){
    // dotyczy tylko AreaEffect, GraphicalEffect i EffectList; potem moze jakichs innych
    //fprintf(stderr,"PerformAt with offset %d at position %f,%f\n",offset,pos.x,pos.y);
    int count,victimCategory = PHYSICAL_HOSTILES;
    float r;
	CDetector * detector;

	//CPrimitiveClass *pc = gPrimitivesDrawer.GetClass("explosion");
    switch (effectNodes[offset].effectType){
    case etAreaEffect:
		detector = gPhysicalManager.CreateDetector();
		detector->SetPosition( pos );
        //effectNodes[offset+paramOffset(etAreaEffect,ptArea)].fParam;
		r = GetParamNode(offset,ptArea).fValue.Evaluate(context);
		detector->SetBoundingCircle( r );
        victimCategory = effectNodes[offset+paramOffset(etAreaEffect,ptFilter)].iParam;
		detector->SetCollidingPhysicalsAndCasterSide( victimCategory, source->DeterminePhysicalSource()->GetSideAndCategory().side ); // FIXME I think this can explode // versus
		detector->SetBehaviour( APPLY_EFFECT );
		detector->SetEffectOffset( offset + paramOffset(etAreaEffect,ptSubEffect) );
		detector->SetFramesToDeath( 1 );
        detector->SetSource(source);
        detector->SetExecutionContext(context);
        //gPrimitivesDrawer.DrawCircle(pc,pos,effectNodes[offset+1].fParam);
        break;
    case etChanceEffect:
		if (gRand.Rndf() <= GetParamNode(offset,ptChance).fValue.Evaluate(context)){
            PerformAt(pos,offset+paramOffset(etChanceEffect,ptSubEffect),source,context,focus);
        }
        break;
    case etGraphicalEffect:
        {
            SEffectParamSet eps = gGraphicalEffects.Prepare( *effectNodes[offset+paramOffset(etGraphicalEffect,ptName)].sParam );
            eps.duration = GetParamNode(offset,ptTime).fValue.Evaluate(context);
            gGraphicalEffects.ShowEffect( eps, pos );
        }
        break;
    case betLinkedGraphicalEffect:
        {
            CPhysical *phys = NULL;
            if (source->primary.ContainsPhysical()){
                phys = source->primary.physical;
            } else if ((source->primary.type != estNextSource) && (source->secondary.ContainsPhysical())){
                phys = source->secondary.physical;
            }
            if (phys){
                SEffectParamSet eps = gGraphicalEffects.Prepare( *effectNodes[offset+paramOffset(betLinkedGraphicalEffect,ptName)].sParam);
		    	eps.duration = GetParamNode(offset,ptTime).fValue.Evaluate(context);
	    		eps.offsetX = GetParamNode(offset,ptOffsetX).fValue.Evaluate(context);
    			eps.offsetY = GetParamNode(offset,ptOffsetY).fValue.Evaluate(context);
                gGraphicalEffects.ShowEffect( eps, pos, phys );
            } else {
                fprintf(stderr,"linked graphical effect with damaged source; printing and aborting\n");
                source->PrintToStderr();
            }
        }
        break;
    case etEffectList:
        offset++;
        count = effectNodes[offset].iParam;
        offset++;
        for (int i = 0; i < count; i++){
            PerformAt(pos,offset,source,context);
            offset = effectNodes[offset].endOffset+1;
        }
        break; 
	case betBullet: 
		{
			int tmp = offset + paramOffset(betBullet,ptBulletEmitter);
			int rot;
			if (source->primary.type == estBullet){
				rot = source->primary.bullet->GetRotation();
			} else if (source->primary.type == estGenericPhysical){
				rot = source->primary.physical->GetRotation();
			} else {
				rot = 0;
			}
			effectNodes[tmp].bulletEmitter->Shoot(pos,rot,0.0f,context,source);
			break;
		}
    case betConsole:{ // specjalne traktowanie - najpierw licznik, potem text
    	std::map<std::wstring, std::wstring> params;
    	params[L"$X"] = StringUtils::ConvertToWString(StringUtils::ToString(pos.x));
    	params[L"$Y"] = StringUtils::ConvertToWString(StringUtils::ToString(pos.y));
        for (int i = 0; i < effectNodes[offset+1].iParam; i++)
            gCommands.ParseCommand(*(effectNodes[offset+2].wsParam+i),params);
        break;
    }
	case betSound:
		{
			int size = effectNodes[offset+1].iParam;
			if (size == 0) break;
			int r = gRand.Rnd(0,size);
			System::Resource::CSound *s = gResourceManager.GetSound(*effectNodes[offset+2+r].sParam);
			if (s!= NULL)
				gAudioManager.GetSoundPlayer().Play(s, pos);
			break;
		}		
    case betJump:
		PerformAt(pos,GetParamNode(offset,ptName).iParam,source,context);
		break;
	case betInject:
		if (context != NULL){
			CPinnedAbilityBatch *pab = context->GetPinnedAbilityBatch();
			if (pab != NULL){
				std::string *tree = GetParamNode(offset,ptTree).sParam;
				std::string *code = GetParamNode(offset,ptCode).sParam;
				std::string *tag = GetParamNode(offset,ptName).sParam;
				if (pab->GetAbilityLevel(*tree,*code) >= 1){
					ExecutionContextPtr newCtx = pab->MorphContext(*tree,*code, context);
					if (mTags.count(*tag) > 0)
						PerformAt(pos,mTags[*tag],source,newCtx);
					else
						fprintf(stderr,"WARNING: tag: %s not found\n",tag->c_str());
				}
			}
		}
		break;
    default:
        return;
    }
}

bool CEffectManager::ParseEffect(CXml &xml, xml_node<> *node){
    SEffectNode en;
	xml_node<> *n;
    int tmp;
    std::wstring *tmp_wstr_ptr,tmp_wstr;
	std::string tmp_str;
    bool ok;
    std::wstring effectType = StringUtils::ConvertToWString(xml.GetString(node,"type"));
    EffectType et = parseEffect(effectType,xml.GetFilename());
    en.effectType = et;
	en.debugInfo = xml.GetFilename();
    effectNodes.push_back(en);
    std::vector<ParamType> &params = availableParams(et);
    ParamType pt;
	//bool addedTag = false;
	tmp_str = xml.GetString(node,"tag");
	if (tmp_str != ""){
		mTags[tmp_str] = effectNodes.size()-1;
		//addedTag = true;
		//fprintf(stderr,"Added tag %s at %d\n",tmp_str.c_str(),mTags[tmp_str]);
	}
    switch(et){
        case etEffectList:
            tmp = (int)effectNodes.size();
            // lista w xmlu to tylko seria kolejnych <effect type="...">;
            // w pamieci ulatwiamy sobie zycie podajac dlugosc listy na jej poczatku
            // mini-hack: liczymy efekty, wpisujac tu odpowiednia liczbe
            en.iParam = 0;
            effectNodes.push_back(en);
            ok = true;
            for ( n=xml.GetChild(node,paramName(ptSubEffect)); n; n=xml.GetSibl(n,paramName(ptSubEffect)) ){
                effectNodes[tmp].iParam++;
                ok = ok && ParseEffect(xml,n);
            }
            if (!ok){
                fprintf(stderr,"WARNING: Failed to parse sub-effects for list effect in %s\n",xml.GetFilename().c_str());
                effectNodes[tmp].iParam=0;
            }
            return ok;
        case betSound:
            tmp = (int)effectNodes.size();
            // analogicznie jak przy liscie, chcemy miec kilka wezlow name pod dzwiekiem
            en.iParam = 0;
            effectNodes.push_back(en);
            for ( n=xml.GetChild(node,paramName(ptName)); n; n=xml.GetSibl(n,paramName(ptName)) ){
                effectNodes[tmp].iParam++;
				en.sParam = new std::string(xml.GetString(n,"value"));
				effectNodes.push_back(en);
            }
            break;
        case betConsole: // konsola - zbieramy wiele linii w jeden blok pamieci
            tmp_wstr = StringUtils::ReinterpretFromUTF8(xml.GetString(xml.GetChild(node,"text")));
            tmp_wstr_ptr = StringUtils::WSplit(tmp_wstr,'\n',&en.iParam);
            effectNodes.push_back(en);
            en.wsParam = tmp_wstr_ptr;
            effectNodes.push_back(en);
            break;
        default:
            for (unsigned int i = 0; i < params.size(); i++){
                pt = params[i];
                //fprintf(stderr,"Parsing param %s for effect %ls at offset %ud\n",paramName(pt).c_str(),effectName(et).c_str(),effectNodes.size());
                switch(pt){
                case ptBadParam:
                    en.iParam = 0;
                    break;
                case ptSubEffect:
					n = xml.GetChild(node,paramName(ptSubEffect));
					if (n){
                        return ParseEffect(xml,n); 
                    } else {
                        fprintf(stderr,"WARNING: Failed to find sub-effect for %ls, in %s\n",effectName(et).c_str(),xml.GetFilename().c_str());
                        en.effectType = etNoEffect;
                        effectNodes.push_back(en);
                        return false;
                    }
                    break;
                case ptFilter:
                    en.iParam = ParsePhysicalFilter(
                        xml.GetString(xml.GetChild(node,paramName(pt)),"value"));
                    break;
                case ptName:
                    switch(et){
                    case etGraphicalEffect:
                    case betLinkedGraphicalEffect:
					case betInject:
                        en.sParam = new std::string(xml.GetString(xml.GetChild(node,paramName(pt)),"value"));
                        break;
					case betJump:
						tmp_str = xml.GetString(xml.GetChild(node,paramName(pt)),"value");
						if (mTags.count(tmp_str)>0){
							//fprintf(stderr,"found tag \"%s\"in %s at %d\n",tmp_str.c_str(),xml.GetFilename().c_str(),mTags[tmp_str]);
							//fprintf(stderr,"it is an %ls\n",effectName(effectNodes[mTags[tmp_str]].effectType).c_str());
							en.iParam = mTags[tmp_str];
						}else
							fprintf(stderr,"WARNING: can't find tag \"%s\"in %s\n",tmp_str.c_str(),xml.GetFilename().c_str());
						break;
                    default:
                        fprintf(stderr,"WARNING: I don't know how to interpret \"name\" inside of %ls, in %s\n",effectName(et).c_str(),xml.GetFilename().c_str());
                        break;
                    }
                    break;
                case ptTree:
				case ptCode:
					en.sParam = new std::string(xml.GetString(xml.GetChild(node,paramName(pt)),"value"));
					break;
                case ptAspect:
                    en.aspect = xml.GetAspect(
                        xml.GetChild(node,paramName(pt)),"value");
                    break;
                case ptDamageType:
                    n = xml.GetChild(node,paramName(pt));
                    if (n){
                        en.damageType = DamageTypes::ParseDamageType(StringUtils::ConvertToWString(
                            xml.GetString(n,"value")));
                    } else
                        en.damageType = DamageTypes::dtPhysical;
                    break;
                case ptBulletEmitter:
                    en.bulletEmitter = new CBulletEmitter();
                    // ladowanie odroczone, zeby sie offsety nie popsuly
                    mDelayedLoadables.push_back(en.bulletEmitter);
                    mDelayedLoadablesNodes.push_back(xml.GetChild(node,paramName(pt)));
                    break;
                case ptTargetChooser:
                    en.targetChooser = new CTargetChooser();
                    en.targetChooser->Load(xml,xml.GetChild(node,paramName(pt)));
                    break;
                case ptSummoner:
                    en.summoner = new CSummoner();
                    mDelayedLoadables.push_back(en.summoner);
                    mDelayedLoadablesNodes.push_back(xml.GetChild(node,paramName(pt)));
                    break;
                case ptShowAmount:
                    en.iParam = (int)(!!xml.GetChild(node, paramName(pt)));
                    break;
				default: // jakis floatowy param, sporo ich jest
					{ 
						rapidxml::xml_node<> *n = xml.GetChild(node,paramName(pt));
						en.fValue = CComputedValueFactory::Parse(xml,n);
						break;
					}
                }
                effectNodes.push_back(en);
            }
            break;
    }
    return true;
}

CEffectHandle *CEffectManager::LoadEffect(CXml &xml, rapidxml::xml_node<> *node){
    CEffectHandle *eh = new CEffectHandle((int)effectNodes.size());
    if (ParseEffect(xml,node)){
        Compile(eh->GetOffset());
        // bulletEmitery ladujemy po zaladowaniu wszystkiego i kompilacji,
        // bo zapewne zaladuja wlasne efekty, co popsuje offsety
		//
		// while a nie if, bo moga byc zagniezdzone
        while (mDelayedLoadables.size() > 0){
            // kopiujemy... zeby uniknac ladowania tych samych emiterow w 
            // rekurencyjnym przebiegu
            std::vector<IDefferedLoadable*> emitters = mDelayedLoadables;
            std::vector<rapidxml::xml_node<>*> nodes = mDelayedLoadablesNodes;
            // czyscimy...
            mDelayedLoadables.clear();
            mDelayedLoadablesNodes.clear();
            // i wreszcie ladujemy
            for (unsigned int i = 0; i < emitters.size(); i++)
                emitters[i]->Load(xml,nodes[i]);
        }
        return eh;
    } else {
        fprintf(stderr,"WARNING: Effect parsing aborted\n");
        delete eh;
        return NULL;
    }
}

const SEffectNode &CEffectManager::GetParamNode(int offset, ParamType pt){
	EffectType et = effectNodes[offset].effectType;
	return effectNodes[offset+paramOffset(et,pt)];
}

void CEffectManager::Serialize(int offset, std::ostream &out, int indent){
    EffectType et = effectNodes[offset].effectType;
	//src/Logic/Effects/CEffectManager.cpp:832: warning: unused variable 'params'
    //std::vector<ParamType> &params = availableParams(et);
    std::string indentString = "";
    for (int i = 0; i < indent+1; i++)
        indentString += '\t';
    switch(et){
        case betConsole:
            out << "[\n";
            for (int i = 1; i < effectNodes[offset+1].iParam - 1; i++)
                out << indentString << StringUtils::ConvertToString(*(effectNodes[offset+2].wsParam+i)) << "\n";
            out << indentString << "]";
            break;
        default:
            out << indentString << "<!-- CCondition::Serialize is not fully implemented! -->\n";
    }
}
