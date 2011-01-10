#include <cassert>

#include "CStats.h"
#include "../CPhysical.h"
#include "../../Utils/MathsFunc.h"
#include "../Effects/CEffectManager.h"
#include "../../Rendering/CDisplayable.h"

CStats::CStats(CPhysical *attached) :
mAttached(attached),
mHP(100),
mMana(100)
{
    //assert(mAttached!=NULL);
}

CStats::~CStats(){
}

void CStats::SetBaseAspect(EAspect a, float v){
    mBaseAspects.Set(a,v);
    if (mAttached != NULL)
        gEffectManager.RecalculateModifier(mAttached,a);
    else
        SetCurrAspect(a,v);
}

void CStats::LerpContent(CStats *min,CStats *max, float f){
    assert(min!=NULL);
    assert(max!=NULL);
    assert((f<=1.0f)&&(f>=0.0f));
    for (int i = 0; i < Aspects::aspectCount-1; i++)
        SetBaseAspect((EAspect)(1 << i), Maths::Lerp(min->GetBaseAspect((EAspect)(1 << i)),max->GetBaseAspect((EAspect)(1 << i)),f));
    mHP = Maths::Lerp(min->GetHP(),max->GetHP(),f);
    mMana = Maths::Lerp(min->GetMana(),max->GetMana(),f);
}

void CStats::Load(CXml &xml,const std::string &attr){
    for (int i = 0; i < Aspects::aspectCount-1; i++)
        SetBaseAspect((EAspect)(1 << i),xml.GetFloat( StringUtils::ConvertToString( Aspects::aspectNames[i+1]), attr ));
    SetHP(GetCurrAspect(aMaxHP));
    SetMana(GetCurrAspect(aMaxMana));
}

void CStats::SetCurrAspect(EAspect a, float v)
{ 
    mCurrAspects.Set(a, v);
    switch(a){
		case aScale:
			if (mAttached != NULL)
			{	
				if (mAttached->GetDisplayable() != NULL)	mAttached->GetDisplayable()->SetScale(v);
				if (mAttached->GetShadow() != NULL)			mAttached->GetShadow()->SetScale(v);
			}
			break;
        case aRadius: 
            if (mAttached != NULL)
                mAttached->SetBoundingCircle(v); 
            break;
        case aMaxHP: if (mHP > v) mHP = v; break;
        case aMaxMana: if (mMana > v) mMana = v; break;
        case aSpeed: 
            if (mAttached != NULL)
                if (Maths::Length(mAttached->GetVelocity()) > v) 
                    mAttached->SetVelocity(Maths::Normalize(mAttached->GetVelocity())*v); 
            break;
        case aMass:
            mCurrAspects.Set(a,v <= 0.0f? 0.0000001f : v);
            break;
        default:
            break;
    }
}

void CStats::DoDamage(float dmg){
    SetHP(mHP - dmg); 
}

void CStats::SetHP(float hp){ 
    mHP = hp; 
    if (mHP > GetCurrAspect(aMaxHP)) 
        mHP = GetCurrAspect(aMaxHP); 
    else if (mHP < 0.f) {
        mHP = 0.f;
        mAttached->Kill();
    }
}

bool CStats::TryDrainHealth(float val){
    if (mHP > val){
        SetHP(mHP - val);
        return true;
    } else return false;
}

void CStats::RestoreMana(float val){
    mMana+=val;
    if (mMana > GetCurrAspect(aMaxMana)) 
        mMana = GetCurrAspect(aMaxMana); 
}

void CStats::DrainMana(float val){
    mMana -= val;
    if (mMana < 0.0f)
        mMana = 0.0f;
}

bool CStats::TryDrainMana(float val){
    if (mMana > val){
        mMana -=val;
        return true;
    } else return false;
}

