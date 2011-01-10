#ifndef CACTOR_FLOCK_H_
#define CACTOR_FLOCK_H_

#include <SFML/System/Vector2.hpp>
#include <vector>

#include "CActorAI.h"
#include "CAIScheme.h"

class CActorAI;


class CActorFlock{
private:
	CActorAIData mData;
	std::vector<Memory::CSafePtr<CActorAI> > mActors;

	sf::Vector2f mCenter;
	float mAvgDistance;

	float mMaintenanceTimer;
public:
	CActorFlock();
	~CActorFlock();

	// adds a new actor; doesn't recalculate params
	void AddActor(Memory::CSafePtr<CActorAI> actor);
	// removes actors straying to far away from the center
	void Trim();
	// splits the flock into two, returns the new flock
	// calls RecalculateParams()
	CActorFlock *Split();
	// merges two flocks into this one; destroys the other
	void Merge(CActorFlock *other);

	// calls recalculate params
	void Update(float dt);

	inline CActorAIData *GetData()							{ return &mData; }
	inline const std::vector<Memory::CSafePtr<CActorAI> > &GetActors() const	{ return mActors; }
	inline unsigned int GetActorCount() const				{ return mActors.size(); }

	inline const sf::Vector2f &GetCenter() const			{ return mCenter; }
	inline float GetAvgDistance() const						{ return mAvgDistance; }

	inline float GetMaintenanceTimer() const				{ return mMaintenanceTimer; }
	inline void ResetMaintenanceTimer()						{ mMaintenanceTimer = 0.0f; }
private:	
	// recalculate center and avgdistance
	void RecalculateParams();
	void RemoveActor(int index);
};

#endif // CACTOR_FLOCK_H_
