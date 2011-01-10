#ifndef __PART_CPOINTMASSAFFECTOR_H__
#define __PART_CPOINTMASSAFFECTOR_H__

#include "../IAffector.h"
#include "../../../Logic/CPhysical.h"
#include "../../../Utils/MathsFunc.h"
#include <list>

namespace part
{
    namespace affector
    {
        struct SPointMassParams
        {
            float mass;
            float minDistanceSquare;
            float maxDistanceSquare;
        };
        /// Punkty masy wp³ywaj¹ce na cz¹steczkê
        class CPointMass : public IAffector
		{
		public:
            explicit CPointMass(SPointMassParams &p) : param(p) {}

            void SetParams(SPointMassParams &parameters) { param = parameters; }
            SPointMassParams& GetParams() { return param; }

            void AddPhysical(const CPhysical *point) { points.push_back(point); }

            virtual std::string AffectorName() const { return "PointMass"; }

			virtual void Modify(CParticle &p, float dt)
            {
                float distanceSquare;
                const CPhysical *phys(NULL);
                for (it = points.begin(); it != points.end();)
                {
                    phys = *it;

                    if (phys == NULL) {
                        points.erase(it++);
                        continue;
                    }

                    distance = phys->GetPosition() - p.pos / 64.f;
                    distanceSquare = Maths::LengthSQ(distance);

                    if (distanceSquare > param.maxDistanceSquare) {
                        ++it; continue;
                    }                

                    p.mod.force += (Maths::Normalize(distance) * param.mass * dt) / std::max(param.minDistanceSquare, distanceSquare);

                    ++it;
                }
            }
        private:
            SPointMassParams param;
            sf::Vector2f distance;            
            std::list<const CPhysical *> points;
            std::list<const CPhysical *>::iterator it;
		};
	};
};
#endif //__PART_CPOINTMASSAFFECTOR_H__//

