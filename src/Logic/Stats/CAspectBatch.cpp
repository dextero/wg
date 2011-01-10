#include "CAspectBatch.h"
#include <cassert>
#include "../../Utils/MathsFunc.h"

CAspectBatch::CAspectBatch():timestamp(0){
    for (int i = 0; i < Aspects::aspectCount; i++)
        values[i] = 0.0f;
    Set(aSpeed,5.0f);
    Set(aMaxHP,100.0f);
    Set(aMaxMana,100.0f);
    Set(aMass,1.f);
}

const float &CAspectBatch::operator[](EAspect a) const{
    int l = Maths::BinaryLog(((int)a)<<1);
    assert((l>=0) && (l<Aspects::aspectCount));
    return values[l];
}

void CAspectBatch::Set(EAspect a, float v){
    int l = Maths::BinaryLog(((int)a)<<1);
    assert((l>=0) && (l<Aspects::aspectCount));
    if (values[l]!=v){
        timestamp++;
        values[l] = v;
    }
}
