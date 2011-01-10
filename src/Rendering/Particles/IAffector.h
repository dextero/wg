#ifndef __PART_IAFFECTOR_H__
#define __PART_IAFFECTOR_H__

#include "CParticle.h"

namespace part
{
    namespace affector
    {
        // rAum: Interfejs afektora który modyfikuje zachowania cz¹steczek np: spowalnia/zmniejsza/koloruje etc
        class IAffector
        {
        public:
            virtual ~IAffector() {}
            virtual void Modify(CParticle &p, float dt) = 0;
            virtual std::string AffectorName() const = 0;
        };
    }
};

#endif //__PART_IAFFECTOR_H_//
