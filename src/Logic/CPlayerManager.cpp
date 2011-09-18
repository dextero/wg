#include <cstdio>

#include "CPlayerManager.h"
#include "CPlayer.h"
#include "MapObjects/CRegion.h"
#include "CPhysicalManager.h"
#include "../Utils/StringUtils.h"
#include "../ResourceManager/CResourceManager.h"
#include "Factory/CPlayerTemplate.h"
#include "CLogic.h"
#include "../GUI/CHud.h"
#include "CActorController.h"
using namespace StringUtils;

template<> CPlayerManager* CSingleton<CPlayerManager>::msSingleton = 0;

CPlayerManager::CPlayerManager()
{
    fprintf( stderr, "CPlayerManager::CPlayerManager()\n" );
}

CPlayerManager::~CPlayerManager()
{
    fprintf( stderr, "CPlayerManager::~CPlayerManager()\n" );
}

void CPlayerManager::LoadPlayer(size_t number)
{
	std::string templ = std::string("data/player/player") + ToString(number) + std::string(".xml");
	std::wstring name = std::wstring(L"player") + ToWString(number);
    CPlayer* player = (CPlayer*) gResourceManager.GetPhysicalTemplate(templ)->Create(name);
	RegisterPlayer(player, number);
    gLogic.GetHud(number)->Init(number);
}

void CPlayerManager::RegisterPlayer(CPlayer *player, size_t number)
{
    assert(player!=NULL);

	if (player != NULL)
	{
		UnregisterPlayer(number);
		player->SetNumber(number);
		mPlayers.push_back(player);
	}
}

void CPlayerManager::UnregisterPlayer(CPlayer* player)
{
	assert(player!=NULL);

	if (player!=NULL)
	{
		UnregisterPlayer(player->GetNumber());
	}
}

void CPlayerManager::UnregisterPlayer(size_t number)
{
	for (size_t i=0; i<mPlayers.size(); i++)
	if (mPlayers[i]->GetNumber() == number)
	{
		mPlayers[i]->MarkForDelete();
		mPlayers[i]->SetNumber(-1);
		mPlayers.erase(mPlayers.begin()+i);
		return;
	}

	fprintf( stderr, "warning: CPlayerManager::UnregisterPlayer(CPlayer*) - player not found in mPlayer\n") ;
}

CPlayer *CPlayerManager::GetPlayerByNumber(size_t number)
{
	for (size_t i=0; i<mPlayers.size(); i++)
		if (mPlayers[i]->GetNumber() == number)
			return mPlayers[i];

	return NULL;
}

void CPlayerManager::XPGained(float xp, bool ignoreSkillPoints, bool muteSound){
    if (!ignoreSkillPoints)
        xp /= (float)mPlayers.size();
    for (unsigned int i = 0; i < mPlayers.size(); i++)
		mPlayers[i]->XPGained(xp, ignoreSkillPoints, muteSound);
}

void CPlayerManager::SetGodMode(bool gm){
    for (unsigned int i = 0; i < mPlayers.size(); i++)
        mPlayers[i]->SetGodMode(gm);
}

void CPlayerManager::MovePlayersToRegion(const std::string &region){
    CPhysical *p = gPhysicalManager.GetPhysicalById(StringUtils::ConvertToWString(region));
    CDynamicRegion *r = dynamic_cast<CDynamicRegion*>(p);
    if(r != NULL) {
        for (unsigned int i = 0; i < mPlayers.size(); i++) {
			mPlayers[i]->SetPosition( r->GetPosition() + sf::Vector2f(.5f,0)*float(i) );
            mPlayers[i]->GetActorController()->SetTrueRot(r->GetRotation());
        }
    }
}

void CPlayerManager::MovePlayersToFirstPlayer() {
	if (CPlayer* p = GetPlayerByNumber(0))
		for (unsigned i = 0; i < mPlayers.size(); i++)
			if (mPlayers[i]->GetNumber() != 0)
				mPlayers[i]->SetPosition( p->GetPosition() + sf::Vector2f(0.1f, 0.0f)*(float)i );
}

bool CPlayerManager::VerifyKnownPhysical(CPhysical *phys){
    for (size_t i = 0; i < mPlayers.size(); i++)
        if (phys == mPlayers[i])
            return true;
    return false;
}
