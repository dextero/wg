#ifndef __PART_CPARTICLEPOOL_H__
#define __PART_CPARTICLEPOOL_H__

#include "CParticle.h"
#include "../../Utils/CSingleton.h"

#define gParticlePool CParticlePool::GetSingleton()

namespace part
{
    // Zarz¹dza particlami, tak aby nie przydzielaæ zbêdnej iloœci pamiêci ;]
    // Póki co jest to opakowanie new i delete - w celu zbadania zapotrzebowania
    // pamiêci na particle, by potem to wykorzystac przy pisaniu czegos intelignetniejszego
    class CParticlePool : public CSingleton<CParticlePool>
    {
        unsigned m_particleCount;
        unsigned m_maxParticles;
        unsigned m_createCount;
        unsigned m_releaseCount;
    public:
        CParticlePool() : m_particleCount(0), m_maxParticles(0), m_createCount(0), m_releaseCount(0) {}
        
        CParticle *CreateParticles(unsigned count)
        {
            CParticle *allocate = new CParticle[count];

            m_particleCount += count;

            if (m_particleCount > m_maxParticles)
                m_maxParticles = m_particleCount;
            
            ++m_createCount;

            return allocate;
        }

        void Release(CParticle *r, unsigned count)
        {
            assert(r != 0);
            
            delete[] r;

            ++m_releaseCount;

            m_particleCount -= count;
        }

        unsigned GetCreateCallNum() { return m_createCount; }
        unsigned GetReleaseCallNum() { return m_releaseCount; }

        unsigned CurrentParticleCount() { return m_particleCount; }
        unsigned MaximumParticleCount() { return m_maxParticles; }
    };
};

#endif //__PART_CPARTICLEPOOL_H__//

