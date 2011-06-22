#ifndef __CSINGLETONCLEANER_H__
#define __CSINGLETONCLEANER_H__

#include <assert.h>
#include <vector>
#include <cstdio>

class CCleanupable{
    friend class CSingletonCleaner;
protected:
    virtual void Cleanup(){};
    virtual ~CCleanupable(){};
};

class CSingletonCleaner
{
private:
    std::vector<CCleanupable *> cleanupables;
    static CSingletonCleaner *msInstance;
public:
    static CSingletonCleaner& GetSingleton();
    CSingletonCleaner();
    virtual ~CSingletonCleaner();

    void Register(CCleanupable *c);
};

#endif//__CSINGLETONCLEANER_H__

