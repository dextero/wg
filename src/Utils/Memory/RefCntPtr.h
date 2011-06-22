#ifndef __REF_CNT_PTR_H__
#define __REF_CNT_PTR_H__

#include <cstdio>

/**
 * Liosan, 23.06.09
 * Inteligentny wskaznik - zlicza referencje do obiektu, przy braku referencji zwolni obiekt.
 * Pozwala na trzymanie null-a.
 *
 * Wymagania co do klas wewnatrz-szablonowych:
 * - void Release(); // wolana zamiast destruktora przy braku referencji
 * - int &GetRefCnt(); // zwraca referencje do licznika referencji (poczatkowo 0)
 * - konstruktor kopiujacy powinien zerowac licznik referencji
 * Chyba najprosciej jest dziedziczyc po CRefPoolable
 *
 * Proponuje uzywac via typedef:
 * typedef RefCntPtr<EffectSource> EffectSourcePtr;
 * etc.
 */

namespace Memory{

template<class T>
class RefCntPtr{
private:
    T *obj;
    // zaklada obj!=null
    inline void Attach(){
        obj->GetRefCnt()++;
    }
    // zaklada obj!=null
    inline void Detach(){
        obj->GetRefCnt()--;
        if (obj->GetRefCnt()<=0)
            obj->Release();
    }
public:
    RefCntPtr(T *obj): obj(obj){
        if (obj!=NULL)
            Attach();
    }
    RefCntPtr(): obj(NULL){}
    RefCntPtr(const RefCntPtr<T> &other){
        obj = other.obj;
        if (obj!=NULL)
            Attach();
    }
    ~RefCntPtr(){
        if (obj!=NULL)
            Detach();
    }

    T& operator*(){
        return *obj;
    }

    T *operator->(){
        return obj;
    }

    inline bool operator==(const RefCntPtr<T> &other) const {
        return obj == other.obj;
    }
    
    inline bool operator!=(const RefCntPtr<T> &other) const {
        return obj != other.obj;
    }

    inline RefCntPtr<T> &operator=(const RefCntPtr<T> &other){
        if (&other == this)
            return *this;
        if (obj!=NULL)
            Detach();
        obj = other.obj;
        if (obj!=NULL)
            Attach();
        return *this;
    }
};

}

#endif

