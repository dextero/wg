#include "CQuestManager.h"
#include "CQuest.h"
using namespace Quests;

template<> CQuestManager* CSingleton<CQuestManager>::msSingleton = 0;

CQuestManager::CQuestManager()
{
//	LoadQuest( "data/plot/test-quest.xml" );
}

CQuestManager::~CQuestManager()
{
}

void CQuestManager::LoadQuest(const std::string &file)
{
	CQuest* quest = new CQuest;
	quest->Load( file );
	mQuests.push_back( quest );
}

void CQuestManager::HandleEvent(Quests::CQuestEvent *e)
{
	for ( unsigned i = 0; i < mQuests.size(); i++ )
	{
		mQuests[i]->Update( e );
	}
}

void CQuestManager::DefineFlag(const std::wstring &flag)
{
	mFlags[ flag ] = true;
}

void CQuestManager::UndefFlag(const std::wstring &flag)
{
	if ( mFlags.count( flag ) > 0 )
	{
		mFlags.erase( flag );
	}
}

bool CQuestManager::FlagDefined(const std::wstring &flag)
{
	return mFlags.count( flag ) > 0;
}

