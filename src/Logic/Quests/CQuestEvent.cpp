#include "CQuestEvent.h"
#include "../../Utils/StringUtils.h"
#include "../../Logic/CLogic.h"
using namespace Quests;

CQuestEvent::CQuestEvent( questEventType type )
:	mType( type )
{
}

bool CQuestEvent::Equal( CQuestEvent* e )
{
	return mType == e->mType;
}


CPhysicalDeathById::CPhysicalDeathById(const std::wstring &id)
:	CQuestEvent( PHYSICAL_DEATH_BY_ID ),
	mPhysicalId( id )
{
}

bool CPhysicalDeathById::Equal(Quests::CQuestEvent *e)
{
	if ( mType == e->mType )	return mPhysicalId == ((CPhysicalDeathById*) e)->mPhysicalId;
	else						return false;
}


CPhysicalDeathByGenre::CPhysicalDeathByGenre(const std::wstring &t)
:	CQuestEvent( PHYSICAL_DEATH_BY_GENRE ),
	mPhysicalGenre( t )
{
}

bool CPhysicalDeathByGenre::Equal(Quests::CQuestEvent *e)
{
	if ( mType == e->mType )	return mPhysicalGenre == ((CPhysicalDeathByGenre*) e)->mPhysicalGenre;
	else						return false;
}


CStatementDisplay::CStatementDisplay(const std::wstring &id)
:	CQuestEvent( STATEMENT_DISPLAY ),
	mStatementId( id )
{
}

bool CStatementDisplay::Equal(Quests::CQuestEvent *e)
{
	if ( mType == e->mType )	return mStatementId == ((CStatementDisplay*) e)->mStatementId;
	else						return false;
}


CItemGiven::CItemGiven(const std::wstring &id)
:	CQuestEvent( ITEM_GIVEN ),
	mItemId( id )
{
}

bool CItemGiven::Equal(Quests::CQuestEvent *e)
{
	if ( mType == e->mType )	return mItemId == ((CItemGiven*) e)->mItemId;
	else						return false;
}


CItemTaken::CItemTaken(const std::wstring &id)
:	CQuestEvent( ITEM_TAKEN ),
	mItemId( id )
{
}

bool CItemTaken::Equal(Quests::CQuestEvent *e)
{
	if ( mType == e->mType )	return mItemId == ((CItemTaken*) e)->mItemId;
	else						return false;
}


CDifficultySet::CDifficultySet(const std::wstring &diff)
:	CQuestEvent( DIFFICULTY_SET ),
	mDifficulty( diff )
{
}

bool CDifficultySet::Equal(Quests::CQuestEvent *e)
{
    if ( mType == e->mType )	return StringUtils::Parse<float>(mDifficulty) == gLogic.GetDifficultyFactor();
	else						return false;
}
