#include "CAnimationManager.h"

#include "../../CGame.h"
#include "SAnimationState.h"
#include "SAnimationFrame.h"
#include "SAnimation.h"

#include <SFML/Graphics/Sprite.hpp>

#include "../../Commands/CCommands.h"
#include "../../Commands/AnimCommands.h"

#include "../../Utils/StringUtils.h"
#include <fstream>

template<> CAnimationManager* CSingleton<CAnimationManager>::msSingleton = 0;

CAnimationManager::CAnimationManager(){
    fprintf( stderr, "CAnimationManager::CAnimationsManager()\n" );
    gGame.AddFrameListener( this );

//    //zaslepka:
//    // niech sobie tutaj pozostanie, moze ktos kiedy bedzie chcial od reki tworzyc animacje
//    // to przynajmniej zobaczy przyklady:
//
//    SAnimationFrame frame1( "data/physicals/hero.png", sf::IntRect( 48, 0, 72, 24 ) );
//    SAnimationFrame frame2( "data/physicals/hero.png", sf::IntRect( 48, 24, 72, 48 ) );
//    SAnimationFrame frame3( "data/physicals/hero.png", sf::IntRect( 48, 48, 72, 72 ) );
//    SAnimationFrame frame4( "data/physicals/hero.png", sf::IntRect( 96, 0, 120, 24 ) );
//
//    STimedAnimationFrame tframe1( 0.25f, frame1 );
//    STimedAnimationFrame tframe2( 0.5f, frame2 );
//    STimedAnimationFrame tframe3( 0.75f, frame3 );
//    STimedAnimationFrame tframe4( 1.0f, frame4 );
//
//    SAnimation* animation1 = new SAnimation;
//    animation1->frames.push_back( tframe1 );
//    animation1->frames.push_back( tframe2 );
//    animation1->frames.push_back( tframe3 );
//    animation1->frames.push_back( tframe4 );
//
//    mAnimations[ "testowa.anim1" ] = animation1;
//
/////////////// baron of hell:
//
//    bool flipped = true;
//    SAnimation* animation2 = new SAnimation;
//    animation2->frames.push_back( STimedAnimationFrame( 0.250f, 
//                SAnimationFrame( "data/physicals/doom-baron.png", sf::IntRect( 12, 6, 53, 81 ), sf::Vector2f( 32, 40 ) ) ) );
//    animation2->frames.push_back( STimedAnimationFrame( 0.500f, 
//                SAnimationFrame( "data/physicals/doom-baron.png", sf::IntRect( 8, 91, 59, 168 ), sf::Vector2f( 31, 126 ) ) ) );
//    animation2->frames.push_back( STimedAnimationFrame( 0.750f, 
//                SAnimationFrame( "data/physicals/doom-baron.png", sf::IntRect( 13, 184, 57, 260 ), sf::Vector2f( 37, 217 ) ) ) );
//    animation2->frames.push_back( STimedAnimationFrame( 1.000f, 
//                SAnimationFrame( "data/physicals/doom-baron.png", sf::IntRect( 12, 271, 63, 347 ), sf::Vector2f( 39, 306 ) ) ) );
//
//    mAnimations[ "baron/move/down.anim1" ] = animation2;
//
//    animation2 = new SAnimation;
//    animation2->frames.push_back( STimedAnimationFrame( 0.250f, 
//                SAnimationFrame( "data/physicals/doom-baron.png", sf::IntRect( 199, 363, 267, 436 ), sf::Vector2f( 241, 393 ) ) ) );
//    animation2->frames.push_back( STimedAnimationFrame( 0.500f, 
//                SAnimationFrame( "data/physicals/doom-baron.png", sf::IntRect( 74, 449, 145, 521 ), sf::Vector2f( 119, 480 ) ) ) );
//    animation2->frames.push_back( STimedAnimationFrame( 0.750f, 
//                SAnimationFrame( "data/physicals/doom-baron.png", sf::IntRect( 20, 622, 77, 687 ), sf::Vector2f( 44, 648 ) ) ) );
//
//    mAnimations[ "baron/attack/down.anim1" ] = animation2;



    std::ifstream infile( "data/physicals/all.anims" );
    if ( infile.fail() || !infile.good() || infile.bad() )
    {
        fprintf( stderr, "error: CAnimationManager::CAnimationManager() - unable to open animations file\n" );
    }
    else
    {
        std::string animationName;
        std::string line;
        unsigned int lineNo = 0;
        float totalTime = 0;
        SAnimation* animation = NULL;

        bool isInBlock = false;

        while ( !infile.eof() )
        {
            getline (infile, line); lineNo++;
            line = StringUtils::TrimWhiteSpaces( line );

            if ( line.empty() ) continue;
            if ( line.find( '#' ) == 0 ) continue;
            if ( line.find( "//" ) == 0 ) continue;

            if ( line.find( "{" ) == 0 )
            {
                if ( isInBlock )
                    fprintf( stderr, "warning: CAnimationManager::ReadFromFile found double { at line %d\n", lineNo );
                else
                {
                    isInBlock = true;
                    animation = new SAnimation();
                    totalTime = 0;
                }
                continue;
            }
            if ( line.find( "}" ) == 0 )
            {
                if ( !isInBlock )
                    fprintf( stderr, "warning: CAnimationManager::ReadFromFile found closing } outside block at line %d\n", lineNo );
                else
                {
                    isInBlock = false;

                    if ( mAnimations.find( animationName ) == mAnimations.end() )
                         mAnimations[ animationName ] = animation;
                    else
                    {
                        fprintf( stderr, "warning: CAnimationManager::ReadFromFile - tried to assign existing SAnimation of name"
                                " %s, discarding\n", animationName.c_str() );
                        delete animation;
                        animation = NULL;
                    }
                }
                continue;
            }

            if ( !isInBlock )
            {
                animationName = line;
            }
            else
            {
                float frameLength;
                std::string imageName;
                sf::IntRect rect;
                sf::Vector2f offset;
                int isFlipped = 0;
                sf::Vector2i normalSize;

                sscanf( line.c_str() ,"%f %*s %d %d %d %d %f %f %d %d %d", 
                        &frameLength, 
                        &(rect.Left), &(rect.Top), &(rect.Right), &(rect.Bottom),
                        &(offset.x), &(offset.y),
                        &isFlipped,
                        &(normalSize.x), &(normalSize.y)
                        );
                imageName = line.substr( line.find( "\"" ) );
                if ( !imageName.empty() ) imageName = imageName.substr( 1 );
                imageName = imageName.substr( 0, imageName.find( "\"" ) );

//                fprintf( stderr, "lineNo: %d, %f %s %d %d %d %d - %f %f - %d - %d %d\n" , lineNo,
//                    frameLength,
//                    imageName.c_str(),
//                    rect.Left, rect.Top, rect.Right, rect.Bottom,
//                    offset.x, offset.y,
//                    isFlipped,
//                    normalSize.x, normalSize.y
//                    );
    
                totalTime += frameLength;
                animation->frames.push_back( STimedAnimationFrame( totalTime, 
                SAnimationFrame( imageName, rect, offset, ( isFlipped != 0 ), normalSize ) ) );
                animation->name = animationName;
            }    
        }


    }





//    //diagnostyka:
//    for ( std::map< std::string, SAnimation* >::iterator it = mAnimations.begin() ; it != mAnimations.end() ; it++ )
//    {
//        fprintf( stderr, "mAnimations[ %s ]\n{\n", it->first.c_str() );
//        SAnimation* anim = it->second;
//        for ( size_t i = 0 ; i < anim->frames.size() ; i++ )
//        {
//            STimedAnimationFrame tframe = anim->frames[i];
//            fprintf( stderr, "tframe( time = %f ", tframe.time );
//            SAnimationFrame frame = tframe.frame;
//            fprintf( stderr, "frame( file = %s, rect( %d, %d, %d, %d ), offset( %f, %f ), normalSize( %d, %d ), isFlipped(%d) ) )\n", 
//                  frame.imageName.c_str(), frame.rect.Left, frame.rect.Top, frame.rect.Right, frame.rect.Bottom,
//                  frame.offset.x, frame.offset.y, frame.normalSize.x, frame.normalSize.y, (int)frame.isFlipped );
//        }
//        fprintf( stderr, "}\n" );
//    }

    gCommands.RegisterCommands(AnimCommands);
}

CAnimationManager::~CAnimationManager(){
    fprintf( stderr,"CAnimationManager::~CAnimationManager()\n" );

    for ( std::set< SAnimationState* >::iterator it = mAnimationStates.begin(); it != mAnimationStates.end() ; it++ )
    {
        delete (*it);
    }
    mAnimationStates.clear();

    for ( std::map< std::string, SAnimation* >::iterator it = mAnimations.begin() ; it != mAnimations.end() ; it++ )
    {
        delete (it->second);
    }
    mAnimations.clear();
}

void CAnimationManager::FrameStarted( float secondsPassed )
{
    for ( std::set< SAnimationState* >::iterator it = mAnimationStates.begin() ; it != mAnimationStates.end() ; it++ )
    {
        SAnimationState* state = (*it);
        if ( !state->isPaused )
            state->AddTime( secondsPassed );
    }
}

SAnimation* CAnimationManager::GetAnimation(const std::string& animationName){
    SAnimation* animationPtr = NULL;
    if ( mAnimations.find( animationName ) != mAnimations.end() )
        animationPtr = mAnimations[ animationName ]; // poprawic

    if ( animationPtr == NULL )
    {
        if (animationName != "")
            fprintf( stderr, "warning: CAnimationManager::CreateAnimationState - unable to find `%s' animation in mAnimations map\n", animationName.c_str() );
        return NULL;
    }
    return animationPtr;
}

SAnimationState* CAnimationManager::CreateAnimationState( const std::string& animationName )
{
    SAnimation* animationPtr = GetAnimation(animationName);

    return CreateAnimationState(animationPtr);
}

SAnimationState* CAnimationManager::CreateAnimationState( SAnimation *animationPtr )
{
    if (animationPtr == NULL)
        return NULL;

    SAnimationState* animationState = new SAnimationState( animationPtr);
    
    mAnimationStates.insert( animationState );

    return animationState;
}

void CAnimationManager::DestroyAnimationState( SAnimationState* animationState )
{
    if (!animationState)
    {
        fprintf( stderr, "warning: CAnimationManager::DestroyAnimationState - animationState was NULL\n" );
        return;
    }

    if ( mAnimationStates.find( animationState ) != mAnimationStates.end() )
    {
        mAnimationStates.erase( mAnimationStates.find( animationState ) );
        delete animationState;
        return;
    }
    else
    {
        fprintf( stderr, "error: CAnimationManager::DestroyAnimationState - animationState was not found in set\n" );
        return;
    }
}

