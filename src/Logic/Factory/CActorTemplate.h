/* Generyczny wzorzec aktora - zawiera wspolne elementy dla gracza, potwora itp */

#ifndef _ACTOR_TEMPLATE_H_
#define _ACTOR_TEMPLATE_H_

#include "CPhysicalTemplate.h"
#include "../CEnemy.h"
#include <string>
#include <vector>

class CXml;
class CAbility;
class CAnimSet;
struct SAbilityInstance;
class CStats;

namespace System { namespace Resource {
	class CSound;
}}

class CActorTemplate : public CPhysicalTemplate
{
public:
    CActorTemplate(): mMinXPValue(0.f), mMaxXPValue(0.f), mMinStats(NULL), mMaxStats(NULL), mAnimations(NULL), mDefaultAbility(0) {} 
	virtual ~CActorTemplate() { }

	inline float GetMaxRadius() { return mMaxRadius; }

	/* IResource */

	std::string GetType();

    // CActorTemplate::Load zaladuje wspolne cechy aktorow
	bool Load(CXml &xml);
	void Drop();

	// Metoda do przypisywania obiektowi CDisplayable domyslnego sprite'a badz animacji
	// co ma zastosowanie w edytorze
	virtual void PrepareEditorPreviewer( CDisplayable* d );

protected:
    // wypelnij aktora wartosciami (skalowanymi f: 0.0..1.0)
    virtual void Fill(CActor *actor, float f);

    float mMinXPValue,   mMaxXPValue;
    CStats *mMinStats,*mMaxStats;
    int mNoStun;

    std::vector<CAnimSet*> mAvailableAnimations;
    CAnimSet *mAnimations;

    std::vector<SAbilityInstance> mAbilities;
    std::vector<std::string> mAbilityAnims;
    int mDefaultAbility;

    std::map< std::string, std::vector<System::Resource::CSound*> > mSound;
public:
    virtual CTemplateParam *ReadParam(CXml &xml, xml_node<> *node, CTemplateParam *orig = NULL);
    virtual void Parametrise(CPhysical *actor,CTemplateParam *param);
    virtual void SerializeParam(std::ostream &out, CTemplateParam *param);

    inline const std::vector<CAnimSet*> &GetAvailableAnims() { return mAvailableAnimations; }
    const std::wstring GetStatsWString();
};

#endif // _ACTOR_TEMPLATE_H_

