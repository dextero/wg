#include "CEffectHandle.h"
#include "CEffectManager.h"

void CEffectHandle::Apply(CPhysical *physical, EffectSourcePtr source,ExecutionContextPtr context){
    gEffectManager.ApplyOnce(physical,offset, source,context);
}

void CEffectHandle::ApplyPermanent(CPhysical *physical, EffectSourcePtr source,ExecutionContextPtr context){
    gEffectManager.ApplyPermanent(physical,offset, source,context);
}

std::string CEffectHandle::GetType()
{
	return "CEffect";
}

void CEffectHandle::Drop()
{
	delete this;
}


bool CEffectHandle::Load(std::map<std::string,std::string>& argv)
{
	return true;
}


void CEffectHandle::PerformAt(const sf::Vector2f &pos, EffectSourcePtr source,ExecutionContextPtr context){
    gEffectManager.PerformAt(pos,offset,source,context);
}

void CEffectHandle::Serialize(std::ostream &out, int indent){
    gEffectManager.Serialize(offset,out,indent);
}