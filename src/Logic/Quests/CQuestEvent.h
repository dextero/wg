#ifndef _QUEST_EVENT_
#define _QUEST_EVENT_

#include <string>

namespace Quests
{
	enum questEventType
	{
		PHYSICAL_DEATH_BY_ID,
		PHYSICAL_DEATH_BY_GENRE,
		STATEMENT_DISPLAY,
		ITEM_GIVEN,
		ITEM_TAKEN,
        DIFFICULTY_SET
	};


	class CQuestEvent
	{
	public:
		questEventType mType;

		CQuestEvent( questEventType type );
		virtual bool Equal( CQuestEvent* e );
	};


	class CPhysicalDeathById : public CQuestEvent
	{
	public:
		std::wstring mPhysicalId;

		CPhysicalDeathById( const std::wstring& id );
		virtual bool Equal( CQuestEvent* e );
	};


	class CPhysicalDeathByGenre : public CQuestEvent
	{
	public:
		std::wstring mPhysicalGenre;

		CPhysicalDeathByGenre( const std::wstring& t );
		virtual bool Equal( CQuestEvent* e );
	};


	class CStatementDisplay : public CQuestEvent
	{
	public:
		std::wstring mStatementId;

		CStatementDisplay( const std::wstring& id );
		virtual bool Equal( CQuestEvent* e );
	};

	class CItemGiven : public CQuestEvent
	{
	public:
		std::wstring mItemId;

		CItemGiven( const std::wstring& id );
		virtual bool Equal( CQuestEvent* e );
	};

	class CItemTaken : public CQuestEvent
	{
	public:
		std::wstring mItemId;

		CItemTaken( const std::wstring& id );
		virtual bool Equal( CQuestEvent* e );
	};
    
	class CDifficultySet : public CQuestEvent
	{
	public:
		std::wstring mDifficulty;

		CDifficultySet( const std::wstring& id );
		virtual bool Equal( CQuestEvent* e );
	};
};

#endif

