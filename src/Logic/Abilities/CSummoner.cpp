#include <cassert>
#include "CSummoner.h"
#include "../CActor.h"
#include "../../Utils/CXml.h"
#include "../Effects/CComputedValueFactory.h"
#include "../../Map/SceneManager/CQuadTreeSceneManager.h"
#include "../../Map/CMapManager.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../Factory/CActorTemplate.h"
#include "../../Utils/MathsFunc.h"
#include <vector>
#include <algorithm>

CSummoner::CSummoner():mTemplate(NULL){}

CSummoner::~CSummoner(){}

Memory::CSafePtr<CActor> CSummoner::CreateSummoned(sf::Vector2f pos){
	Memory::CSafePtr<CActor> result = Memory::CSafePtr<CActor>::NullPtr();
    if (mTemplate){
        CPhysical *phys = mTemplate->Create();
		result = dynamic_cast<CActor*>(phys)->GetSafePointer();
        result->SetPosition(pos);
    }
    return result;
}

int CSummoner::FillGrid(sf::Vector2f pos, const std::vector<CPhysical* > &physicals, std::vector<bool> &grid,float nodeSize, float radius, float minRange){
    int gridSize = GridSize(nodeSize,radius);
    grid.clear();
    int result = 0;
    for (int i = 0; i < gridSize*gridSize; i++){
        sf::Vector2f gridPos = GridToPosition(nodeSize,radius,i);
        grid.push_back((Maths::Length(gridPos) <= radius) && (Maths::Length(gridPos) >= minRange));
        if (grid.back()){
            for (unsigned int j = 0; j < physicals.size(); j++){
                if (Maths::Length(physicals[j]->GetPosition() - (pos + gridPos)) < physicals[j]->GetCircleRadius()){
                    grid.back() = false;
                    break;
                }
            }
            if (grid.back()) result++;
        }
    }
    return result;
}

int CSummoner::GridSize(float nodeSize, float radius){
    return (int)(radius / nodeSize + 1.0f);
}

sf::Vector2f CSummoner::GridToPosition(float nodeSize, float radius, int index){
    int gridSize = GridSize(nodeSize,radius);
    int x = index % gridSize, y = index / gridSize;
    sf::Vector2f topLeft(- gridSize * nodeSize / 2.0f, - gridSize * nodeSize / 2.0f);
    return topLeft + sf::Vector2f((x + 0.5f) * nodeSize, (y + 0.5f) * nodeSize);
}

void CSummoner::Load(CXml &xml, rapidxml::xml_node<char> *root){
	mTTL = CComputedValueFactory::Parse(xml,xml.GetChild(root,"ttl"),10000.0f);
	mMinRadius = CComputedValueFactory::Parse(xml,xml.GetChild(root,"min-radius"),10000.0f);
    mCount = CComputedValueFactory::Parse(xml,xml.GetChild(root,"count"),10000.0f);

    std::string templ = xml.GetString(xml.GetChild(root,"template"),"value");
    mTemplate = dynamic_cast<CActorTemplate*>(gResourceManager.GetPhysicalTemplate(templ));
}

int CSummoner::Summon(CPhysical *caster, ExecutionContextPtr context){
	std::vector<CPhysical *> physicals;
    std::vector<bool> grid;
    int count = (int)mCount.Evaluate(context), freeSpots = -1;
    float ttl = mTTL.Evaluate(context);
    float minRadius = mMinRadius.Evaluate(context);
    float summRadius = mTemplate->GetMaxRadius(), range;
    sf::Vector2f pos;
    // iteracyjnie wybieramy miejsca, gdzie mozna przywolac potwory
    for (int iter = 0; iter < 4; iter++){
        range = 1.0f + 1.0f * iter;
        if (range < minRadius)
            continue;
        gMapManager.GetSceneManager().GetPhysicalsInRadius(caster->GetPosition(),range,PHYSICAL_SUMMONBLOCKER,physicals);
        // pokrywamy otoczenie siatka
        pos = caster->GetPosition();
        freeSpots = FillGrid(pos,physicals,grid,summRadius,range,minRadius);
        if (freeSpots >= count)
            break;
        grid.clear();
        // jeszcze raz, tylko z przesunieciem
        pos = caster->GetPosition() + sf::Vector2f(summRadius/2.0f,summRadius/2.0f);
        freeSpots = FillGrid(pos,physicals,grid,summRadius,range,minRadius);
        if (freeSpots >= count)
            break;
        grid.clear();
    }
    std::vector<Memory::CSafePtr<CActor> > work;
    // faktyczne przywolanie
    if (freeSpots > 0){
        int gridSize = GridSize(summRadius,range);
        assert(gridSize > 0);
        if (count > freeSpots) count = freeSpots;
        // znajdujemy wolne miejsca na siatce
        std::vector<int> gridIndexes;
        for (unsigned int i = 0; i < grid.size(); i++)
            if (grid[i])
                gridIndexes.push_back(i);
        assert((int)gridIndexes.size() == freeSpots);
        std::random_shuffle(gridIndexes.begin(), gridIndexes.end());
        // tworzymy physicale, wykorzystujac wolne miejsca z siatki
        for (int i = 0; i < count; i++){
            sf::Vector2f pos = caster->GetPosition() + GridToPosition(summRadius,range,gridIndexes[i]);
			Memory::CSafePtr<CActor> actor = CreateSummoned(pos);
            if (actor != NULL)
                work.push_back(actor);
        }
    }
    caster->GetSummonContainer()->RegisteredSummoned(work,this,ttl);
    return (int)(work.size());
}