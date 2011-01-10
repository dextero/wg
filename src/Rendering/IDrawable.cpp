#include "IDrawable.h"
#include "CDrawableManager.h"

IDrawable::~IDrawable(){
    if (mZVectorIndex >= 0)
        gDrawableManager.UnregisterDrawable(this);
}

