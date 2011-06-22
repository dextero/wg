#ifndef __CSUMMONCONTAINER_H__
#define __CSUMMONCONTAINER_H__

/**
 * Liosan, 07.11:
 * klasa pomocnicza, obslugujaca liste przywolanych przez aktora pomocnikow
 */

#include <string>
#include <vector>
#include "../../Utils/Memory/CSafePtr.h"

class CActor;
class CSummoner;

class CSummonContainer{
public:
    CSummonContainer(CActor *attached);
    ~CSummonContainer();

    struct SSummonedActor{
		Memory::CSafePtr<CActor> actor;
        CSummoner *summoner;
        float timeLeft;
		inline SSummonedActor(Memory::CSafePtr<CActor> actor, CSummoner *summ, float ttl):
		actor(actor){
            summoner = summ;
            timeLeft = ttl;
        }
    };
protected:
    CActor *mAttached;

    std::vector<SSummonedActor> mSummoned;
public:
    inline const std::vector<SSummonedActor> &GetSummonedActors(){ return mSummoned; }
    
    void RegisteredSummoned(const std::vector<Memory::CSafePtr<CActor> > &actors, CSummoner *summoner, float TTL);
};

#endif /* __CSUMMONCONTAINER_H__ */

