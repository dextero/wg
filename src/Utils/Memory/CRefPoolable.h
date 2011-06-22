#ifndef __REFPOOLABLE_H__
#define __REFPOOLABLE_H__

#include <cstdio>

/**
 * Liosan, 23.06.09
 * Klasa wspierajaca zarowno bycie w puli pamieci, jak i zliczanie referencji
 */

#include "CPoolable.h"
#include "CPool.h"
namespace Memory{

class CRefPoolable: public CPoolable{
private:
    int mRefCnt;
    CPoolBase *mPool;
public:
    CRefPoolable():mRefCnt(0),mPool(NULL){}
    CRefPoolable(const CRefPoolable &other): mRefCnt(0),mPool(other.mPool){
    }
    virtual ~CRefPoolable(){}

    // wywolywane z powodu referencji
    virtual void Release();
    virtual void Bind(CPoolBase *pool);
    inline int &GetRefCnt(){ return mRefCnt; }
};

}
#endif

