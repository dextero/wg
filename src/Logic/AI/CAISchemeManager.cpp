#include <cstdio>

#include "CAISchemeManager.h"

#include "CWandererScheme.h"
#include "CStupidChaserScheme.h"
#include "CQuickChaserScheme.h"
#include "CLairGuardianScheme.h"
#include "CRangedCasterScheme.h"
#include "CMeleeChaserScheme.h"
#include "CBoidChaserScheme.h"
#include "CGenericCombatScheme.h"
#include "CIdleMovementScheme.h"
#include "CTurningScheme.h"
#include "../../CGame.h"

template<> CAISchemeManager* CSingleton<CAISchemeManager>::msSingleton = 0;

CAISchemeManager::CAISchemeManager()
{
    fprintf(stderr,"CAISchemeManager::CAISchemeManager()\n");
    
    RegisterScheme(L"wanderer",new CWandererScheme());
    RegisterScheme(L"stupidchaser",new CStupidChaserScheme());
	RegisterScheme(L"quickchaser", new CQuickChaserScheme());
	RegisterScheme(L"lair-guardian", new CLairGuardianScheme());
    RegisterScheme(L"ranged-caster", new CRangedCasterScheme());
	RegisterScheme(L"melee-chaser", new CMeleeChaserScheme());
	RegisterScheme(L"boid-chaser", new CBoidChaserScheme());
	RegisterScheme(L"generic-combat", new CGenericCombatScheme());
	RegisterScheme(L"idle-movement", new CIdleMovementScheme());
	RegisterScheme(L"turning-only", new CTurningScheme());

	std::map<std::wstring,CAIScheme *>::iterator it;
	for ( it=schemes.begin() ; it != schemes.end(); it++ )
		(*it).second->Init(*this);

	gGame.AddFrameListener( this );
}

CAISchemeManager::~CAISchemeManager()
{
    fprintf(stderr,"CAISchemeManager::~CAISchemeManager()\n");
}

void CAISchemeManager::FrameStarted(float secondsPassed){
	std::map<std::wstring,CAIScheme *>::iterator it;
	for ( it=schemes.begin() ; it != schemes.end(); it++ )
		(*it).second->Update(secondsPassed);
}

void CAISchemeManager::RegisterScheme(const std::wstring &name, CAIScheme *scheme)
{
    assert(schemes.count(name) == 0);
    schemes[name] = scheme;
}

void CAISchemeManager::UnregisterScheme(const std::wstring &name){
    assert(schemes.count(name) > 0);
    schemes.erase(name);
}

CAIScheme *CAISchemeManager::GetScheme(const std::wstring &name)
{
	if (schemes.count(name) > 0)	return schemes[name];
    else							return NULL;    
}

void CAISchemeManager::SetBoidDebug(bool on){
	CBoidChaserScheme* boids = dynamic_cast<CBoidChaserScheme*>(schemes[L"boid-chaser"]);
	if (boids != NULL){
		boids->SetDebug(on);
	}
}