#ifndef __CPINNEDABILITYBATCH_H__
#define __CPINNEDABILITYBATCH_H__

#include <string>
#include "../Effects/CExecutionContext.h"

class CPlayer;
class CAbility;

class CPinnedAbilityBatch {
public:
	struct SExportedAbility{
	public:
		int hash;
		CAbility *abi;
		std::string tree;
		std::string code;
		int abiId;
		double level;
		std::string exportName;
	};
private:
	int mVersion;

	static int sVersion;

	bool mInitialized;

	std::vector<SExportedAbility> mPinned;
	CPlayer *mPlayer;

	int GetHash(const std::string &tree, const std::string &code);
	SExportedAbility *GetByHash(int hash);
public:
	CPinnedAbilityBatch(CPlayer *player);
	~CPinnedAbilityBatch();

	void Init();
	void Update();

	int GetAbilityLevel(const std::string &tree, const std::string &code);
	CAbility *GetAbility(const std::string &tree, const std::string &code);
	CExecutionContext *MorphContext(const std::string &tree, const std::string &code, ExecutionContextPtr ctx);

	inline const std::vector<SExportedAbility> &GetPinnedAbilities() const{
		return mPinned;
	}

	inline int GetVersion(){
		return mVersion;
	}
};

#endif

