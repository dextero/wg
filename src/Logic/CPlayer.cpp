#include <cstdio>

#include "CPlayer.h"

#include "CPlayerManager.h"
#include "CPhysicalManager.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Utils/Maths.h"
#include "../Map/SceneManager/CSceneNode.h"
#include "../Rendering/CDisplayable.h"
#include "../Rendering/ZIndexVals.h"
#include "../GUI/CRoot.h"
#include "../GUI/CProgressBar.h"
#include "../GUI/CTextArea.h"
#include "../GUI/CImageBox.h"
#include "CLogic.h"
#include "../Rendering/CCamera.h"
#include "../Input/CPlayerController.h"
#include "../GUI/Messages/CMessageSystem.h"
#include "Abilities/CAbilityTree.h"
#include "Abilities/CAbility.h"
#include "Abilities/CPinnedAbilityBatch.h"
#include "Abilities/SAbilityInstance.h"
#include "Stats/CRPGCalc.h"
#include "../GUI/Localization/CLocalizator.h"
#include "../Audio/CAudioManager.h"
#include "../Utils/ToxicUtils.h"
#include "../CGameOptions.h"
#include "../Commands/CCommands.h"
#include "Items/CItem.h"
#include <sstream>
#include <cassert>

CPlayer::CPlayer(const std::wstring& uniqueId) :
    CActor(uniqueId),
	mNumber(-1),
    mTotalXP(0.0f),
    mImmortal(false),
    mDied(false),
    mSkillPoints(0),
    mAttrPoints(0),
    mAbilityTrees(NULL),
    mAbiCodes(NULL),
	mPinned(new CPinnedAbilityBatch(this))
{
    fprintf(stderr,"CPlayer::CPlayer(%ls)\n",uniqueId.c_str());

    mController = new CPlayerController(this);
    SetZIndex(Z_PLAYER);
}

CPlayer::~CPlayer()
{
    fprintf(stderr,"CPlayer::~CPlayer(%ls)\n",GetUniqueId().c_str());
    gPlayerManager.UnregisterPlayer(this);

    if (mDied)
        gLogic.SetState(L"death");

    while (!mItems.empty()) {
        delete mItems.back();
        mItems.pop_back();
    }
}

void CPlayer::Update(float dt){
    mController->Update(dt);
    CActor::Update(dt);

    if (mImmortal){
        mStats.RestoreMana(20.0f*dt);
        mStats.DoHeal(20.0f*dt);
    }
}

void CPlayer::Kill()
{
    if (!mImmortal){
        CActor::Kill();
        mDied = true;
        gCommands.ParseCommand(L"capture-screen");
 		gLogic.SaveGame(gGameOptions.GetUserDir() + "/game.save", true, false);

        // dex: zapobiega problemom z save'ami w grze dla 2 graczy - patrz ticket #575
        for (size_t i = 0; i < gPlayerManager.GetPlayerCount(); ++i)
            gPlayerManager.GetPlayerByNumber(i)->SetGodMode(true);
    }
}

void CPlayer::NextLevel( bool ignoreSkillPoints, bool muteSound ){
    mStats.SetBaseAspect(aLevel,mStats.GetBaseAspect(aLevel)+1.0f);
//    mStats.SetBaseAspect(aMaxHP,mStats.GetBaseAspect(aMaxHP)+5.0f);
    if ( !ignoreSkillPoints )
    {
        gMessageSystem.AddMessagef(gLocalizator.GetText("MSG_LEVEL_UP").c_str(), GetLevel() );
        gMessageSystem.AddMessagef(gLocalizator.GetText("MSG_SKILLPOINT_GET").c_str());
//        mAttrPoints+=gRPGCalc.GetLevelDescr(GetLevel()).attrPoints;
        mSkillPoints += 5;
//        mSkillPoints+=gRPGCalc.GetLevelDescr(GetLevel()).skillPoints;
    }

    if (!muteSound)
        if (unsigned sndCount = GetSounds("level-up").size())
            gAudioManager.GetSoundPlayer().Play(GetSounds("level-up")[rand() % sndCount]);
}

void CPlayer::XPGained(float xp, bool ignoreSkillPoints, bool muteSound){
    mTotalXP += xp;
    while (mTotalXP > XPRequired())
        NextLevel( ignoreSkillPoints, muteSound );
}

float CPlayer::XPRequired(){
    return (float)gRPGCalc.GetLevelDescr(GetLevel()+1).requiredXP;
}

float CPlayer::XPPreviouslyRequired(){
    return (float)gRPGCalc.GetLevelDescr(GetLevel()).requiredXP;
}

int CPlayer::GetLevel(){
    return (int)mStats.GetBaseAspect(aLevel);
}

void CPlayer::BindAbilityTrees(std::vector<CAbilityTree*> *mAbiTrees, std::vector<std::string> *mAbiCodes){
    assert(mAbilityTrees == NULL);
    assert(this->mAbiCodes == NULL);
    mAbilityTrees = mAbiTrees;
    this->mAbiCodes = mAbiCodes;
    assert(mAbilityTrees != NULL);
    assert(this->mAbiCodes != NULL);
    assert(mAbiCodes->size() == mAbilityTrees->size());
    // sprawdzamy, czy cos dostajemy "od reki"
    if (!ToxicUtils::isGameInCrimsonMode) {
        for (unsigned int i = 0; i < mAbilityTrees->size(); i++){
            CAbilityTree *at = mAbilityTrees->at(i);
            for (unsigned int j = 0; j < at->GetAbilityNodes().size(); j++){
                const SAbilityNode &an = at->GetAbilityNodes()[j];
                if (an.startLevel > 0)
                    for (int level = 0; level < an.startLevel; level++){
                        AdvanceAbilityLevel(i,j);
                    }
            }
        }
    }
	mPinned->Init();
}

void CPlayer::AddAbilityFromTree(const SAbilityNode *an){
    SAbilityInstance ai(an->ability,1,an);
    mAbilityPerformer.AddAbility(ai,an->animCode);
	mPinned->Update();
}

void CPlayer::AdvanceAbilityLevel(int treeId, int abiId){
    if ((treeId < -1) || (abiId < -1)) return;
    std::vector<SAbilityInstance> *abis = mAbilityPerformer.GetAbilities();
    const SAbilityNode *an = &(mAbilityTrees->at(treeId)->GetAbilityNodes().at(abiId));
    int selected = -1;
    for (unsigned int i = 0; i < abis->size(); i++)
        if (abis->at(i).abiNode == an){
            selected = i;
            break;
        }
	if (selected >= 0)
		mAbilityPerformer.AdvanceAbilityLevel(selected);
	else
	{
		AddAbilityFromTree(an);
		EnableTriggerInAbility(treeId, abiId);
	}
	mPinned->Update();
}

bool CPlayer::CheckPreqs(int treeId, const std::vector<std::pair<int, int> > &preqs){
    const std::vector<SAbilityNode> &tree = mAbilityTrees->at(treeId)->GetAbilityNodes();
    std::vector<SAbilityInstance> *abis = mAbilityPerformer.GetAbilities();
    unsigned int found = 0;

    for (std::vector<std::pair<int, int> >::const_iterator i = preqs.begin(); i != preqs.end(); ++i)
    {
        for (std::vector<SAbilityInstance>::iterator j = abis->begin(); j != abis->end(); ++j)
            if (j->abiNode == &(tree[i->first]))
            {
                if (j->level < i->second)
                    return false;
                ++found;
                break;
            }
    }

    // jesli found != preqs.size(), to ktorys z wymaganych skilli nie jest wykupiony
    return found == preqs.size();
}

EAbilityBuyingResult CPlayer::CanBuyAbilityLevel(int treeId, int abiId){
    const SAbilityNode &an = mAbilityTrees->at(treeId)->GetAbilityNodes()[abiId];
    if (an.ability)
    {
        for (size_t i = 0; i < mAbilityPerformer.GetAbilities()->size(); ++i)
        {
            if (an.ability == mAbilityPerformer.GetAbility(i)->ability)
                if (an.ability->mMaxLevel <= mAbilityPerformer.GetAbility(i)->level) return abrMaxLevelReached;
        }
        if (!an.ability->mCanBuy) return abrNoBuy;
    }
    if (mSkillPoints <= 0) return abrNoPoints;
    if (GetLevel() < an.minPlayerLevel) return abrLevelReqNotMet;
	if (!CheckPreqs(treeId,an.preqs)) return abrTooLowPreqLevel;
	return abrOK;
}

EAbilityBuyingResult CPlayer::TryBuyAbilityLevel(int treeId, int abiId){
    EAbilityBuyingResult result = CanBuyAbilityLevel(treeId, abiId);
    if (result != abrOK) return result;
    mSkillPoints--;
    AdvanceAbilityLevel(treeId, abiId);
    return result;
}

void CPlayer::EnableTriggerInAbility(int treeId, int abiId)
{
	if (treeId >= 0 && abiId >= 0)
	{
		const SAbilityNode *an = &(mAbilityTrees->at(treeId)->GetAbilityNodes().at(abiId));

		if (an->sharedTrigger)
		{
			std::vector<SAbilityInstance> *abis = mAbilityPerformer.GetAbilities();
			for (unsigned i = 0; i < abis->size(); i++)
				if (abis->at(i).ability->trigger == an->ability->trigger)
					abis->at(i).triggerEnabled = (abis->at(i).abiNode == an);
		}
	}
}

int CPlayer::FindAbilityTree(const std::string &code){
    for (unsigned int i = 0; i < mAbiCodes->size(); i++)
        if (mAbiCodes->at(i) == code)
            return i;
    return -1;
}

int CPlayer::FindAbilityInTree(int treeId, const std::string &code){
    if (treeId < 0)
        return -1;
    else
        return mAbilityTrees->at(treeId)->FindAbility(code);
}

void CPlayer::SetGodMode(bool gm){
    mImmortal = gm;
}

std::wstring CPlayer::GetUnboughtPrequisitivesString(int treeIdx, int abiIdx){
	const std::vector<SAbilityNode> &tree = mAbilityTrees->at(treeIdx)->GetAbilityNodes();
	const SAbilityNode &an = tree[abiIdx];
    const std::vector<std::pair<int, int> > & preqs = an.preqs;
    std::vector<SAbilityInstance> *abis = mAbilityPerformer.GetAbilities();
	std::wstring result = L" (";
    for (std::vector<std::pair<int, int> >::const_iterator i = preqs.begin(); i != preqs.end(); ++i){
        bool found = false;
        for (std::vector<SAbilityInstance>::iterator j = abis->begin(); j != abis->end(); ++j)
            if (j->abiNode == &(tree[i->first]) && j->level >= i->second) {
				found = true;
			}
		if (!found){
			if (result!=L" (")
				result+=L", ";
			result+=tree[i->first].ability->name;
            result+=L" [";
            result+=StringUtils::ToWString<int>(i->second);
            result+=L"]";
		}
    }
    result += L")";
	return result;
}

CPinnedAbilityBatch *CPlayer::GetPinnedAbilityBatch(){
	return mPinned;
}

CItem * CPlayer::GetItem(size_t invPos) {
    size_t i = mItems.size();
    while (i > 0) { // od tylu przeszukujemy, bo pozniejsze przedmioty moga przykryc poprzednie... chociaz w sumie nie powinny, wtf
        if (mItems[i-1]->mInvPos == invPos) {
            return mItems[i-1];
        }
        i--;
    }
    return NULL;
}

void CPlayer::AddItem(CItem * item, size_t invPos) {
    CItem * prevItem = GetItem(invPos);
    if (prevItem) {
        RemoveItem(prevItem);
    }
    mItems.push_back(item);
    item->mInvPos = invPos;

    SAbilityInstance ai(gResourceManager.GetAbility(item->GetAbility()), 1);
    GetAbilityPerformer().AddAbility(ai, "", invPos);
}

void CPlayer::RemoveItem(CItem * item) {
    for (size_t i = 0 ; i < mItems.size() ; i++) {
        if (mItems[i] == item) {
            mItems[i] = mItems.back();
            mItems.pop_back();
            break;
        }
    }
}

