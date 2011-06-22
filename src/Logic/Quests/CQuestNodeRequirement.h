#ifndef _QUEST_NODE_REQUIREMENT_
#define _QUEST_NODE_REQUIREMENT_

#include "CQuestEvent.h"
#include <string>

namespace Quests
{
	class CQuestNodeRequirement
	{
	public:
		CQuestNodeRequirement();
		~CQuestNodeRequirement();

		inline void SetRequiredEvent( CQuestEvent* e, unsigned count )	{ mRequiredEvent = e; mEventCallsCount = count; }
		inline void SetRequiredFlag( const std::wstring& flag )			{ mRequiredFlag = flag; }
		inline bool Achieved() const									{ return mAchieved; }

		void Update( CQuestEvent * e );
		
	private:
		CQuestEvent* mRequiredEvent;
		unsigned	 mEventCallsCount;
		std::wstring mRequiredFlag;
		bool		 mAchieved;
	};
};

#endif

