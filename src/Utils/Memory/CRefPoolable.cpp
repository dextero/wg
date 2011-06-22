#include "CRefPoolable.h"

using namespace Memory;

void CRefPoolable::Release(){
    if (mPool != NULL)
        mPool->DeallocMe(this);
    else
        delete this;
}

void CRefPoolable::Bind(CPoolBase *pool){
    mPool = pool;
}

