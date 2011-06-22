#include "CActorFlock.h"
#include "../CActor.h"
#include "../../Utils/Maths.h"

CActorFlock::CActorFlock():
mData(NULL),
mCenter(sf::Vector2f(0.0f,0.0f)),
mAvgDistance(0.0f),
mMaintenanceTimer(0.0f)
{
	mData.SetFlock(this);
}

CActorFlock::~CActorFlock(){
	for (unsigned int i = 0; i < mActors.size(); i++){
		mActors[i]->GetData()->SetFlock(NULL);
	}
}

void CActorFlock::AddActor(Memory::CSafePtr<CActorAI> actor){
	mActors.push_back(actor);
	actor->GetData()->SetFlock(this);
}

void CActorFlock::Trim(){
	static const int DISTANCE_FACTOR = 3;
	for(unsigned int i = 0; i < mActors.size(); /* opcjonalna inkrementacja */)
    {
		sf::Vector2f pos = mActors[i]->GetActor()->GetPosition();
		if (Maths::Length(pos - mCenter) > DISTANCE_FACTOR * mAvgDistance){
			i++;
		} else {
			RemoveActor(i);
		}
    }
}

CActorFlock* CActorFlock::Split(){
	return NULL;
}

void CActorFlock::Merge(CActorFlock *other){
	std::vector<Memory::CSafePtr<CActorAI> > otherActors = other->mActors;
	for (unsigned int i = 0; i < otherActors.size(); i++){
		AddActor(otherActors[i]);
	}
	otherActors.clear();
	delete other;
	RecalculateParams();
}

void CActorFlock::Update(float dt){
	mMaintenanceTimer += dt;
	for(unsigned int i = 0; i < mActors.size(); /* opcjonalna inkrementacja */)
    {
		if (mActors[i] == NULL){
			RemoveActor(i);
		} else {
			i++;
		}
    }
	RecalculateParams();
}

void CActorFlock::RecalculateParams(){
	// srodek
    mCenter = sf::Vector2f(0.0f, 0.0f);
    for(unsigned int i = 0; i < mActors.size(); i++)
    {
		mCenter += mActors[i]->GetActor()->GetPosition();
    }
    mCenter.x /= mActors.size();
    mCenter.y /= mActors.size();

	// srednia odleglosc
	mAvgDistance = 0.0f;
    for(unsigned int i = 0; i < mActors.size(); i++)
    {
		mAvgDistance += Maths::Length(mActors[i]->GetActor()->GetPosition() - mCenter);
    }
	mAvgDistance /= mActors.size() * 2;
}

void CActorFlock::RemoveActor(int index){
	std::swap(mActors[index],mActors[mActors.size()-1]);
	mActors.pop_back();
}
