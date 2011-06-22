#ifndef __CSUMMONER_H__
#define __CSUMMONER_H__

/**
 * Liosan, 07.11:
 * klasa pomocnicza, obslugujaca przywolywanie potworow na podstawie konfiguracji
 */

#include <SFML/System/Vector2.hpp>
#include "../Effects/CComputedValue.h"
#include "../Effects/CExecutionContext.h"
#include "IDefferedLoadable.h"
#include <vector>
#include "../../Utils/Memory/CSafePtr.h"

class CActor;
class CActorTemplate;

class CSummoner : public IDefferedLoadable{ 
private:
    Memory::CSafePtr<CActor> CreateSummoned(sf::Vector2f pos);

    CActorTemplate *mTemplate;
    CComputedValue mMinRadius;
    CComputedValue mCount;
    CComputedValue mTTL;

    inline int FillGrid(sf::Vector2f pos, const std::vector<CPhysical* > &phys, 
        std::vector<bool> &grid,float nodeSize, float radius, float minRange);
    inline int GridSize(float nodeSize, float radius);
    inline sf::Vector2f GridToPosition(float nodeSize, float radius, int index);
public:
    CSummoner();
    ~CSummoner();

    virtual void Load(CXml &xml, rapidxml::xml_node<char> *root = 0);

    // zwraca liczbe przyzwanych potworow
    int Summon(CPhysical *caster, ExecutionContextPtr context);

    inline const CComputedValue &GetTTL(){ return mTTL; }
};

#endif /* __CSUMMONER_H__ */

