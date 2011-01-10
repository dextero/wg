#include "CSummonContainer.h"
#include "../CActor.h"
#include <cassert>
#include "CSummoner.h"
#include <cstdio>

CSummonContainer::CSummonContainer(CActor *attached) :
    mAttached(attached)
{
}

CSummonContainer::~CSummonContainer(){
    for (unsigned int i = 0; i < mSummoned.size(); i++){
		if (mSummoned[i].actor != NULL)
			mSummoned[i].actor->MarkForDelete();
    }
}

void CSummonContainer::RegisteredSummoned(const std::vector<Memory::CSafePtr<CActor> > &actors, CSummoner *summoner, float TTL){
    if (summoner == NULL){
        fprintf(stderr,"CSummonContainer::RegisterSummoned: summoner is NULL, aborting\n");
        return;
    }
    // usuwamy physicale przywolane przez ten CSummoner
    for (unsigned int i = 0; i < mSummoned.size(); /* selektywna inkrementacja */ ){
        if (mSummoned[i].summoner == summoner){
			if (mSummoned[i].actor != NULL)
				mSummoned[i].actor->MarkForDelete();
            mSummoned[i] = mSummoned.back();
            mSummoned.pop_back();
        } else i++;
    }
    for (unsigned int i = 0; i < actors.size(); i++){
        mSummoned.push_back(SSummonedActor(actors[i],summoner,TTL));
    }
}