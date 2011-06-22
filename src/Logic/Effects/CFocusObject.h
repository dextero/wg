#ifndef __CFOCUSOBJECT_H__
#define __CFOCUSOBJECT_H__

#include <string>
#include "../../Utils/Memory/CRefPoolable.h"
#include "../../Utils/Memory/RefCntPtr.h"

/**
 * Liosan, 19.10
 * obiekt informujacy o koncentracji rzucajacego dany czar
 * jesli focus rzucajacego zostanie zlamany, wszystkie efekty 
 * oznaczone jako <effect type="focus"> zostana przerwane
 * 
 * Realizujemy to w ten sposob, a nie przez np. wskaznik na aktora,
 * bo smierc aktora jest jednym ze sposobow przerwania focusu 
 * - a to by oznaczalo dyndajacy wskaznik
 */
class CFocusObject;

typedef Memory::RefCntPtr<CFocusObject> FocusObjectPtr;

class CFocusObject : public Memory::CRefPoolable {
private:
    static int InstancesCount;
protected:
    friend class Memory::CPool<CFocusObject>;

    CFocusObject();
    ~CFocusObject(){ InstancesCount--; }

    bool mValid;
public:
    static inline int GetInstanceCount(){ return InstancesCount; }

    inline bool StillValid(){ return mValid; }
    void Invalidate();
    
    static CFocusObject *Alloc();
};

#endif /*__CFOCUSOBJECT_H__*/
