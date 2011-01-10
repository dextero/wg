#ifndef __CGRAPHICALEFFECTS_H__
#define __CGRAPHICALEFFECTS_H__

#include "../../Utils/CSingleton.h"
#include "../../IFrameListener.h"
#include "SEffectParamSet.h"

#include <SFML/System/Vector2.hpp>

class CPhysical;
struct SDisplayableEffect;

#define gGraphicalEffects CGraphicalEffects ::GetSingleton()

class CGraphicalEffects : public CSingleton<CGraphicalEffects>, IFrameListener
{
public:
    // _musi_ byæ wywo³ane na pocz¹tku 
    bool Initialize(const std::string & configFile);

    virtual void FrameStarted( float secondsPassed );

    SEffectParamSet Prepare( const std::string & templateName );

    SDisplayableEffect * ShowEffect(
            SEffectParamSet & eps,
            CPhysical * from = NULL,
            CPhysical * to = NULL
    );

    SDisplayableEffect * ShowEffect(
            SEffectParamSet & eps, 
            const sf::Vector2f & position = sf::Vector2f( 0.0f, 0.0f ),
            CPhysical * from = NULL, 
            CPhysical * to = NULL
    );

    void NoticePhysicalDestroyed( CPhysical* physical );

    void RemoveEffect( SDisplayableEffect * handle );

    void ClearEffects();
};

#endif //__CGRAPHICALEFFECTS_H__//
