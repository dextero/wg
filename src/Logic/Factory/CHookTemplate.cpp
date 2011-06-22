#include "CHookTemplate.h"
#include "../CPhysicalManager.h"
#include "../MapObjects/CHook.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Maths.h"
#include "../../Rendering/CDisplayable.h"
#include "../../Rendering/ZIndexVals.h"
#include "../../Rendering/CDrawableManager.h"
#include "../Effects/CEffectManager.h"

using namespace rapidxml;
using namespace StringUtils;
using namespace Maths;

std::string CHookTemplate::GetType()
{
	return "CHookTemplate";
}

bool CHookTemplate::Load(CXml &xml)
{
    if (!CPhysicalTemplate::Load(xml)) return false;
	
	if ( xml.GetString(0,"type") != "hook" )
		return false;

	xml_node<>*	n = xml.GetChild(xml.GetRootNode(),"effect");
    if (n != NULL)
        effect = gEffectManager.LoadEffect(xml,n);
	else
		effect = NULL;

	return true;
}

void CHookTemplate::Drop()
{
	delete this;
}

void CHookTemplate::PrepareEditorPreviewer(CDisplayable* d)
{
	d->SetStaticImage("data/GUI/hook-icon.png");

	if ( d->GetZIndex() != Z_PARTICLE )
	{
		gDrawableManager.UnregisterDrawable( d );
		gDrawableManager.RegisterDrawable( d, Z_PARTICLE );
	}
}

CHook *CHookTemplate::Create(std::wstring id)
{
    CHook* hook = gPhysicalManager.CreateHook();
	hook->SetCategory( PHYSICAL_HOOK );

	if (effect != NULL)
		effect->ApplyPermanent(hook);

	return hook;
}

