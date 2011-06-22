#include "AnimCommands.h"
#include "CCommands.h"
#include "../Utils/StringUtils.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CDisplayable.h"
#include "../Rendering/Animations/SAnimationState.h"

#include <fstream>

// na koncu musi byc {0,0,0}, bo sie wszystko ***
CCommands::SCommandPair AnimCommands [] =
{
    {L"spawn-displayable"                   , "$MAN_SPAWN_DISPLAYABLE"                      , CommandSpawnDisplayable        },
    {L"set-displayable-image"               , "$MAN_SET_DISPLAYABLE_IMAGE"                  , CommandSetDisplayableImage         },
    {L"set-displayable-position"            , "$MAN_SET_DISPLAYABLE_POSITION"               , CommandSetDisplayablePosition      },
    {L"set-displayable-center"              , "$MAN_SET_DISPLAYABLE_CENTER"                 , CommandSetDisplayableCenter        },
    {L"set-displayable-scale"               , "$MAN_SET_DISPLAYABLE_SCALE"                  , CommandSetDisplayableScale         },
    {L"set-displayable-rect"                , "$MAN_SET_DISPLAYABLE_RECT"                   , CommandSetDisplayableRect          },
    {L"set-displayable-animation"           , "$MAN_SET_DISPLAYABLE_ANIMATION"              , CommandSetDisplayableAnimation     },
    {L"set-displayable-animation-play"      , "$MAN_SET_DISPLAYABLE_ANIMATION_PLAY"         , CommandSetDisplayableAnimationPlay },
    {L"set-displayable-animation-stop"      , "$MAN_SET_DISPLAYABLE_ANIMATION_STOP"         , CommandSetDisplayableAnimationStop },
    {L"set-displayable-animation-loop"      , "$MAN_SET_DISPLAYABLE_ANIMATION_LOOP"         , CommandSetDisplayableAnimationLoop },
    {L"set-displayable-animation-rewind-to" , "$MAN_SET_DISPLAYABLE_ANIMATION_REWIND_TO"    , CommandSetDisplayableAnimationRewindTo },
    {0,0,0}
};

CDisplayable* gTextDisplayable;

void CommandSpawnDisplayable(size_t argc, const std::vector<std::wstring> &argv)
{
    gTextDisplayable = gDrawableManager.CreateDisplayable();
}

void CommandSetDisplayableImage(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc >= 2 )
        gTextDisplayable->SetStaticImage( StringUtils::ConvertToString( argv[1] ) );
}

void CommandSetDisplayableAnimation(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc >= 2 )
        gTextDisplayable->SetAnimation( StringUtils::ConvertToString( argv[1] ) );
}

void CommandSetDisplayableAnimationPlay(size_t argc, const std::vector<std::wstring> &argv)
{
    SAnimationState* animationState = gTextDisplayable->GetAnimationState();
    if ( animationState ) // w przeciwnym wypadku displayable nie jest animowany w tym momencie
    {
        animationState->Play();
    }
}

void CommandSetDisplayableAnimationStop(size_t argc, const std::vector<std::wstring> &argv)
{
    SAnimationState* animationState = gTextDisplayable->GetAnimationState();
    if ( animationState ) // w przeciwnym wypadku displayable nie jest animowany w tym momencie
    {
        animationState->Stop();
    }
}

void CommandSetDisplayableAnimationLoop(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 2 ) return;

    SAnimationState* animationState = gTextDisplayable->GetAnimationState();
    if ( animationState ) // w przeciwnym wypadku displayable nie jest animowany w tym momencie
    {
        int loop = 0;
        swscanf( argv[1].c_str() , L"%d", &loop );
        animationState->isLooped = ( loop != 0 );
    }
}

void CommandSetDisplayableAnimationRewindTo(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 2 ) return;

    SAnimationState* animationState = gTextDisplayable->GetAnimationState();
    if ( animationState ) // w przeciwnym wypadku displayable nie jest animowany w tym momencie
    {
        float position = 0.0;
        swscanf( argv[1].c_str() , L"%f", &position );
        animationState->RewindTo( position );
    }
}



void CommandSetDisplayablePosition(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 3 ) return;

    float x, y;
    x = y = 0.0f;
    swscanf( argv[1].c_str() , L"%f", &x );
    swscanf( argv[2].c_str() , L"%f", &y );

    gTextDisplayable->SetPosition( x, y );
}

void CommandSetDisplayableRect(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 5 ) return;

    int left, top, right, bottom;
    left = top = right = bottom = 0;
    swscanf( argv[1].c_str() , L"%d", &left );
    swscanf( argv[2].c_str() , L"%d", &top );
    swscanf( argv[3].c_str() , L"%d", &right );
    swscanf( argv[4].c_str() , L"%d", &bottom );

    gTextDisplayable->SetSubRect( left, top, right, bottom );
}

void CommandSetDisplayableCenter(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 3 ) return;

    float x, y;
    x = y = 0.0f;
    swscanf( argv[1].c_str() , L"%f", &x );
    swscanf( argv[2].c_str() , L"%f", &y );

//    gTextDisplayable->GetSFSprite()->SetCenter( sf::Vector2f( x, y ) );
}

void CommandSetDisplayableScale(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 3 ) return;

    float x, y;
    x = y = 0.0f;
    swscanf( argv[1].c_str() , L"%f", &x );
    swscanf( argv[2].c_str() , L"%f", &y );

    gTextDisplayable->SetScale( x, y );
}

