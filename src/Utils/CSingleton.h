#ifndef __CSINGLETON_H__
#define __CSINGLETON_H__

#include <assert.h>
#include "CSingletonCleaner.h"

template <typename T> class CSingleton: public CCleanupable
{
protected:
    static T* msSingleton;
    
    CSingleton()
    {
        assert( !msSingleton );
        msSingleton = static_cast< T* >( this );
        CSingletonCleaner::GetSingleton().Register(this);
    }
    
    virtual ~CSingleton()
    {
        assert( msSingleton );
        msSingleton = 0;
    }

	virtual void Cleanup() {}
public:
    static T& GetSingleton()
    {
        if (msSingleton == 0)
            new T();
        return ( *msSingleton );
    }

    static T* GetSingletonPtr()
    {
        return msSingleton;
    }
};
#endif//__CSINGLETON_H__

