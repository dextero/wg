#ifndef __CTRIGGEREFFECTS_H__
#define __CTRIGGEREFFECTS_H__

/**
 * Liosan, 04.07: klasa pomocnicza, odpowiedzialna za trzymanie
 * informacji o tym, co powinno siê dziaæ w czasie rzucania czaru
 * (efekty graficzne, dzwiekowe itp)
 */

#include <string>
#include <vector>
#include "../../ResourceManager/CSound.h"
#include "../Effects/CEffectHandle.h"

class CAnimSet;
struct SAnimation;

class CTriggerEffects{
private:
    class CTriggerKeyMap;
    CTriggerKeyMap *mMap;
public:
    static const int AbilityKeyCount;

    struct STriggerEffect{
    public:
        System::Resource::CSound *mSound;
        CEffectHandle *mEffect;
        SAnimation *mAnim;

        STriggerEffect(): mSound(NULL),mEffect(NULL),mAnim(NULL){}
    };

    CTriggerEffects(const std::string &filename, CAnimSet *anims);

    STriggerEffect *Find(const std::vector<int> &sequence);
};

#endif /* __CTRIGGEREFFECTS_H__ */


