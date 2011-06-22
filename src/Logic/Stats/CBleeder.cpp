#include <cassert>

#include "CBleeder.h"
#include "../CPhysical.h"
#include "../CActor.h"
#include "../CLogic.h"
#include "../../GUI/Messages/CMessageSystem.h"
#include "../../Rendering/Effects/CGraphicalEffects.h"

#include "../../Rendering/Effects/SEffectParamSet.h"
#include "../../Audio/CAudioManager.h"


CBleeder::CBleeder(CPhysical *attached):
mAttached(attached){
    assert(mAttached!=NULL);
}

CBleeder::~CBleeder(){
}

void CBleeder::Bleed(float dmg,EffectSourcePtr source){
    
    SEffectParamSet eps = gGraphicalEffects.Prepare( "bloodsplatter" );
    gGraphicalEffects.ShowEffect( eps, mAttached );

    eps = gGraphicalEffects.Prepare( "blood-splat" );
    eps.amount = dmg;
    gGraphicalEffects.ShowEffect( eps, mAttached->GetPosition() );
    
    gMessageSystem.AddMessageTof(mAttached, sf::Color::Red, L"%.0f", dmg);

	CActor *actor = dynamic_cast<CActor*>(mAttached);
	if (actor != NULL){
		if (source->primary.type == estAppliedEffect){

		}

		std::vector<System::Resource::CSound*>& sounds = actor->GetSounds("damaged");
		if (unsigned sndCount = sounds.size())
			gAudioManager.GetSoundPlayer().Play(sounds[rand() % sndCount], actor->GetPosition());
	}
}
