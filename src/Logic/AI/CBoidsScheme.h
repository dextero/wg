#ifndef __CBOIDGROUP_H__
#define __CBOIDGROUP_H__

#include "../CActor.h"
#include "CAIScheme.h"

#include <vector>

/*
    dex: niepodpiete, niesprawdzone
*/
class CBoidsScheme: public CAIScheme
{
private:
    std::vector<CActor*> mActors;
    typedef std::vector<CActor*>::iterator ActorIterator;

    std::vector<CActor*> GetNeighbours(CActor* actor, float dist, float angle); /* dist - zasieg; angle - 1/2 kata widzenia */

    inline void Rule1_MoveLikeOthers(CActor* actor, sf::Vector2f& out, std::vector<CActor*>& neighbours, float factor);
    inline void Rule2_BeInCenter(CActor* actor, sf::Vector2f& out, std::vector<CActor*>& neighbours, float factor);
    inline void Rule3_KeepSafeDistance(CActor* actor, sf::Vector2f& out, std::vector<CActor*>& neighbours, float factor, float minDist);

public:    
    void RegisterAI(CActorAI *ai);
    void UnregisterAI(CActorAI *ai);

    virtual void UpdateAI(CActorAIData *ai, float dt);
    std::string GetName() const { return "CBoidGroup"; };

	void Init(CAISchemeManager &sm);
};

#endif // __CBOIDGROUP_H__ //
