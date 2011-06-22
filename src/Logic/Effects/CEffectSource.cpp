#include "CEffectSource.h"
#include "../CActor.h"
#include "../CDetector.h"
#include "../CBullet.h"
#include "../Abilities/CAbility.h"

int CEffectSource::InstancesCount = 0;

CPhysical *CEffectSource::DeterminePhysicalSource(){
    CPhysical *prim = NULL, *sec = NULL;
    if (primary.type == estActor)
        prim = primary.actor;
    else if (primary.type == estNextSource)
        if (primary.nextSource != EffectSourcePtr(NULL))
            prim = primary.nextSource->DeterminePhysicalSource();
    if (secondary.type == estActor)
        sec = secondary.actor;
    else if (secondary.type == estNextSource)
        if (secondary.nextSource != EffectSourcePtr(NULL))
            sec = secondary.nextSource->DeterminePhysicalSource();
    if (prim == NULL)
        return sec;
    else if (sec == NULL)
        return prim;
    else {
        fprintf(stderr,"ah! confused while determining physical source!\n");
        return sec;
    }
}

void CEffectSource::PrintToStderr(){
    PrintToStderr(0);
}

void CEffectSource::PrintToStderr(int indent){
    for (int i = 0; i < indent; i++)
        fprintf(stderr,"\t");
    fprintf(stderr,"prim: ");
    primary.PrintToStderr(indent);
    for (int i = 0; i < indent; i++)
        fprintf(stderr,"\t");
    fprintf(stderr,"sec:  ");
    secondary.PrintToStderr(indent);    
}

void CEffectSource::CSource::PrintToStderr(int indent){
    switch (type){
        case estNone:
            fprintf(stderr,"NONE\n");
            break;
        case estNextSource:
            if (nextSource == NULL)
                fprintf(stderr,"Next: NULL\n");
            else {
                fprintf(stderr,"Next: \n");
                nextSource->PrintToStderr(indent+1);
            }
            break;
        case estBullet:
            fprintf(stderr,"Bullet: %p at %f, %f\n",bullet,bullet->GetPosition().x,bullet->GetPosition().y);
            break;
        case estAbility:
            fprintf(stderr,"Ability: %ls\n",ability->name.c_str());
            break;
        case estExplosion:
            fprintf(stderr,"Detector/explosion: %p at %f, %f\n",detector,detector->GetPosition().x,detector->GetPosition().y);
            break;
        case estAppliedEffect:
            fprintf(stderr,"AE for offset %d\n",appliedEffect->offset);
            break;
        case estActor:
            fprintf(stderr,"Actor: %p (%ls) at %f, %f\n",actor,actor->GetUniqueId().c_str(),actor->GetPosition().x,actor->GetPosition().y);
            break;
        case estDelayingAbility:
            fprintf(stderr,"Delaying ability: %ls\n",ability->name.c_str());
            break;
        case estCastingAbility:
            fprintf(stderr,"Casting ability: %ls\n",ability->name.c_str());
            break;
        case estGenericPhysical:
            fprintf(stderr,"Generic physical: %p (%ls)\n",physical,physical->GetUniqueId().c_str());
            break;
    }
}

