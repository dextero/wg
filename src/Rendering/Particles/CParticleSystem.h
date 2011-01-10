#ifndef __CPARTICLE_SYSTEM_H__
#define __CPARTICLE_SYSTEM_H__

#include "CParticle.h"
#include "IAffector.h"
#include "IEmiter.h"
#include "Emitters/CEmiter.h"
#include "Emitters/SEmiterTemplate.h"
#include "../../Utils/CRand.h"

#include "../Effects/EffectTypes.h"

#include <assert.h>

class CPhysical;

namespace part
{
    namespace State
    {
        enum SystemState { Normal, Hidden, Freeze, ToDelete, CanDelete };
    };

    // interfejs systemu particli
    class IParticleSystem
    {
    public:
        virtual ~IParticleSystem() {};

        virtual void Draw() = 0;
        virtual void Update(float dt) = 0;
        
        virtual State::SystemState GetState() const = 0;
        virtual void SetState(State::SystemState state) = 0;
        
        virtual unsigned TotalParticles() const = 0;
        virtual unsigned AliveParticles() const = 0;

        virtual void SetAffector(affector::IAffector *) = 0;
        virtual void SetEmiter(emiter::IEmiter *) = 0;

        virtual emiter::IEmiter *GetEmiter() = 0;

        virtual sf::Rect<int> GetBoundingBox()  const = 0;

        virtual void SetDuration(float duration) = 0;
        virtual float GetDuration() const = 0;
        virtual float GetSystemAge() const = 0;
        virtual void ResetSystemAge() = 0;

        virtual void AssignPhysical(const CPhysical *) {}

        virtual EffectHandle GetEffectHandle() const = 0;
    };

    // "Szablon/Wzór" dla systemu particli
    struct SParticleSystemTemplate
    {
        affector::IAffector *mAffector;
        unsigned mCount;
        emiter::SEmiterTemplate *mEmiterTemplate;
        float mDurationMin,
              mDurationMax;
        sf::Blend::Mode mBlendMode;
        EffectHandle mHandle; // dla identyfikacji rodziny efektu

        explicit SParticleSystemTemplate(unsigned count = 0, affector::IAffector *a = 0, emiter::SEmiterTemplate *e = 0, int tex = 0, float d = 0.f) 
            : mAffector(a), mCount(count), mEmiterTemplate(e), mDurationMin(d), mDurationMax(d), mBlendMode(sf::Blend::Alpha), mHandle(0) {}
    };

    // Reprezentuje system particli (np: jeden efekt jak ogieñ)
    class CParticleSystem : public IParticleSystem
    {
    public:
        explicit CParticleSystem(unsigned count)
            : mParam(count), mParticles(0), mAutodel(false), mState(State::Normal), mDuration(0), mSystemAge(0)
        {
            Recreate(count);
            mEmiter = new emiter::CEmiter(*mParam.mEmiterTemplate);
        }
        
        CParticleSystem(const SParticleSystemTemplate &pst)
              : mParam(pst), mParticles(0), mAutodel(false), mState(State::Normal), mSystemAge(0)
        {
            mDuration = gRand.Rndf(mParam.mDurationMin, mParam.mDurationMax);
            Recreate(pst.mCount);
            mEmiter = new emiter::CEmiter(*mParam.mEmiterTemplate);
        }

        virtual ~CParticleSystem() { Clean(); }
        void Clean();

        virtual void Draw();
        virtual void Update(float dt);
        
        virtual State::SystemState GetState() const { return mState; }
        virtual void SetState(State::SystemState state) { mState = state; }
        
        virtual unsigned TotalParticles() const { return mParam.mCount; }
        virtual unsigned AliveParticles() const { return mAlive; }

        virtual sf::IntRect GetBoundingBox() const { return sf::IntRect(); }

        virtual void SetEmiter(emiter::IEmiter *emiter) { mEmiter = emiter; }
        virtual emiter::IEmiter *GetEmiter() { return mEmiter; }

        /// autodel dawaæ na true w przypadku niezarz¹dzanego afektora
        virtual void SetAffector(affector::IAffector *affector) { SetAffector(affector,false); }
        virtual void SetAffector(affector::IAffector *affector, bool autodel) { mParam.mAffector = affector; mAutodel = autodel; }

        virtual void SetDuration(float duration) { mDuration = duration; }
        virtual float GetDuration() const { return mDuration; }
        virtual float GetSystemAge() const { return mSystemAge; }
        virtual void ResetSystemAge() { mSystemAge = 0.f; }

        virtual void AssignPhysical(const CPhysical *);

        virtual EffectHandle GetEffectHandle() const { return mParam.mHandle; }

    private:
        /// Pozwala zmieniæ liczbê cz¹steczek
        void Recreate(unsigned number); 
        /// Przechodzi wszystkie afektory
        void SetPhysicalToAffectors(affector::IAffector *parent, const CPhysical *phys);

        SParticleSystemTemplate mParam; // zawiera mAffector
        emiter::IEmiter *mEmiter;

        CParticle *mParticles;
        bool mAutodel;
        State::SystemState mState;
        unsigned //mCount, 
            mAlive;
        float mDuration, mSystemAge;        
    };
};

#endif //__CPARTICLE_SYSTEM_H__//

