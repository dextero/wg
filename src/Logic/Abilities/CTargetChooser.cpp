#include "CTargetChooser.h"
#include "../CDetector.h"
#include "../CPhysicalManager.h"
#include "../CPhysical.h"
#include "../PhysicalCategories.h"
#include "../../Utils/CRand.h"
#include "../../Utils/Directions.h"
#include "../../Utils/Maths.h"
#include "../../Map/CMapManager.h"
#include "../../Map/SceneManager/CQuadTreeSceneManager.h"
#include "../Effects/CEffectManager.h"
#include "../Effects/CComputedValueFactory.h"
#include "../Effects/CExecutionContext.h"
#include <algorithm>

CTargetChooser::CTargetChooser() :
    filter(0),
	aimAtGround(0),
	count(1),
	deadBodies(0)
{
    ;
}

CTargetChooser::~CTargetChooser(){
}

void CTargetChooser::Load(CXml &xml, rapidxml::xml_node<> *node){
	xml_node<> *n;

    filter = ParsePhysicalFilter(xml.GetString(xml.GetChild(node,"filter"),"value"));
	n = xml.GetChild(node,"range");
	if (n)
		range = CComputedValueFactory::Parse(xml,n);
	n = xml.GetChild(node,"angle");
	if (n)
		angle = CComputedValueFactory::Parse(xml,n);
	n = xml.GetChild(node,"count");
	if (n)
		count = CComputedValueFactory::Parse(xml,n);


	n = xml.GetChild(node,"aim-at-ground");
	if (n)
		aimAtGround = xml.GetInt(n,"value");

	n = xml.GetChild(node,"dead-bodies");
	if (n)
		deadBodies = xml.GetInt(n,"value");
}

void CTargetChooser::Perform(CPhysical *emiter, int offset, EffectSourcePtr source, ExecutionContextPtr context){
	std::vector<CPhysical *> result;
    ChooseMultiple(emiter, result, context);
    if ((emiter != NULL) && (source != NULL)){
        // trzeba podac explicite typ
        source = (new CEffectSource(estGenericPhysical,emiter))->SetSecondary(source);
    }
	int performed = 0;
	//warning C4244: 'argument' : conversion from 'int' to 'float', possible loss of data
	int c = (int)count.Evaluate(context);

	if ((result.size()!=0) && (offset != -1)){
        //fprintf(stderr,"chosen target at %f,%f; detector is at %f,%f\n",chosen->GetPosition().x,
        //    chosen->GetPosition().y,GetPosition().x,GetPosition().y);
		for (unsigned int i = 0; i < result.size(); i++){
			gEffectManager.ApplyOnce(result[i], offset, source, context);
			performed++;
		}
    }
	if (performed < c) {
        if ((offset!=-1)&&(GetAimAtGround())){
            bool ok;
			for ( ; performed < c; performed++){
				sf::Vector2f pos = ChoosePos(emiter,context,ok);
				if (ok){
					//fprintf(stderr,"chosen target is NULL, shooting ground at %f,%f, detector is at %f,%f\n",
					//   pos.x,pos.y,GetPosition().x,GetPosition().y);
					gEffectManager.PerformAt(pos,offset,source,context );
				} // else fprintf(stderr,"chosen target is NULL, detector is at %f,%f\n",GetPosition().x,GetPosition().y);
			}
        }
    }
}

CPhysical *CTargetChooser::Choose(CPhysical *emiter, ExecutionContextPtr context){
    work.clear();
	gMapManager.GetSceneManager().GetPhysicalsInRadius(emiter->GetPosition(),range.Evaluate(context),filter,emiter->GetSideAndCategory().side, work); 
	std::vector<CPhysical*> result;
    ChooseFromList(emiter,work,result,emiter,context);
	if (result.size() == 0)
		return NULL;
	else
		return result[0];
}

void CTargetChooser::ChooseMultiple(CPhysical *emiter, std::vector<CPhysical*> &result, ExecutionContextPtr context){
    work.clear();
	gMapManager.GetSceneManager().GetPhysicalsInRadius(emiter->GetPosition(),range.Evaluate(context),filter,emiter->GetSideAndCategory().side, work); 
    ChooseFromList(emiter,work,result,emiter,context);
}

struct DistanceToEmiterComparator{
	CPhysical *emiter;
	bool operator()	( CPhysical *a, CPhysical *b){
		float distA = Maths::Length(emiter->GetPosition() - a->GetPosition());
		float distB = Maths::Length(emiter->GetPosition() - b->GetPosition());
		return distA < distB;
	}
};

void CTargetChooser::ChooseFromList(CPhysical *emiter, const std::vector<CPhysical*> &observed, std::vector<CPhysical*> &result, CPhysical *taboo, ExecutionContextPtr context){
	result.clear();
	for (unsigned int i = 0; i < observed.size(); i++){
		// eliminacja taboo-aktora
        if (observed[i]==taboo)
			continue;
		// eliminacja martwych aktorow, jesli nie wybieramy trupow jako cele
        if ((deadBodies == 0) && (observed[i]->IsDead()))
            continue;
		// eliminacja zywych aktorow, jesli wybieramy trupy jako cele
        if ((deadBodies != 0) && (!(observed[i]->IsDead())))
            continue;
		// eliminacja celow spoza kata
		//warning C4244: 'argument' : conversion from 'int' to 'float', possible loss of data
		float obsAngle = Maths::AngleBetween(RotationToVector((float)emiter->GetRotation()), observed[i]->GetPosition() - emiter->GetPosition());
		if (Maths::Abs(obsAngle) > angle.Evaluate(context))
			continue;
		result.push_back(observed[i]);
	}
    DistanceToEmiterComparator comp;
	comp.emiter = emiter;
	std::sort(result.begin(), result.end(), comp);
	//warning C4244: 'initializing' : conversion from 'float' to 'int', possible loss of data
	//warning C4018: '>=' : signed/unsigned mismatch
	unsigned int c = (unsigned int)count.Evaluate(context);
	if (result.size() >= c)
		result.erase(result.begin() + c,result.end());
}

sf::Vector2f CTargetChooser::ChoosePos(CPhysical *emiter, ExecutionContextPtr context, bool &outOK){
    if (emiter==NULL){
        outOK = false;
        return sf::Vector2f();
    }
    float rnd = gRand.Rndf(0.5f);
    fprintf(stderr, "rnd = %f", rnd);
    outOK = true;
	float a = angle.Evaluate(context);
    int rot = (int)(emiter->GetRotation() + a * rnd - a * 0.5f);
    if (rot >= 360) rot -= 360;
    if (rot < 0) rot += 360;
	float r = range.Evaluate(context);
	float dist = 0.85f * r + 0.4f * r * rnd;
    fprintf(stderr, "dist = %f\n", dist);
	sf::Vector2f v = RotationToVector((float)rot)*dist;
	float x = emiter->GetPosition().x + v.x;
	float y = emiter->GetPosition().y + v.y;
	sf::Vector2f result(x,y);
	return result;
}
