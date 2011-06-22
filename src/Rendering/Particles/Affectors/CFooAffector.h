#ifndef __PART_CFOOAFFECTOR_H__
#define __PART_CFOOAFFECTOR_H__

#include "../IAffector.h"

namespace part
{
    namespace affector
    {
		/// Pusty affektor
		class CFooAffector : public IAffector
		{
		public:
			virtual void Modify(CParticle &p, float dt) {}
            virtual std::string AffectorName() const { return "Foo"; }
		};
	};
};
#endif //__PART_CFOOAFFECTOR_H__//
