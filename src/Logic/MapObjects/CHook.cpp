#include "CHook.h"
#include "../CLogic.h"

CHook::CHook(const std::wstring& uniqueId): 
CPhysical(uniqueId),
mAppliedEffectContainer(this)
{
	if ( gLogic.GetState() == L"editor" )
		SetImage("data/GUI/hook-icon.png");
}

CHook::~CHook(){
}


CAppliedEffectContainer *CHook::GetAppliedEffectContainer(){
	return &mAppliedEffectContainer;
}

void CHook::Update(float dt) {
	mAppliedEffectContainer.Update(dt);
}
