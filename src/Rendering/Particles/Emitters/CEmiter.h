#ifndef __PART_CEMITER_H__
#define __PART_CEMITER_H__

#include "../IEmiter.h"
#include "SEmiterTemplate.h"

class CPhysical;

namespace part
{
    namespace emiter
    {
        class CEmiter : public IEmiter
        {
        public:
            CEmiter( const SEmiterTemplate &tmp, sf::Vector2f position );
            CEmiter( const SEmiterTemplate &tmp, const CPhysical* physical );
            CEmiter( const SEmiterTemplate &tmp );

            void Reattach( sf::Vector2f newpos) { pos = newpos; tracking = STATIC; }
            void Reattach( const CPhysical* physical = NULL ) { obj = physical; tracking = (physical != NULL)? PHYSICAL : INVALID; }

            const CPhysical* GetPhysicalIfAssigned() { return tracking == PHYSICAL? obj : NULL; }
            
            virtual void SetSpeed(float speed) { param.emitSpeed = speed; }
            virtual float GetSpeed() { return param.emitSpeed; }
        private:
            CEmiter() {}

            virtual unsigned Emit(CParticle *ps, unsigned alive, unsigned count, float dt);
            SEmiterTemplate param;
            sf::Vector2f pos;
            const CPhysical* obj;
            enum { INVALID = -1, STATIC, PHYSICAL } tracking;
            float mAcc;
        };
    };
};

#endif //__PART_CEMITER_H__//
