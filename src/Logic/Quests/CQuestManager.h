#ifndef _QUEST_MANAGER_
#define _QUEST_MANAGER_

#include "../../Utils/CSingleton.h"
#include <vector>
#include <string>
#include <map>

#define gQuestManager Quests::CQuestManager::GetSingleton()

namespace Quests
{
	class CQuestEvent;
	class CQuest;

	class CQuestManager : public CSingleton< CQuestManager >
	{
	public:
		CQuestManager();
		~CQuestManager();

		void LoadQuest( const std::string& file );
		void HandleEvent( CQuestEvent* e );

		void DefineFlag( const std::wstring& flag );
		void UndefFlag( const std::wstring& flag );
		bool FlagDefined( const std::wstring& flag );

        inline const std::map<std::wstring, bool>& GetFlags() const { return mFlags; }

	private:
		std::vector<CQuest*> mQuests;
		std::map<std::wstring,bool> mFlags;
	};
};

#endif

