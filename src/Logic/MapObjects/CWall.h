#ifndef CWALL_H_
#define CWALL_H_

#include <string>
#include <vector>

#include "../CPhysical.h"

class CWall: public CPhysical {
    friend class CPhysicalManager;
protected:
    CWall(const std::wstring& uniqueId);
    virtual ~CWall();
};

#endif /*CWALL_H_*/
