#ifndef __PART_CRANDOMAFFECTOR_H__
#define __PART_CRANDOMAFFECTOR_H__

#include "../IAffector.h"
#include "../../../Utils/MathsFunc.h"
#include "../../../Utils/CRand.h"

namespace part
{
    namespace affector
    {
        // Struktura opisuj¹ca parametry "losowego afektora"
        struct SRandomParams
        {
            float lifeMin, lifeMax;
            float rotMin, rotMax;
            float sizeMin, sizeMax;
            sf::Vector2f velMin;
            sf::Vector2f velMax;
            //----------------------
            SRandomParams() : lifeMin(0.001f), lifeMax(0.001f), rotMin(0.f), rotMax(0.f), sizeMin(0.f), sizeMax(0.f) {}
        };

        class CRandom : public IAffector
        {
        public:
            explicit CRandom(SRandomParams &parameters) : param(parameters) {}

            void SetParams(SRandomParams &parameters) { param = parameters; }
            SRandomParams &GetParams() { return param; }

            virtual void Modify(CParticle &p, float dt)
            {
                p.pos += sf::Vector2f ( gRand.Rndf(param.velMin.x,param.velMax.x),
                                        gRand.Rndf(param.velMin.y,param.velMax.y)) * dt;

                p.life -= gRand.Rndf(param.lifeMin,param.lifeMax) * dt;
                p.life = Maths::Clamp(p.life,-1.f,1.f);

                p.rotation += gRand.Rndf(param.rotMin,param.rotMax) * dt;
                p.size += gRand.Rndf(param.sizeMin, param.sizeMin) * dt;
            }

            virtual std::string AffectorName() const { return "Random"; }

        private:
            SRandomParams param;
        };
    };
};

#endif //__PART_CRANDOMAFFECTOR_H__//
