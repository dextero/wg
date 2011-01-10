#ifndef _QUEST_
#define _QUEST_

#include "CQuestNodeRequirement.h"
#include <vector>
#include <string>

namespace Quests
{
	class CQuestEvent;

	struct SNote
	{
		std::wstring pageCaption;
		std::wstring content;
	};

	struct SQuestNode
	{
		std::vector<CQuestNodeRequirement*> mRequirements;
		std::vector<std::wstring>	mActions;		/* polecenia konsoli wywolywane po osiagnieciu wezla fabuly */
		std::vector<SNote>			mNotes;			/* wpisy dodane do notatnika/dziennika po -- || -- */
		std::vector<unsigned>		mChilds;
	};

	class CQuest
	{
	public:
		CQuest();
		~CQuest();

		void Load( const std::string& file );
		void Update( CQuestEvent* e = NULL );

	private:
		void AddNode( const SQuestNode& node, unsigned id );
		void AddRequirement( std::string& type, std::vector<std::wstring>& params, SQuestNode& destNode ); 

		bool CheckNodeRequirements( unsigned nodeId, CQuestEvent* e );
		void ExecuteNode( unsigned nodeId );

		std::vector<SQuestNode> mQuestNodes;
		std::vector<unsigned> mGlobalNodes;
		SQuestNode mStartNode;
		SQuestNode* mCurrentNode;
	};
};

#endif

