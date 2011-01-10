#ifndef __CBLEEDER_H__
#define __CBLEEDER_H__

/**
 * Liosan, 03.07.09
 * Klasa zawierajaca opis mechanizmow krwawienia potwora.
 * Co prawda na razie nie ma czego opisywac, ale co tam.
 */

#include "../Effects/CEffectSource.h"

class CPhysical;

class CBleeder{
private:
    CPhysical *mAttached;
public:
    CBleeder(CPhysical *attached);
    ~CBleeder();

    inline CPhysical *GetPhysical(){ return mAttached; }
    inline void SetPhysical(CPhysical *attached){ mAttached = attached; }

    virtual void Bleed(float dmg,EffectSourcePtr source = NULL);
};

#endif /*__CBLEEDER_H__*/

