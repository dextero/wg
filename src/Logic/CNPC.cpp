#include "CNPC.h"
#include "AI/CActorAI.h"

CNPC::CNPC( const std::wstring& uniqueId )
:	CActor( uniqueId )
{
	CActorAI * ai = new CActorAI( this );
	ai->GetData()->SetAttitude( AI_NEUTRAL );
	mController = ai;
}

CNPC::~CNPC()
{
}

void CNPC::Update(float dt)
{
    mController->Update(dt);
    CActor::Update(dt);
}

