#ifndef __PART_CINCREMENTALAFFECTOR_H__
#define __PART_CINCREMETNALAFFECTOR_H__

#include "../IAffector.h"
#include "../../../Utils/Maths.h"

namespace part
{
    namespace affector
    {
        /// Parametry afektora CIncremental
        struct SIncrementalParams
        {
            sf::Vector2f vel;
            sf::Vector2f grav;
            sf::Vector2f force;
            float life;
            float rotation;
            float size;

            SIncrementalParams(): vel(0.f, 0.f), grav(0.f, 0.f), force(0.f, 0.f), life(0.f), rotation(0.f), size(0.f) {}
        };

        class CIncremental : public IAffector
        {
        public:
            explicit CIncremental(SIncrementalParams &parameters) : param(parameters) {}

            void SetParams(SIncrementalParams &parameters) { param = parameters; }
            SIncrementalParams& GetParams() { return param; }

            virtual void Modify(CParticle &p, float dt)
            {
                p.pos += (param.vel + param.grav + param.force) * dt;
                
                p.life -= param.life * dt;
                p.life = Maths::Clamp(p.life,-1.f,1.f);

                p.rotation += param.rotation * dt;
                p.size += param.size * dt;              
            }
            virtual std::string AffectorName() const { return "Incremental"; }
        private:
            SIncrementalParams param;
        };
    };
};

#endif //__PART_CINCREMETNALAFFECTOR_H__//

