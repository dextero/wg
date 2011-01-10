#include "CQuestNodeRequirement.h"
#include "CQuestManager.h"
using namespace Quests;

CQuestNodeRequirement::CQuestNodeRequirement()
:	mRequiredEvent( NULL ),
	mEventCallsCount( 0 ),
	mRequiredFlag( L"" ),
	mAchieved( false )
{
}

CQuestNodeRequirement::~CQuestNodeRequirement()
{
	delete mRequiredEvent;
}

void CQuestNodeRequirement::Update(Quests::CQuestEvent *e)
{
	bool mEventsCalled = true;
	bool mFlagDefined = true;

	/*if ( mEventCallsCount == 0 && mRequiredEvent != NULL )
	{
		delete mRequiredEvent;
		mRequiredEvent = NULL;
	}*/

	if ( mEventCallsCount > 0 && mRequiredEvent != NULL && e != NULL )
    {
	    if ( mRequiredEvent->Equal( e ) )
		    mEventsCalled = ( --mEventCallsCount == 0 );
	    else
		    mEventsCalled = false;
    }

	if ( mRequiredFlag != L"" )
		mFlagDefined = gQuestManager.FlagDefined( mRequiredFlag );

	mAchieved = mEventsCalled && mFlagDefined;
}

