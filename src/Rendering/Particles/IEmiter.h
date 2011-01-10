#ifndef __PART_IEMITER_H_
#define __PART_IEMITER_H_

#include "CParticle.h"

namespace part
{
    namespace emiter
    {
        /// rAum: Interfejs emitera cz�steczek
        class IEmiter
        {
        public:
            // Funkcja emituj�ca nowe particle (na og� wtedy gdy dana cz�steczka jest martwa)
            // Zwraca ilo�� aktualnie �ywych cz�steczek.
            virtual unsigned Emit(CParticle *p, unsigned alive, unsigned count, float dt) = 0;
            
            virtual void SetSpeed(float speed) = 0;
            virtual float GetSpeed() = 0;

            IEmiter() {}
            virtual ~IEmiter() {};
        private:
        };
    };
};

#endif //__PART_IEMITER_H_

