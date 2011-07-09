#include "CGraphicalEffects.h"
#include "../Particles/CParticleManager.h"
#include "../Particles/Renderers/CSpriteRender.h"
#include "../PostProcessing/CPostProcessing.h"
#include "../../Utils/CXml.h"
#include "../CDisplayable.h"
#include "../CDrawableManager.h"
#include "../Animations/SAnimationState.h"
#include "../Animations/SAnimation.h"
#include "../../Utils/CRand.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/Directions.h"
#include "LinearEffects/CLightning.h"
#include "LinearEffects/CStraightLightning.h"
#include "../../Map/CMap.h"
#include "SEffectParamSet.h"
#include "../../Logic/CPhysicalManager.h"
#include "../../Console/CConsole.h"

// tox, 29 jul - tak naprawde to te efekty displayable to powinny byc
// przypadkami szczegolnymi particli (jedno-billboardowe particle)
// i korzystac z calego dobrodziejstwa afektorow czasteczkowych
// tymczasem zeby to chociaz dzialalo to zrobie na lapu-capu
// a najwyzej ktos inny potem bedzie to przerabiac na particle

struct SDisplayableEffectFrame {
    float duration;
    float scale;
    float alpha;
    float rotation;

    SDisplayableEffectFrame(
            float _duration = 1.0,
            float _scale = 1.0,
            float _alpha = 1.0,
            float _rotation = 0.0
    ) :
            duration( _duration ),
            scale( _scale ),
            alpha( _alpha ),
            rotation( _rotation )
    {
        ;
    }


};

typedef std::vector< SDisplayableEffectFrame > EffectAnimation;
static std::map< std::string, EffectAnimation > gsAnimations;

struct SDisplayableEffect {
    SEffectParamSet eps;
    CDisplayable * displayable;
    ILinearEffect * linearEffect;
    CPhysical * tracedPhysical;
    float timeToLive;
    bool isPerpetual;

    SDisplayableEffect( 
            const SEffectParamSet & _eps,
            CDisplayable * _displayable,
            ILinearEffect * _linearEffect
    ) :
            eps( _eps ),
            displayable( _displayable ),
            linearEffect( _linearEffect ),
            tracedPhysical( NULL ),
            isPerpetual(false)
    {
        if ( eps.effectAnimation.empty() )
            timeToLive = eps.duration;
        else
        {
//            timeToLive = 0;            
//            const EffectAnimation & animation = gsAnimations[ eps.effectAnimation ];
//            for ( EffectAnimation::const_iterator it = animation.begin() ; it != animation.end() ; it++ )
//                timeToLive += it->duration;
              timeToLive = eps.duration;
        }
    };
};
        
SDisplayableEffectFrame CalculateInterpolatedFrame(
        const EffectAnimation & animation,
        float timePosition
        )
{
//    fprintf( stderr, "timePosition=%f\n", timePosition );
    SDisplayableEffectFrame ret;
    float animationLength = 0;
    for ( EffectAnimation::const_iterator it = animation.begin() ; it != animation.end() ; it++ )
        animationLength += it->duration;
    // todo, optymalizacja: cachowac gdzies dlugosc animacji, np przy inicjalizacji
//    fprintf( stderr, "animationLength=%f\n", animationLength );

    const SDisplayableEffectFrame * frame = NULL;
    const SDisplayableEffectFrame * nextFrame = NULL;

    float fraction = 0.0f;

    float currentTime = 0.0f;
    float prevTime = currentTime;
    for ( EffectAnimation::const_iterator it = animation.begin() ; it != animation.end() ; it++ )
    {
        prevTime = currentTime;
        currentTime += it->duration / animationLength;
        if ( currentTime >= timePosition )
        {
            frame = &*it;
            it++;
            if ( it != animation.end() )
            {
                nextFrame = &*it;
                fraction = ( timePosition - prevTime ) / ( currentTime - prevTime );
            }
            else
                nextFrame = frame;
            break;
        }
    }
    if ( !nextFrame || !frame )
    {
        fprintf( stderr, "warning: CGraphicalEffects.cpp::FetchFrame: frames are null, "
                "probably timePosition=%f exceeds 1.0\n", timePosition );
        return ret;
    }
//    fprintf( stderr, "fraction=%f\n", fraction );
//    fprintf( stderr, "frame1{ %f, %f, %f, %f }\n", frame->duration, frame->scale, frame->alpha, frame->rotation );
//    fprintf( stderr, "frame2{ %f, %f, %f, %f }\n", nextFrame->duration,
//           nextFrame->scale, nextFrame->alpha, nextFrame->rotation );

    ret.scale = ( frame->scale * ( 1 - fraction ) ) + ( nextFrame->scale * fraction );
    ret.alpha = ( frame->alpha * ( 1 - fraction ) ) + ( nextFrame->alpha * fraction );
    ret.rotation = ( frame->rotation * ( 1 - fraction ) ) + ( nextFrame->rotation * fraction );

//    fprintf( stderr, "ret{ %f, %f, %f, %f }\n", ret.duration, ret.scale, ret.alpha, ret.rotation );
    return ret;
}

typedef std::vector< SDisplayableEffect * > DisplayableEffectVector;
static DisplayableEffectVector gsEffects;

template<> CGraphicalEffects*CSingleton<CGraphicalEffects>::msSingleton = 0;

void DeleteDisplayableEffect(SDisplayableEffect * effect);

bool CGraphicalEffects::Initialize(const std::string & configFile)
{
    gGame.AddFrameListener( this );

    //fprintf(stderr, ">Initalizing graphic effects...\n");

    CXml cfg(configFile, "root");

    if (cfg.GetString(0,"type") != "effects-system")
        return false;

    xml_node<> *node;

    //fprintf(stderr,"\tLoading particle renderer data...\n");
    node = cfg.GetChild(0,"prenderer");
    if (!part::renderer::gSpriteRenderer.LoadConfiguration(cfg.GetString(node,"config")))
        return false;

    //fprintf(stderr,"\tLoading particle systems configuration...\n");
    node = cfg.GetChild(0,"particles");   
    if (!part::gParticleManager.LoadConfiguration(cfg.GetString(node,"config")))
        return false;


    // TODO:
    // PostFX ³adowanie konfiguracji

    //fprintf(stderr,">Initialization done.\n");


    // tox, 29 jul: todo: z xmla wczyt
    EffectAnimation animation;
    animation.push_back( SDisplayableEffectFrame( 0.270f, 0.001f, 1.000f, 0.000f ) );
    animation.push_back( SDisplayableEffectFrame( 0.000f, 1.000f, 0.000f, 0.000f ) );
    gsAnimations[ "shockwave" ] = animation;

    animation.clear();
    animation.push_back( SDisplayableEffectFrame( 0.225f, 0.001f, 0.000f, 10.000f ) );
    animation.push_back( SDisplayableEffectFrame( 0.225f, 0.500f, 1.000f, 5.000f ) );
    animation.push_back( SDisplayableEffectFrame( 2.000f, 1.000f, 1.000f, 0.000f ) );
    animation.push_back( SDisplayableEffectFrame( 0.450f, 1.000f, 1.000f, 0.000f ) );
    animation.push_back( SDisplayableEffectFrame( 0.000f, 1.000f, 0.000f, 0.000f ) );
    gsAnimations[ "enlarge-then-disappear" ] = animation;

    animation.clear();
    animation.push_back( SDisplayableEffectFrame( 0.750f, 0.001f, 0.000f, 0.000f ) );
    animation.push_back( SDisplayableEffectFrame( 5.000f, 1.000f, 1.000f, 0.000f ) );
    animation.push_back( SDisplayableEffectFrame( 5.000f, 1.000f, 1.000f, 0.000f ) );
    animation.push_back( SDisplayableEffectFrame( 0.000f, 1.000f, 0.000f, 0.000f ) );
    gsAnimations[ "blood-splat" ] = animation;

    animation.clear();
    animation.push_back( SDisplayableEffectFrame( 0.250f, 1.000f, 0.000f, 0.000f ) );
    animation.push_back( SDisplayableEffectFrame( 0.150f, 1.250f, 1.000f, 22.50f ) );
    animation.push_back( SDisplayableEffectFrame( 0.150f, 1.500f, 0.600f, 45.00f ) );
    animation.push_back( SDisplayableEffectFrame( 0.150f, 1.750f, 0.250f, 67.50f ) );
    animation.push_back( SDisplayableEffectFrame( 0.000f, 2.000f, 0.000f, 90.00f ) );
    gsAnimations[ "magic-circle" ] = animation;

    animation.clear();
    animation.push_back( SDisplayableEffectFrame( 1.000f, 0.000f, 0.000f, 0.000f ) );
    animation.push_back( SDisplayableEffectFrame( 1.000f, 0.500f, 0.500f, 90.0f ) );
    animation.push_back( SDisplayableEffectFrame( 1.000f, 0.800f, 0.800f, 180.0f ) );
    animation.push_back( SDisplayableEffectFrame( 1.000f, 0.900f, 0.900f, 270.0f ) );
    animation.push_back( SDisplayableEffectFrame( 0.000f, 1.000f, 1.000f, 360.0f ) );
    animation.push_back( SDisplayableEffectFrame( 1.000f, 1.000f, 1.000f, 0.0f ) );
    animation.push_back( SDisplayableEffectFrame( 1.000f, 0.900f, 0.900f, 90.0f ) );
    animation.push_back( SDisplayableEffectFrame( 1.000f, 0.800f, 0.800f, 180.0f ) );
    animation.push_back( SDisplayableEffectFrame( 1.000f, 0.500f, 0.500f, 270.0f ) );
    animation.push_back( SDisplayableEffectFrame( 0.000f, 0.000f, 0.000f, 360.0f ) );
    gsAnimations[ "loot-circle" ] = animation;


    return true;
}
void RecalculateColorForPhysical( CPhysical * physical )
{
    assert ( physical != NULL );
    if ( !physical->GetDisplayable() )
        return;

    float r,g,b,a;
    r = g = b = a = 0.0;
    int colorizeCount = 0;
    for ( size_t i = 0 ; i < gsEffects.size() ; i++ )
    {
        SDisplayableEffect & effect = *gsEffects[i];
        if ( effect.tracedPhysical == physical && effect.eps.name == "colorize" )
        {
            colorizeCount++;
            r += effect.eps.red;
            g += effect.eps.green;
            b += effect.eps.blue;
            a += effect.eps.alpha;
        }
    }
    float baseR, baseG, baseB, baseA;
    physical->GetColor( &baseR, &baseG, &baseB, &baseA );
    if ( colorizeCount > 0 )
    {
        baseR *= ( r / (float)colorizeCount );
        baseG *= ( g / (float)colorizeCount );
        baseB *= ( b / (float)colorizeCount );
        baseA *= ( a / (float)colorizeCount );
    }
    physical->GetDisplayable()->SetColor( baseR, baseG, baseB, baseA );
}

void CGraphicalEffects::FrameStarted( float secondsPassed )
{
    // tox, 11 czerwca: a co bedzie, jesli gra zmieni poziom, wywali wszystkie dotychczasowe
    // CDisplayable, takze te trzymany na gsEffects[] ? crash? trzeba by dorobic zatem jakegos
    // CGraphicalEffects::Clear() i go wywolywac przy czyszczeniu DrawableManagera... hm..hm..hm..
    //
    // tox, 13 czerwca: liosan cos na GG mi mowil o tym, ale juz zapomnialem co... hm..hm..hm..
	// Liosan, 30.08: milo widziec, ze moje strzepienie palce nie idzie na marne :]
	// zaznaczam, ze bug wystepuje - gdy gracz sie nie ruszy na pierwszej planszy i zginie,
	// to po respawnie plamy krwi sa obecne

    std::vector< size_t > toDeleteIndexes;
    std::vector< CPhysical* > toRecalculateColor;

    for ( size_t i = 0 ; i < gsEffects.size() ; i++ )
    {
        SDisplayableEffect & effect = *gsEffects[i];

        if ( effect.displayable )
        {
            if ( effect.tracedPhysical )
                effect.displayable->SetPosition( effect.tracedPhysical->GetPosition() );

            SAnimationState * animState = effect.displayable->GetAnimationState();
            if ( animState )
            {
                float timePosition = (effect.eps.duration - effect.timeToLive) / effect.eps.duration;
                animState->RewindTo( timePosition );
            }
        }
        
        effect.timeToLive -= secondsPassed;
        if (effect.isPerpetual && effect.timeToLive <= 0.0f) {
            effect.timeToLive += effect.eps.duration;
        }

        if (effect.timeToLive <= 0.0f)
        {
            if ( effect.eps.name == "colorize" )
            {
                CPhysical * traced = effect.tracedPhysical;
                if ( traced )
                    toRecalculateColor.push_back( traced );
            }

            toDeleteIndexes.push_back( i ); 
            continue;
        }

        if ( effect.linearEffect )
        {
            effect.linearEffect->Animate(secondsPassed);
        }

        if ( effect.displayable && !effect.eps.effectAnimation.empty() )
        {
            float timePosition = (effect.eps.duration - effect.timeToLive) / effect.eps.duration;
            const EffectAnimation & animation = gsAnimations[ effect.eps.effectAnimation ];
            SDisplayableEffectFrame interpolatedFrame = CalculateInterpolatedFrame( animation, timePosition );

            float scale = effect.eps.scale;
            float rotation = effect.eps.rotation;
            effect.displayable->SetScale( scale * interpolatedFrame.scale );
            effect.displayable->SetColor( 1.0f * effect.eps.red, 1.0f * effect.eps.green, 1.0f * effect.eps.blue, 
                    interpolatedFrame.alpha * effect.eps.alpha );
            effect.displayable->SetRotation( rotation + interpolatedFrame.rotation );
        }
    }

    for ( std::vector< size_t >::reverse_iterator it = toDeleteIndexes.rbegin() ; it != toDeleteIndexes.rend() ; it++ )
    {
        SDisplayableEffect * effect = gsEffects[ (*it) ];
        DeleteDisplayableEffect(effect);
        gsEffects.erase( gsEffects.begin() + (*it) );
    }

    for ( std::vector< CPhysical* >::iterator it = toRecalculateColor.begin() ; it != toRecalculateColor.end() ; it++ )
        RecalculateColorForPhysical( *it );
}


SEffectParamSet CGraphicalEffects::Prepare( const std::string & templateName )
{
//    gConsole.Printf( L"Prepare( %s )", templateName.c_str() );

    SEffectParamSet eps;

// tox, 28 todo: zrobic jakas mape string->eps i ja sobie wczytywac z xml'a w kontruktorze

    eps.name = templateName;
    
    if ( templateName == "blood-splat" )
    {
        eps.image = "data/effects/blood/blood" + StringUtils::ToString( gRand.Rnd( 1, 4 + 1 ) ) + ".png";
        eps.scale = 1.0f;
        eps.zIndex = Z_BLOOD_SPLATS;
        eps.amount = 10.0f;
        eps.rotation = gRand.Rndf( 0, 360 );
        eps.effectAnimation = "blood-splat";
        eps.duration = 8.0f + gRand.Rndf(0.0f, 4.0f);
    }
    else if ( templateName == "bullet-explosion" )
    {
        eps.animation = "core/anim/effects/test-explosions/" + StringUtils::ToString( gRand.Rnd( 1, 16 + 1 ) );
        eps.duration = 0.8f;
    }
    else if ( templateName == "bullet-explosion-mini" )
    {
        eps.animation = "core/anim/effects/test-explosions/mini/" + StringUtils::ToString( gRand.Rnd( 1, 14 + 1 ) );
        eps.duration = 0.8f;
    }
    else if ( templateName == "green-explosion" )
    {
        eps.animation = "core/anim/effects/green-explosions/" + StringUtils::ToString( gRand.Rnd( 1, 16 + 1 ) );
        eps.duration = 0.8f;
    }
    else if ( templateName == "green-explosion-mini" )
    {
        eps.animation = "core/anim/effects/green-explosions/mini/" + StringUtils::ToString( gRand.Rnd( 1, 14 + 1 ) );
        eps.duration = 0.8f;
    }
    else if ( templateName == "blue-explosion" )
    {
        eps.animation = "core/anim/effects/blue-explosions/" + StringUtils::ToString( gRand.Rnd( 1, 16 + 1 ) );
        eps.duration = 0.8f;
    }
    else if ( templateName == "blue-explosion-mini" )
    {
        eps.animation = "core/anim/effects/blue-explosions/mini/" + StringUtils::ToString( gRand.Rnd( 1, 14 + 1 ) );
        eps.duration = 0.8f;
    }
    else if ( templateName == "hibernate" )
    {
        eps.animation = "core/anim/effects/meditation";
        eps.duration = 5.8f;
        // tox 29sie, todo: trzeba dorobic mechanizm skalowania dlugosci takze animacji przez eps.duration,
        // bo sie efekt-graficzny nie naklada z dlugoscia efektu-logicznego... to samo jest przy effectAnimation...
    }
    else if ( templateName == "colorize-blue" )
    {
        eps.name = "colorize";
        eps.blue = 1.0f;
        eps.green = 0.55f;
        eps.red = 0.50f;
        eps.alpha = 1.0f;
    }
    else if ( templateName == "colorize-red" )
    {
        eps.name = "colorize";
        eps.blue = 0.0f;
        eps.green = 0.0f;
        eps.red = 1.0f;
        eps.alpha = 1.0f;
    }
    else if ( templateName == "colorize-invisible" )
    {
        eps.name = "colorize";
        eps.red = 0.5f;
        eps.green = 0.5f;
        eps.blue = 0.5f;
        eps.alpha = 0.10f;
    }
    else if ( templateName == "colorize-green" )
    {
        eps.name = "colorize";
        eps.red = 0.2f;
        eps.blue = 0.4f;
        eps.green = 0.9f;
        eps.alpha = 1.0f;
    }
    else if ( templateName == "magic-circle" )
    {
        eps.image = "data/effects/magic/magiccircle" + StringUtils::ToString( gRand.Rnd( 1, 5 + 1 ) ) + ".png";
        eps.zIndex = Z_BLOOD_SPLATS;
        eps.effectAnimation = "magic-circle";
        eps.scale = 2.5f;
    }
    else if ( templateName == "magic-circle-1" )
    {
        eps.image = "data/effects/magic/magiccircle1.png";
        eps.zIndex = Z_BLOOD_SPLATS;
        eps.effectAnimation = "magic-circle";
        eps.scale = 2.5f;
    }
    else if ( templateName == "magic-circle-2" )
    {
        eps.image = "data/effects/magic/magiccircle2.png";
        eps.zIndex = Z_BLOOD_SPLATS;
        eps.effectAnimation = "magic-circle";
        eps.scale = 2.5f;
    }
    else if ( templateName == "magic-circle-3" )
    {
        eps.image = "data/effects/magic/magiccircle3.png";
        eps.zIndex = Z_BLOOD_SPLATS;
        eps.effectAnimation = "magic-circle";
        eps.scale = 2.5f;
    }
    else if ( templateName == "magic-circle-4" )
    {
        eps.image = "data/effects/magic/magiccircle4.png";
        eps.zIndex = Z_BLOOD_SPLATS;
        eps.effectAnimation = "magic-circle";
        eps.scale = 2.5f;
    }
    else if ( templateName == "magic-circle-5" )
    {
        eps.image = "data/effects/magic/magiccircle5.png";
        eps.zIndex = Z_BLOOD_SPLATS;
        eps.effectAnimation = "magic-circle";
        eps.scale = 2.5f;
    }
    else if ((templateName == "loot-circle" ) || (templateName == "loot-circle-perpetual"))
    {
        eps.image = "data/effects/loot-circle.png";
        eps.zIndex = Z_BLOOD_SPLATS - 1; // minus jeden, zeby koleczko bylo widoczne wsrod kaluz krwi
        eps.effectAnimation = "loot-circle";
        eps.scale = 1.0f;
        eps.duration = 2.0f;
        eps.removeTogetherWithTraced = true;
    }
    else if ( templateName == "entangle" )
    {
        eps.rotation = gRand.Rndf( 0, 360 );
        eps.image = "data/effects/entangle.png";
        eps.effectAnimation = "enlarge-then-disappear";
        eps.scale = 0.95f;
    }
	else if ( eps.name.substr(0,5) == "laser" )
	{
		eps.duration = 0.50f;
	}
    else if ( eps.name.substr(0,9) == "lightning" )
    {
        // nic nie rob - lightning jest osobno obslugiwany w show effect
        // albo nie, ustaw duration:
        eps.duration = 0.50f;
    }
	else if ( eps.name.substr(0,18) == "straight-lightning" )
    {
		eps.duration = 0.50f;
	}
    else if ( templateName == "acid" ||
              templateName == "acid-cloud" ||
              templateName == "storm-cloud" ||
			  templateName == "acid-spray" ||
              templateName == "bloodsplatter" ||
              templateName == "firearrow-trail" ||
              templateName == "frost" ||
              templateName == "heal" ||
              templateName == "iceball" ||
              templateName == "shield" ||
              templateName == "smallfireball" ||
              templateName == "stars" ||
              templateName == "tinyfireball" ||
              templateName == "green-cloud" ||
              templateName == "flames" ||
              templateName == "trail" ||
			  templateName == "smoke" ||
			  templateName == "lightball" )
    {
        eps.duration = 0.01f;
        eps.particle = templateName;
    }
    else if ( templateName == "frost-wave" )
    {
        eps.image = "data/effects/frost-wave.png";
        eps.duration = 0.16f;
        eps.scale = 6.0f;
        eps.red = 0.1f;
        eps.green = 0.5f;
        eps.blue = 1.0f;
        eps.alpha = 1.0f;
        eps.effectAnimation = "shockwave";
    }
    else if ( templateName == "fire-circle-shockwave" )
    {
        eps.rotation = gRand.Rndf( 0, 360 );
        eps.image = "data/effects/firecircle.png";
        eps.scale = 6.0f;
        eps.duration = 0.4f;
        eps.effectAnimation = "shockwave";
    }
    else if ( templateName == "ice-circle-shockwave" )
    {
        eps.rotation = gRand.Rndf( 0, 360 );
        eps.image = "data/effects/wave.png";
        eps.scale = 6.0f;
        eps.duration = 0.4f;
        eps.red = 0.1f;
        eps.green = 0.5f;
        eps.blue = 1.0f;
        eps.alpha = 1.0f;
        eps.effectAnimation = "shockwave";
    }
    else // shockwave
    {
        //gConsole.Printf( L"CGraphicalEffects::Prepare: unable to prepare `%s' - not found,"
        //         L" loading shockwave placeholder", templateName.c_str() );
        eps.image = "data/effects/wave.png";
        eps.scale = 2.0f;
        eps.effectAnimation = "shockwave";
    }

    return eps;
}

SDisplayableEffect * CGraphicalEffects::ShowEffect(const std::string & templateName, const sf::Vector2f & position) {
    SEffectParamSet eps = Prepare(templateName);
    return ShowEffect(eps, position, NULL, NULL);
}

SDisplayableEffect * CGraphicalEffects::ShowEffect(const std::string & templateName, CPhysical * from, CPhysical * to) {
    SEffectParamSet eps = Prepare(templateName);
    return ShowEffect(eps, from, to);
}

SDisplayableEffect * CGraphicalEffects::ShowEffect(
        SEffectParamSet & eps,
        CPhysical * from,
        CPhysical * to
)
{
    return ShowEffect( eps, sf::Vector2f(0,0), from, to );
}

SDisplayableEffect * CGraphicalEffects::ShowEffect(
        SEffectParamSet & eps,
        const sf::Vector2f& position,
        CPhysical * from,
        CPhysical * to )
{
    if ( !eps.image.empty() || !eps.animation.empty() )
    {
        CDisplayable * displayable = gDrawableManager.CreateDisplayable( eps.zIndex );
        if ( !displayable )
        {
            fprintf( stderr, "error: CGraphicalEffects::ShowEffect, displayable is null, returning\n" );
            return NULL;
        }

        if ( from )
        {
            displayable->SetPosition( from->GetPosition() );
            if ( eps.name == "frost-wave" )
            {
                displayable->SetRotation( (float)from->GetRotation() );
                eps.rotation = (float)from->GetRotation();
                eps.duration = 0.16f;
            }
        }
        else
            displayable->SetPosition( position );

        if ( eps.animation.empty() )
            displayable->SetStaticImage( eps.image );
        else
        {
            displayable->SetAnimation( eps.animation );
            SAnimationState * animState = displayable->GetAnimationState();
            if ( animState )
            {
                animState->isLooped = false;
                animState->isPaused = true;
            }
        }

        if ( eps.name == "blood-splat" )
        {
            float amount = eps.amount;
            if ( amount > 40.0 ) amount = 40.0;
            if ( amount < 1.0 ) amount = 1.0;
            amount = (amount * 1.5f) + 45.f;
            eps.scale = amount * 0.01f;
        }

        if ( !eps.effectAnimation.empty() )
        {
            if ( gsAnimations.find( eps.effectAnimation ) == gsAnimations.end() )
            {
                fprintf( stderr, "warning: CGraphicalEffect::Prepare - effectAnimation `%s' not found in gsAnimation map\n",
                        eps.effectAnimation.c_str() );
            }
            const EffectAnimation & animation = gsAnimations[ eps.effectAnimation ];
            if ( !animation.empty() )
            {
                const SDisplayableEffectFrame & frame = animation.front();
                displayable->SetScale( eps.scale * frame.scale );
                displayable->SetColor( 1.0f * eps.red, 1.0f * eps.green, 1.0f * eps.blue, frame.alpha * eps.alpha );
                displayable->SetRotation( eps.rotation + frame.rotation );
            }
        }

        SDisplayableEffect * effect = new SDisplayableEffect( eps, displayable, NULL);
        if ( from && eps.name != "frost-wave" )
            effect->tracedPhysical = from;
        if (eps.name == "loot-circle-perpetual")
            effect->isPerpetual = true;
        gsEffects.push_back( effect );
    
        return effect;
    }
    else if ( !eps.particle.empty() )
    {
        EffectHandle handle = CreateEffectHandle( eps.particle );
        float duration = eps.duration;
        if ( !from )
            part::gParticleManager.AddParticleSystem( handle, position, duration );            
        else
            part::gParticleManager.AddParticleSystem( handle, from, duration );
        return NULL;
    }
    else if ( eps.name == "colorize" && from )
    {
        SDisplayableEffect * effect = new SDisplayableEffect( eps, NULL, NULL );
        effect->tracedPhysical = from;
        gsEffects.push_back( effect );
     
        // tox 1 sierpnia - jesli by zrobic klase efektow i podklasy,,, to warto by bylo tutaj
        // przed returnem odpalac cos w rodzaju efekt->update( zero ) zeby sie zespawnowal,
        // pokolorwal, przeskalowal itp....   i tak wszedzie tutaj przed returnami...
        // a na razie z palca uruchamiam 'logike' efektu tutaj:
        RecalculateColorForPhysical( from );

        //gConsole.Printf(L"creating effect %p, %s", effect, eps.name.c_str());

        return effect;
    } 
	else if ( eps.name.substr(0,5) == "laser" && from )
	{
		sf::Vector2f fromPos = from->GetPosition();
		sf::Vector2f fromRot = RotationToVector((float)from->GetRotation());
		fromPos += eps.offsetX * Maths::Rotate(fromRot,90);
		fromPos += eps.offsetY * fromRot;

		ILinearEffect * linearEffect = new CStraightLightning(
                fromPos * (float)(Map::TILE_SIZE),
                (fromPos + fromRot*10.0f) * (float)(Map::TILE_SIZE),
				0.1f * (float)(Map::TILE_SIZE),
				eps.duration, 0.9f, 0.0f, 0.0f, 0.75f
        );
        SDisplayableEffect * effect = new SDisplayableEffect( eps, NULL, linearEffect );
        gsEffects.push_back( effect );

        return effect;
	}
	else if ( eps.name.substr(0,18) == "straight-lightning" && from )
	{
		float red = 1.0f;
		float green = 1.0f;
		float blue = 1.0f;		

		sf::Vector2f fromPos = from->GetPosition();
		sf::Vector2f fromRot = RotationToVector((float)from->GetRotation());
		fromPos += eps.offsetX * Maths::Rotate(fromRot,90);
		fromPos += eps.offsetY * fromRot;
		sf::Vector2f toPos;
        if (to)	toPos = to->GetPosition();
        else    toPos = position;
       
        ILinearEffect * linearEffect = new CStraightLightning(
                fromPos * (float)(Map::TILE_SIZE),
                toPos * (float)(Map::TILE_SIZE),
				0.1f * (float)(Map::TILE_SIZE),
				eps.duration, red, green, blue
        );
        SDisplayableEffect * effect = new SDisplayableEffect( eps, NULL, linearEffect );
        gsEffects.push_back( effect );

        return effect;
	}
	else if ( eps.name.substr(0,9) == "lightning" )
    {
		float red = 1.0f;
		float green = 1.0f;
		float blue = 1.0f;
		if (eps.name == "lightning-red"){
			red = 0.9f;
			green = 0.5f;
			blue = 0.5f;
		} else if (eps.name == "lightning-blue"){
			red = 0.5f;
			green = 0.5f;
			blue = 1.0f;
		}

        if ( !from /* || !to */ )
        {
            fprintf(stderr, "error: CGraphicalEffects::ShowEffect: tried to spawn lighting without valid `from'\n"); //and `to'\n"); 
            return NULL;
        }
/*        float duration = eps.Get( EffectParamNames::duration );
        fprintf(stderr,"creating lightning from: %f,%f to: %f,%f, for %f\n",
                (from->GetPosition()*(float)(Map::TILE_SIZE)).x,
                (from->GetPosition()*(float)(Map::TILE_SIZE)).y,
                (to->GetPosition()*(float)(Map::TILE_SIZE)).x,
                (to->GetPosition()*(float)(Map::TILE_SIZE)).y,
                duration
        );*/
		sf::Vector2f fromPos = from->GetPosition();
		sf::Vector2f fromRot = RotationToVector((float)from->GetRotation());
		fromPos += eps.offsetX * Maths::Rotate(fromRot,90);
		fromPos += eps.offsetY * fromRot;
        sf::Vector2f toPos;
        if (to)
            toPos = to->GetPosition();
        else
            toPos = position;
        ILinearEffect * linearEffect = new CLightning(
                fromPos*(float)(Map::TILE_SIZE),  //tox, 29 jul - na moje oko to te mnozenie przez Map::TILE_SIZE
                toPos*(float)(Map::TILE_SIZE),     //powinno wyleciec do bebechow CLighting
				eps.duration,
				red,green,blue
        );
        SDisplayableEffect * effect = new SDisplayableEffect( eps, NULL, linearEffect );
        gsEffects.push_back( effect );
        
        return effect;
    }
    return NULL;
}

void CGraphicalEffects::NoticePhysicalDestroyed( CPhysical * physical )
{
    assert ( physical != NULL );
//kwik and dirt
    for ( size_t i = 0 ; i < gsEffects.size() ; i++ )
    {
        SDisplayableEffect & effect = *gsEffects[i];

        if ( effect.tracedPhysical == physical ){
            effect.tracedPhysical = NULL;
            if ( effect.eps.removeTogetherWithTraced ) {
                effect.timeToLive = 0.0f;
                effect.isPerpetual = false;
            }
        }
    }
}

void CGraphicalEffects::RemoveEffect( SDisplayableEffect * handle ){
    assert(handle != NULL);

    for ( size_t i = 0 ; i < gsEffects.size() ; i++ )
    {
        SDisplayableEffect * effect = gsEffects[i];
        if ( effect == handle )
        {
            effect->timeToLive = 0.0f;
            effect->isPerpetual = false;
            return;
        }
    }
}

void DeleteDisplayableEffect(SDisplayableEffect * effect) {
    assert(effect != NULL);

    CDisplayable * displayable = effect->displayable;
    if ( displayable )
        gDrawableManager.DestroyDrawable(displayable);
    ILinearEffect * linearEffect = effect->linearEffect;
    if (linearEffect)
        delete linearEffect;
    delete effect;
}

void CGraphicalEffects::ClearEffects() {
    fprintf(stderr, "CGraphicalEffects::Clearing effects\n");
    for (DisplayableEffectVector::iterator it = gsEffects.begin(); it != gsEffects.end(); ++it)
    {
        SDisplayableEffect * effect = *it;
        DeleteDisplayableEffect(effect);
    }
    gsEffects.clear();

    part::gParticleManager.ClearParticles();
}
