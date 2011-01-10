#ifndef __PART_CSPRAYAFFECTOR_H__
#define __PART_CSPRAYAFFECTOR_H__

#include "../IAffector.h"

namespace part
{
    namespace affector
    {
        struct SSprayParams
        {
            float radMinX, radMaxX;
            float radMinY, radMaxY;
            bool rndRotation; // jesli false to uzywa p.rotation
            SSprayParams() : radMinX(1.f), radMaxX(1.f), radMinY(1.f), radMaxY(1.f), rndRotation(true) {}
        };
        /// Modyfikuje czasteczke, rozpylajac ja nieco ;)
		class CSprayAffector : public IAffector
		{
		public:
            explicit CSprayAffector(SSprayParams &p) : param(p) {}

            void SetParams(SSprayParams &parameters) { param = parameters; }
            SSprayParams& GetParams() { return param; }

            virtual std::string AffectorName() const { return "Spray"; }

			virtual void Modify(CParticle &p, float dt)
            {
                switch (param.rndRotation)
                {
                case true:
                {
                    float ang = gRand.Rndf(0,360.f);
                    p.mod.force.x = sin(ang) * gRand.Rndf(param.radMinX, param.radMaxX) * 1000.f * dt;
                    p.mod.force.y = cos(ang) * gRand.Rndf(param.radMinY, param.radMaxY) * 1000.f * dt;
                }
                break;
                case false:
                    p.mod.force.x = sin(p.rotation) * gRand.Rndf(param.radMinX, param.radMaxX) * 1000.f * dt;
                    p.mod.force.y = cos(p.rotation) * gRand.Rndf(param.radMinY, param.radMaxY) * 1000.f * dt;
                }
            }
        private:
            SSprayParams param;
		};
	};
};
#endif //__PART_CSPRAYAFFECTOR_H__//

