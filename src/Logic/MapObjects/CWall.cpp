#include "CWall.h"
#include "../../Rendering/ZIndexVals.h"
#include "../../Rendering/CDisplayable.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../CPhysicalManager.h"
#include "../CLogic.h"

CWall::CWall(const std::wstring& uniqueId): CPhysical(uniqueId)
{
	if ( gLogic.GetState() == L"editor" )	SetZIndex(Z_EDITOR_WALL);
	else									SetZIndex(Z_WALL);
#ifdef __I_AM_TOXIC__
	SetZIndex(Z_EDITOR_WALL);
    SetImage("data/physicals/invisible-wall.png");
#endif
}

CWall::~CWall(){
}
