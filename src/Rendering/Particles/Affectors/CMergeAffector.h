#ifndef __PART_CMERGEAFFECTOR_H__
#define __PART_CMERGEAFFECTOR_H__

#include "../IAffector.h"

namespace part
{    
    namespace affector
    {       
        /// Pozwala scaliæ 2 afektory w jeden umo¿liwiaj¹c tworzenie skomplikowanyh systemów
        /// Nie mog¹ byæ NULL
        class CMerge : public IAffector
        {
            CMerge() {}
        public:
            CMerge(IAffector* a, IAffector* b) : m_a(a), m_b(b)
            {
                assert(m_a != NULL);
                assert(m_b != NULL);
            }
            virtual void Modify(CParticle &p, float dt)
            {
                m_a->Modify(p,dt);
                m_b->Modify(p,dt);
            }

            virtual std::string AffectorName() const { return "Merge"; }

            IAffector *GetA() { return m_a; }
            IAffector *GetB() { return m_b; }
        private:
            IAffector *m_a;
            IAffector *m_b;
        };

        /// Pozwala scaliæ 3 afektory w jeden.
        /// Nie mog¹ byæ NULL
        class CMerge3 : public IAffector
        {
            CMerge3() {}
        public:
            CMerge3(IAffector *a, IAffector *b, IAffector *c) : m_a(a), m_b(b), m_c(c)
            {
                assert(m_a != NULL);
                assert(m_b != NULL);
                assert(m_c != NULL);
            }
            virtual void Modify(CParticle &p, float dt)
            {
                m_a->Modify(p,dt);
                m_b->Modify(p,dt);
                m_c->Modify(p,dt);
            }

            virtual std::string AffectorName() const { return "Merge3"; }
            
            IAffector *GetA() { return m_a; }
            IAffector *GetB() { return m_b; }
            IAffector *GetC() { return m_c; }
        private:
            IAffector *m_a;
            IAffector *m_b;
            IAffector *m_c;
        };
    };
};

#endif //__PART_CMERGEAFFECTOR_H__//
