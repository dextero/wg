#include "CBoidChaserScheme.h"
#include "CActorAI.h"
#include "../CPlayerManager.h"
#include "../CPlayer.h"
#include "../CLair.h"
#include "../CEnemy.h"
#include "../../Utils/CRand.h"
#include "CActorAI.h"
#include "../../Utils/Maths.h"
#include "CAISchemeManager.h"
#include "../../Rendering/Primitives/CPrimitivesDrawer.h"
#include "../../Map/CMapManager.h"
#include "../../Map/SceneManager/CQuadTreeSceneManager.h"
#include "../PhysicalCategories.h"
#include <SFML/System/Vector2.hpp>

#include "../../Utils/HRTimer.h"

CBoidChaserScheme::CBoidChaserScheme():
mDefaultScheme(NULL),
mIdleScheme(NULL),
mDebug(false)
{
	mFlocks.push(&mBogusFlock);
}

CBoidChaserScheme::~CBoidChaserScheme() 
{
}

void CBoidChaserScheme::RegisterAI(Memory::CSafePtr<CActorAI> ai)
{
	mActors.push_back(ai);
	mBogusFlock.AddActor(ai); // TMP... albo i nie
}

void CBoidChaserScheme::UnregisterAI(Memory::CSafePtr<CActorAI> ai)
{
	// TODO: use a map for efficiency
    for(unsigned int i = 0; i < mActors.size(); /* opcjonalna inkrementacja */)
    {
		if (mActors[i] == ai){
			i++;
		} else {
			RemoveActor(i);
			break;
		}
    }
}

void CBoidChaserScheme::Update(float dt){

	//static const float MAINTENANCE_TIMER_THRESHOLD = 1.5f;
	std::vector<CActorFlock*> flocks = mFlocks.GetVector();
	for(unsigned int i = 0; i < flocks.size(); i++){
		UpdateFlock(dt,flocks[i]);
	}
	/*CActorFlock * top = mFlocks.top();
	if (top->GetMaintenanceTimer() > MAINTENANCE_TIMER_THRESHOLD){
		mFlocks.pop();
		if (top == &mBogusFlock){
			MaintainUnassigned();
		} else {
			MaintainAssigned(top);
		}
		top->ResetMaintenanceTimer();
		mFlocks.push(top);
	}*/

	for(unsigned int i = 0; i < mActors.size(); /* opcjonalna inkrementacja */)
	{
		if (mActors[i] == NULL){
			RemoveActor(i);
		} else {
			i++;
		}
	}
}

#define PI      3.1415926f
#define PI_2    1.5707963f
#define PI3_2   4.7123889f
#define PI2     6.2831853f

inline const std::vector<CActor*> CBoidChaserScheme::GetNeighbours(CActorAIData *ai, float dist, float angle)
{
    std::vector<CActor*> out;
	const std::vector<Memory::CSafePtr<CActorAI> > &actors = ai->GetFlock()->GetActors();
	std::vector<Memory::CSafePtr<CActorAI> >::const_iterator it;
	CActor *actor = ai->GetActor();

    for ( it = actors.begin(); it != actors.end(); ++it)
	{
		// rAum: dopisane by sie nie crashowalo
		if (((*it) != NULL)
			&& ((*it)->GetActor() != actor))
		{
			CActor *other = (*it)->GetActor();
			if (other->IsDead())
				continue;

            sf::Vector2f v = actor->GetVelocity();
			sf::Vector2f nv = other->GetVelocity(); // sasiad
            sf::Vector2f look = v - nv;

            if (Maths::LengthSQ(other->GetPosition() - actor->GetPosition()) > dist * dist)
                continue;

            float moveAngle = (v.x == 0 ? (v.y > 0 ? PI_2 : PI3_2) : atan(v.y / v.x));
            float lookAngle = (look.x == 0 ? (look.y > 0 ? PI_2 : PI3_2) : atan(look.y / look.x));

            if (fabsf(moveAngle - lookAngle) < angle)
                out.push_back(other);
        }
	}

    return out;
}

void CBoidChaserScheme::UpdateAI(CActorAIData *ai, float dt) 
{
	static const float DEFAULT_SCHEME_DIST_BONUS = 1.0f;
	CActorFlock *flock = ai->GetFlock();
	CActor *actor = ai->GetActor();
	sf::Vector2f waypoint = flock->GetData()->GetWaypoint();
	sf::Vector2f dir = Maths::Normalize(waypoint - flock->GetCenter());
	//src/Logic/AI/CBoidChaserScheme.cpp:125: warning: unused variable 'dist'
	//float dist = Maths::Length(actor->GetPosition() - flock->GetCenter());
	ai->ResetWaypoint();
	mDefaultScheme->UpdateAI(ai,dt);
	if (ai->GetWaypointSet()){
		Memory::CSafePtr<CActor> tgt = flock->GetData()->GetCombatTarget();
		if ((tgt != NULL) && (Maths::Length(actor->GetPosition() - tgt->GetPosition()) < DEFAULT_SCHEME_DIST_BONUS)){
			// brak akcji - zostajemy przy mDefaultScheme
			if (mDebug){
				gPrimitivesDrawer.DrawLine(gPrimitivesDrawer.GetClass("red-line"),actor->GetPosition(),ai->GetWaypoint(),dt / 2.0f);
			}
		} else {
			const std::vector<CActor*> &neighbours = GetNeighbours(ai, 2 * ai->GetActor()->GetStats()->GetBaseAspect(aRadius), PI3_2 / 2);
			if (neighbours.size() != 0){
				sf::Vector2f out = ai->GetWaypoint();
				Rule0_EatThePlayer(ai->GetAI(),out,0.4f);
				Rule1_MoveLikeOthers(ai->GetAI(),out,neighbours,0.2f);
				Rule2_BeInCenter(ai->GetAI(),out,neighbours,0.2f);
				Rule3_KeepSafeDistance(ai->GetAI(),out,neighbours,0.2f,2.5f * actor->GetCircleRadius());
				Rule4_AvoidObstacles(ai->GetAI(),out,0.2f,2.5f);
				ai->MoveTo(out, actor->GetCircleRadius());
			}
			if (mDebug){
				gPrimitivesDrawer.DrawLine(gPrimitivesDrawer.GetClass("blue-line"),actor->GetPosition(),ai->GetWaypoint(),dt / 2.0f);
			}
		}
	} else {
		const std::vector<CActor*> &neighbours = GetNeighbours(ai, 2 * ai->GetActor()->GetStats()->GetBaseAspect(aRadius), PI3_2 / 2);
		mIdleScheme->UpdateAI(ai,dt);
		if (neighbours.size() != 0){
			sf::Vector2f out = ai->GetWaypoint();
			Rule1_MoveLikeOthers(ai->GetAI(),out,neighbours,0.2f);
			Rule2_BeInCenter(ai->GetAI(),out,neighbours,0.2f);
			Rule3_KeepSafeDistance(ai->GetAI(),out,neighbours,0.2f,2.5f * actor->GetCircleRadius());
			ai->MoveTo(out, actor->GetCircleRadius());
		}
		if (mDebug){
			gPrimitivesDrawer.DrawLine(actor->GetPosition(),ai->GetWaypoint(),dt / 2.0f);
		}
	}
}

std::string CBoidChaserScheme::GetName() const
{
    return "CBoidChaserScheme";
}

void CBoidChaserScheme::Init(CAISchemeManager &sm){
	mDefaultScheme = sm.GetScheme(L"melee-chaser");
	mIdleScheme = sm.GetScheme(L"idle-movement");
}

void CBoidChaserScheme::RemoveActor(int index){
	std::swap(mActors[index],mActors[mActors.size()-1]);
	mActors.pop_back();
}

void CBoidChaserScheme::UpdateFlock(float dt, CActorFlock *flock){
	flock->Update(dt);
	mDefaultScheme->UpdateAI(flock->GetData(),dt);
	//if (mDebug){
	//	gPrimitivesDrawer.DrawCircle(flock->GetCenter(),flock->GetAvgDistance(),dt);
	//	gPrimitivesDrawer.DrawLine(flock->GetCenter(),flock->GetData()->GetWaypoint(),dt);
	//}
}

void CBoidChaserScheme::Rule0_EatThePlayer(CActorAI *ai, sf::Vector2f& out, float factor)
{
    CActor *actor = ai->GetActor();
	sf::Vector2f vec = Maths::Normalize(actor->GetPosition() - ai->GetData()->GetWaypoint());
	out += factor * vec * actor->GetStats()->GetCurrAspect(aSpeed);
}

void CBoidChaserScheme::Rule1_MoveLikeOthers(CActorAI *ai, sf::Vector2f& out, 
											  const std::vector<CActor*> &neighbours, float factor)
{
    sf::Vector2f avg = Maths::VectorZero();
	CActor *actor = ai->GetActor();
    for (unsigned int i = 0; i < neighbours.size(); i++)
        avg += neighbours[i]->GetVelocity();
    avg /= (float)neighbours.size();
    
    out += factor * (avg - actor->GetVelocity());
}

void CBoidChaserScheme::Rule2_BeInCenter(CActorAI *ai, sf::Vector2f& out, 
										 const std::vector<CActor*> &neighbours, float factor)
{
    float avgDist = 0.f;
	CActor *actor = ai->GetActor();
	for (unsigned int i = 0; i < neighbours.size(); i++){
		if (neighbours[i] != actor)
			avgDist += Maths::Length(neighbours[i]->GetPosition() - actor->GetPosition());
	}
    avgDist /= (float)neighbours.size();

    for (unsigned int i = 0; i < neighbours.size(); i++)
    {
		if (neighbours[i] != actor){
			float dist = Maths::Length(neighbours[i]->GetPosition() - actor->GetPosition());
			out += factor * (((neighbours[i]->GetPosition() - actor->GetPosition()) * (dist - avgDist)) / dist);
			if (out.x != out.x){
				out.x += 2;
			}
		}
    }
}

void CBoidChaserScheme::Rule3_KeepSafeDistance(CActorAI *ai, sf::Vector2f& out, 
											   const std::vector<CActor*> &neighbours, float factor, float minDist)
{
	CActor *actor = ai->GetActor();
	for (unsigned int i = 0; i < neighbours.size(); i++)
    {
		CActor *flockActor = neighbours[i];
		if (flockActor != actor){
			sf::Vector2f delta = flockActor->GetPosition() - actor->GetPosition();
			out -= factor * (((delta * minDist) / Maths::Length(flockActor->GetPosition() - actor->GetPosition())) - delta);
		}
    }
}

void CBoidChaserScheme::Rule4_AvoidObstacles(CActorAI* ai, sf::Vector2f& out, float factor, float minDist){
	CActor *actor = ai->GetActor();
	mWork.clear();
	int filter = PHYSICAL_PATHBLOCKER;
	gMapManager.GetSceneManager().GetPhysicalsInRadius(actor->GetPosition(),actor->GetCircleRadius() * minDist,filter,mWork);
	for (unsigned int i = 0; i < mWork.size(); i++)
    {
		CPhysical *phys = mWork[i];
		if (phys != actor){
			sf::Vector2f delta = phys->GetPosition() - actor->GetPosition();
			float bonus = 1.0f;
			if (Maths::Length(delta) < actor->GetCircleRadius() * minDist / 2.0f) bonus = 1.5f;
			out -= factor * bonus * (((delta * minDist) / Maths::Length(phys->GetPosition() - actor->GetPosition())) - delta);
		}
    }
}
