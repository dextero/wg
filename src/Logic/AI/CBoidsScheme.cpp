#include "CBoidsScheme.h"
#include "../../Utils/MathsFunc.h"
#include "CActorAI.h"

#define PI      3.1415926f
#define PI_2    1.5707963f
#define PI3_2   4.7123889f
#define PI2     6.2831853f

std::vector<CActor*> CBoidsScheme::GetNeighbours(CActor* actor, float dist, float angle)
{
    std::vector<CActor*> out;

    for (ActorIterator it = mActors.begin(); it != mActors.end(); ++it)
        if (*it != actor)
        {
            sf::Vector2f v = actor->GetVelocity();
            sf::Vector2f nv = (*it)->GetVelocity(); // sasiad
            sf::Vector2f look = v - nv;

            if (Maths::LengthSQ((*it)->GetPosition() - actor->GetPosition()) > dist * dist)
                continue;

            float moveAngle = (v.x == 0 ? (v.y > 0 ? PI_2 : PI3_2) : atan(v.y / v.x));
            float lookAngle = (look.x == 0 ? (look.y > 0 ? PI_2 : PI3_2) : atan(look.y / look.x));

            if (fabsf(moveAngle - lookAngle) < angle)
                out.push_back(*it);
        }

    return out;
}

inline void CBoidsScheme::Rule1_MoveLikeOthers(CActor* actor, sf::Vector2f& out, std::vector<CActor*>& neighbours, float factor)
{
    sf::Vector2f avg = Maths::VectorZero();
    for (ActorIterator it = neighbours.begin(); it != neighbours.end(); ++it)
        avg += (*it)->GetVelocity();
    avg /= (float)neighbours.size();
    
    out += factor * (avg - actor->GetVelocity());
}

inline void CBoidsScheme::Rule2_BeInCenter(CActor* actor, sf::Vector2f& out, std::vector<CActor*>& neighbours, float factor)
{
    float avgDist = 0.f;
    for (ActorIterator it = neighbours.begin(); it != neighbours.end(); ++it)
        avgDist += Maths::Length((*it)->GetPosition() - actor->GetPosition());
    avgDist /= (float)neighbours.size();

    for (ActorIterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
        float dist = Maths::Length((*it)->GetPosition() - actor->GetPosition());
        out += factor * ((((*it)->GetPosition() - actor->GetPosition()) * (dist - avgDist)) / dist);
    }
}

inline void CBoidsScheme::Rule3_KeepSafeDistance(CActor* actor, sf::Vector2f& out, std::vector<CActor*>& neighbours, float factor, float minDist)
{
    for (ActorIterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
        sf::Vector2f delta = (*it)->GetPosition() - actor->GetPosition();
        out -= factor * (((delta * minDist) / Maths::Length((*it)->GetPosition() - actor->GetPosition())) - delta);
    }
}

// -------------------------------------------

void CBoidsScheme::RegisterAI(CActorAI *ai)
{
    mActors.push_back(ai->GetActor());
}

void CBoidsScheme::UnregisterAI(CActorAI *ai)
{
    for (ActorIterator it = mActors.begin(); it != mActors.end(); ++it)
        if (*it == ai->GetActor())
        {
            mActors.erase(it);
            return;
        }
}

void CBoidsScheme::UpdateAI(CActorAIData *ai, float dt)
{
    sf::Vector2f v = ai->GetActor()->GetVelocity();
    std::vector<CActor*> neighbours = GetNeighbours(ai->GetActor(), ai->GetActor()->GetStats()->GetBaseAspect(aRadius), PI3_2 / 2);

    Rule1_MoveLikeOthers(ai->GetActor(), v, neighbours, 0.1f);
    Rule2_BeInCenter(ai->GetActor(), v, neighbours, 0.1f);
    Rule3_KeepSafeDistance(ai->GetActor(), v, neighbours, 0.1f, ai->GetActor()->GetStats()->GetBaseAspect(aRadius));

    ai->MoveTo(ai->GetActor()->GetPosition() + v);
}

void CBoidsScheme::Init(CAISchemeManager &sm)
{
}
