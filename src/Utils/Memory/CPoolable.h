#ifndef __CPOOLABLE_H__
#define __CPOOLABLE_H__

/**
 * Liosan, 23.06.09
 * Klasa wspierajaca bycie zawartym w puli pamieci
 */
#include "CPool.h"

namespace Memory{

class CPoolable{
public:
    CPoolable(){}
    virtual ~CPoolable();

    virtual void Bind(CPoolBase *pool);
};
}

#endif

