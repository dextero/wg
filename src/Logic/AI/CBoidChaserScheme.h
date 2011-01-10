#ifndef _BOID_CHASER_SCHEME_H_
#define _BOID_CHASER_SCHEME_H_

#include <queue>
#include <vector>
#include "CAIScheme.h"
#include "CActorFlock.h"

class CBoidChaserScheme: public CAIScheme
{
protected:
	struct CActorFlockComparison{
		bool operator() (CActorFlock *lhs, CActorFlock *rhs) const {
			return (lhs->GetMaintenanceTimer() < rhs->GetMaintenanceTimer());
		}
	};
	// nie pytajcie
	class CFlockQueue : public std::priority_queue<CActorFlock*,std::vector<CActorFlock*>,CActorFlockComparison> {
	public:
		inline std::vector<CActorFlock*> &GetVector()	{ return c; }
	};
	CFlockQueue mFlocks;
	std::vector<Memory::CSafePtr<CActorAI> > mActors;
	// TODO: quad-tree for actors and flocks

	// scheme to use if (a) an actor hasn't got a flock (b) an actor is close to the player
	CAIScheme *mDefaultScheme;
	CAIScheme *mIdleScheme;

	// puste stado; gdy pojawi sie na czubku mFlocks, przeliczamy 
	// nieprzypisane stwory
	CActorFlock mBogusFlock;

	bool mDebug;
	std::vector<CPhysical*> mWork;
public:
    CBoidChaserScheme();
    virtual ~CBoidChaserScheme();
    
    virtual void RegisterAI(Memory::CSafePtr<CActorAI> ai);
    virtual void UnregisterAI(Memory::CSafePtr<CActorAI> ai);
	
	virtual void Update(float dt);
    virtual void UpdateAI(CActorAIData *ai, float dt);

    virtual std::string GetName() const;    
	virtual void Init(CAISchemeManager &sm);

	inline void SetDebug(bool debug)	{ mDebug = debug; }
protected:
	void RemoveActor(int index);
	void UpdateFlock(float dt, CActorFlock *flock);
	inline const std::vector<CActor*> GetNeighbours(CActorAIData *ai, float dist, float angle);

	void Rule0_EatThePlayer(CActorAI* ai, sf::Vector2f& out, float factor);
	void Rule1_MoveLikeOthers(CActorAI* ai, sf::Vector2f& out, const std::vector<CActor*> &neighbours, float factor);
	void Rule2_BeInCenter(CActorAI* ai, sf::Vector2f& out, const std::vector<CActor*> &neighbours, float factor);
	void Rule3_KeepSafeDistance(CActorAI* ai, sf::Vector2f& out, const std::vector<CActor*> &neighbours, float factor, float minDist);
	void Rule4_AvoidObstacles(CActorAI* ai, sf::Vector2f& out, float factor, float minDist);
};

#endif /*_BOID_CHASER_SCHEME_H_*/
