#include "CSingletonCleaner.h"

CSingletonCleaner *CSingletonCleaner::msInstance = NULL;

CSingletonCleaner& CSingletonCleaner::GetSingleton()
{
    assert(msInstance && "CSingletonCleaner should be created in main and not accesed otherwise");
    return ( *msInstance );
}

CSingletonCleaner::CSingletonCleaner()
{
    fprintf(stderr,"CSingletonCleaner::CSingletonCleaner()\n");
    msInstance = this;
}

CSingletonCleaner::~CSingletonCleaner()
{
    fprintf(stderr,"CSingletonCleaner::~CSingletonCleaner()\n");
    assert(msInstance);
    while (cleanupables.size()>0){
        cleanupables[cleanupables.size()-1]->Cleanup();
        cleanupables.pop_back();
		//fprintf(stderr,"CSingletonCleaner - %d objects registered\n",cleanupables.size());
    }
    msInstance=NULL;
}

void CSingletonCleaner::Register(CCleanupable *c)
{
    cleanupables.push_back(c);
	//fprintf(stderr,"CSingletonCleaner - %d objects registered\n",cleanupables.size());
}
