#include "CParticleSystem.h"
#include "CParticlePool.h"
#include "Renderers/CSpriteRender.h"
#include "Affectors/CPointMassAffector.h"

namespace part
{

void CParticleSystem::Recreate(unsigned count)
{
    if (mParticles)
        gParticlePool.Release(mParticles, mParam.mCount);

    mAlive = 0;

    mParticles = gParticlePool.CreateParticles(mParam.mCount);

    mParam.mCount = count;
}
//----------------------
void CParticleSystem::Clean()
{
    if (mParticles) {
        gParticlePool.Release(mParticles, mParam.mCount);
        mParticles = 0;
    }

    if (mParam.mAffector && mAutodel)
        delete mParam.mAffector;   
}
//----------------------
void CParticleSystem::Draw()
{
    assert(mParticles != NULL);
    if (mState != State::Hidden) {
        renderer::gSpriteRenderer.SetBlendMode(mParam.mBlendMode);
        renderer::gSpriteRenderer.Render(mParticles,mParam.mCount);
    }
}
//----------------------
void CParticleSystem::Update(float dt)
{
    assert(mParticles != NULL);
    assert(mEmiter != NULL);
    assert(mParam.mAffector != NULL);
    unsigned int dead(0);

    switch (mState)
    {
    case State::Normal:
        mSystemAge += dt;

        if (mSystemAge > mDuration)
            mState = State::ToDelete;

        mAlive = mEmiter->Emit(mParticles,mAlive,mParam.mCount,dt);

    case State::ToDelete:
        
        for (unsigned i=0; i<mParam.mCount; ++i)
        {
            if (mParticles[i].life <= 0.f) {
                ++dead;
                continue;
            }

            mParticles[i].Process(dt);
            mParam.mAffector->Modify(mParticles[i],dt);
        }

        if ((dead == mParam.mCount) && (mAlive != 0))
            mState = State::CanDelete;

    case State::Freeze:
    case State::CanDelete:
    default:
        return;
    }
}

void CParticleSystem::SetPhysicalToAffectors(affector::IAffector *parent, const CPhysical *phys)
{
       
}

void CParticleSystem::AssignPhysical(const CPhysical *phys)
{
    affector::IAffector *it = mParam.mAffector;
    if (it == NULL)
        return;

    if (it->AffectorName() == "PointMass")
            reinterpret_cast<affector::CPointMass *>(it)->AddPhysical(phys);  
}

};
