#ifndef __PART_IEMITER_H_
#define __PART_IEMITER_H_

#include "CParticle.h"

namespace part
{
    namespace emiter
    {
        /// rAum: Interfejs emitera cz¹steczek
        class IEmiter
        {
        public:
            // Funkcja emituj¹ca nowe particle (na ogó³ wtedy gdy dana cz¹steczka jest martwa)
            // Zwraca iloœæ aktualnie ¿ywych cz¹steczek.
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

