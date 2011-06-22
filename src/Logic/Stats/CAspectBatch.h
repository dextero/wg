#ifndef __CASPECTBATCH_H__
#define __CASPECTBATCH_H__

#include <vector>
#include "EAspect.h"

/**
 * Liosan, 26.06.09
 * Kontener na aspekty
 * 
 * Na razie bez logiki, ale jesli beda jakies powiazania miedzy aspektami
 * (np strength dodaje maxHP) to moga tutaj wyladowac (a moze lepiej w CStats...)
 */

class CAspectBatch{
private:
    int timestamp;
    float values[Aspects::aspectCount];
public:
    CAspectBatch();
    const float &operator[](EAspect a) const;
    void Set(EAspect a, float v); // potrzebna mi logika tutaj...
    inline int GetTimestamp() const { return timestamp; }
};

#endif /*__CASPECTBATCH_H__*/
