#include "CFocusObject.h"
#include "../../Utils/Memory/CPool.h"

int CFocusObject::InstancesCount = 0;

Memory::CPool<CFocusObject> foPool;

CFocusObject::CFocusObject(): 
mValid(true)
{
    InstancesCount++;
}

CFocusObject *CFocusObject::Alloc(){
    CFocusObject *focus = foPool.Alloc();
    return focus;
}

void CFocusObject::Invalidate(){
    mValid = false;
}
