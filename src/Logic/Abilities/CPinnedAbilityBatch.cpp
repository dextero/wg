#include "CPinnedAbilityBatch.h"
#include "../CPlayer.h"
#include "CAbilityTree.h"

CPinnedAbilityBatch::CPinnedAbilityBatch(CPlayer *player):
mVersion(0),
mInitialized(false),
mPlayer(player){

}

CPinnedAbilityBatch::~CPinnedAbilityBatch(){

}

int CPinnedAbilityBatch::GetHash(const std::string &tree, const std::string &code){
	int result = 0;
	for (unsigned int i = 0; i < tree.length(); i++){
		result ^= (tree[i] << (i % 4));
	}
	for (unsigned int i = 0; i < code.length(); i++){
		result ^= (code[i] << (i % 4));
	}
	return result;
}

CPinnedAbilityBatch::SExportedAbility *CPinnedAbilityBatch::GetByHash(int hash){
	for (unsigned int i = 0; i < mPinned.size(); i++){
		if (mPinned[i].hash == hash){
			return &mPinned[i];
		}
	}
	return NULL;
}

void CPinnedAbilityBatch::Init(){
	std::vector<CAbilityTree*> &trees = *(mPlayer->GetBoundAbilityTrees());
	const std::vector<std::string> &treeCodes = *mPlayer->GetAbiCodes();
	for (unsigned int i = 0; i < trees.size(); i++){
		CAbilityTree *tree = trees[i];
		std::string treeName = treeCodes[i];
		const std::vector<SAbilityNode> &abiNodes = tree->GetAbilityNodes();
		for (unsigned int j = 0; j < abiNodes.size(); j++){
			if (abiNodes[j].doExport){
				mPinned.push_back(SExportedAbility());
				SExportedAbility &ea = mPinned[mPinned.size()-1];
				ea.abi = abiNodes[j].ability;
				ea.code = abiNodes[j].code;
				ea.hash = GetHash(treeName,ea.code);
				ea.level = 0.0;
				ea.tree = treeName;
				ea.abiId = -1;
				ea.exportName = abiNodes[j].exportName;
			}
		}
	}

	mInitialized = true;
	Update();
}

void CPinnedAbilityBatch::Update(){
	if (!mInitialized) return;

	std::vector<SAbilityInstance> & abis = *mPlayer->GetAbilityPerformer().GetAbilities();
	for (unsigned int i = 0; i < mPinned.size(); i++){
		SExportedAbility &ea = mPinned[i];
		for (unsigned int j = 0; j < abis.size(); j++){
			if (abis[j].ability == ea.abi){
				ea.abiId = j;
				break;
			}
		}
	}

	for (unsigned int i = 0; i < mPinned.size(); i++){
		if (mPinned[i].abiId >= 0)
			mPinned[i].level = (double)(abis[mPinned[i].abiId].level);
		else
			mPinned[i].level = 0.0;
	}
	mVersion++;
}

int CPinnedAbilityBatch::GetAbilityLevel(const std::string &tree, const std::string &code){
	int hash = GetHash(tree,code);
	SExportedAbility *ea = GetByHash(hash);
	if (ea != NULL)
		return (int)(ea->level);
	else
		return 0;
}

CAbility *CPinnedAbilityBatch::GetAbility(const std::string &tree, const std::string &code){
	int hash = GetHash(tree,code);
	SExportedAbility *ea = GetByHash(hash);
	if (ea != NULL)
		return ea->abi;
	else
		return NULL;
}

CExecutionContext *CPinnedAbilityBatch::MorphContext(const std::string &tree, const std::string &code, ExecutionContextPtr ctx){
	int hash = GetHash(tree,code);
	SExportedAbility *ea = GetByHash(hash);
	int level = 0;
	if (ea != NULL)
		level = (int)(ea->level);
	CPhysical *physical = NULL;
	if (ctx->GetCaster()!=NULL)
		physical = &(*(ctx->GetCaster()));
	return CExecutionContext::Alloc(ctx->GetCaster(),physical,level,this);    //CHECK
}
