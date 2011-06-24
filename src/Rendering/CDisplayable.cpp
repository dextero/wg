#include "CDisplayable.h"

#include "../CGame.h"
#include "../CGameOptions.h"

#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../ResourceManager/CImagePart.h"
#include "Animations/CAnimationManager.h"
#include "Animations/SAnimationFrame.h"
#include "Animations/SAnimationState.h"
#include "Animations/SAnimation.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

#ifdef PLATFORM_MACOSX
#include <gl.h>
#else
#include <GL/gl.h>
#endif

//rozmiar kafla:
#include "../Map/CMap.h"

// rAum, 12.04.09 - dodanie obs³ugi atlasu
// Liosan, 25.04.09 - ulepszenie obslugi atlasu

using namespace System::Resource;

CDisplayable::CDisplayable() :
    IDrawable(), mSFSprite( NULL ), mAnimationState( NULL ), mScale( 1.0, 1.0 ), mClipPlaneEnabled(false), mClipRectEnabled(false)
{
    memset(mClipPlane, 0, 4 * sizeof(double));
    memset(mClipRect, 0, 4 * sizeof(int));
    mSFSprite = new CWGSprite();
    mSFSprite->Resize( static_cast<float>(Map::TILE_SIZE), static_cast<float>(Map::TILE_SIZE) );
    mSFSprite->SetCenter( static_cast<float>(Map::TILE_SIZE) * 0.5f, static_cast<float>(Map::TILE_SIZE) * 0.5f );
	m_canDraw = true;
}

CDisplayable::~CDisplayable()
{
    delete mSFSprite;
    if ( mAnimationState )
        gAnimationManager.DestroyAnimationState( mAnimationState );
}

void CDisplayable::SetAnimation( const std::string& animationName )
{
    if (mAnimationName == animationName) return;            // Po co ³adowaæ t¹ sam¹ animacjê 2 razy :)
    // tox, 9 luty: bo fizykal strzelajacy pociskami moze chciec dla kazdego wystrzelonego pocisku odegrac
    // te sama animacje od poczatku - mozna to zrobic dobierajac sie do mAnimationState, a mozna (by) bylo
    // odpalajac te sama animacje jeszcze raz i umawiajac sie, ze to oznacza "uruchom sie animacjo od poczatku"
    //
    // z drugiej strony przydalby sie kod, ktory by podmienial aktualna animacje *bez* zmiany jej aktualnego
    // czasu - jakis taki CExhibit::SwapAniamtion, ktory by zmienial animacje na nowa i od razu przewijal
    // ja do tego samego miejsca, w ktorym byla poprzednia... (zeby uzyc tego przy zmianie np kierunku chodzenia)
    // hm..hm..hm..
    // na razie nie ruszam kodu tutaj, pozdr, tox
    // Liosan: juz logika o to zadba, zeby sie nie przelaczalo. A na razie na tym mechanizmie polega CActor::UpdateAnimations

    mAnimationName = animationName;

    if ( mAnimationState )
        gAnimationManager.DestroyAnimationState( mAnimationState );
    mAnimationState = gAnimationManager.CreateAnimationState( animationName );

    if ( !mAnimationState )
    {
        fprintf( stderr, "warning: CDisplayable::SetAnimation: unable to obtain animation `%s'\n", animationName.c_str() );
    } 
}

void CDisplayable::SetAnimation( SAnimation *animationPtr){
    if (animationPtr == NULL)
        return;
    if ((mAnimationState != NULL) && (mAnimationState->animation == animationPtr))
        return;

    mAnimationName = animationPtr->name;

    if ( mAnimationState )
        gAnimationManager.DestroyAnimationState( mAnimationState );
    mAnimationState = gAnimationManager.CreateAnimationState( animationPtr );

    if ( !mAnimationState ){
        fprintf( stderr, "warning: CDrawable::SetAnimation: unable to obtain animation '%s'\n", mAnimationName.c_str() );
    } 
}

void CDisplayable::SetStaticImage( const std::string& imageName, int number, float extraPixels )
{
    CImage* img = gResourceManager.GetImage( imageName );
    if ( !img )
    {
        fprintf( stderr, "warning: CDisplayable::SetStaticImage: unable to obtain image `%s'\n", imageName.c_str() );
		SetStaticImage(NULL,number,extraPixels);
        return;
    }

    mImageName = imageName;
    SetStaticImage(img, number, extraPixels);
}

void CDisplayable::SetStaticImage(CImage *img, int number, float extraPixels)
{
    if ( mAnimationState )
    {
        gAnimationManager.DestroyAnimationState( mAnimationState );
        mAnimationState = NULL;
    }

    mSFSprite->FlipX( false );
	if (img != NULL) {
        img->SetSmooth(false);
        mSFSprite->SetImage( *img );
		sf::IntRect rect = img->GetRect(number);
		rect.Left += (int) extraPixels;		rect.Top += (int) extraPixels;
		rect.Right -= (int) extraPixels;	rect.Bottom -= (int) extraPixels;
		mSFSprite->SetSubRect( rect );
		mSFSprite->SetCenter( (rect.Right - rect.Left) * 0.5f, (rect.Bottom - rect.Top) * 0.5f );
		mSFSprite->Resize( Map::TILE_SIZE * mScale.x, Map::TILE_SIZE * mScale.y);
	}
}

void CDisplayable::SetStaticImageFromAtlas( const std::string& atlasName, int number)
{
    CImagePart img = gResourceManager.GetImagePart( atlasName, number );
    if ( !img.GetImage() )
    {
        fprintf( stderr, "warning: CDisplayable::SetStaticImage: unable to obtain image from atlas `%s'\n", atlasName.c_str() );
        return;
    }

    SetStaticImageFromAtlas(&img);
}

void CDisplayable::SetStaticImageFromAtlas(CImagePart *img)
{
    if ( mAnimationState )
    {
        gAnimationManager.DestroyAnimationState( mAnimationState );
        mAnimationState = NULL;
    }

    mSFSprite->FlipX( false );
    mSFSprite->SetImage( **img );
    mSFSprite->SetSubRect( *img );
    mSFSprite->SetCenter( (img->GetOffset().Right - img->GetOffset().Left) * 0.5f, (img->GetOffset().Bottom - img->GetOffset().Top) * 0.5f );
    mSFSprite->Resize( Map::TILE_SIZE * mScale.x , Map::TILE_SIZE * mScale.y);
}

void CDisplayable::SetPosition( float x, float y )
{
    mSFSprite->SetPosition( x * Map::TILE_SIZE, y * Map::TILE_SIZE );
}

void CDisplayable::SetPosition( const sf::Vector2f& vec )
{
    SetPosition( vec.x, vec.y );
}

sf::Vector2f CDisplayable::GetPosition()
{
	return mSFSprite->GetPosition();
}

void CDisplayable::SetScale( float x, float y )
{
    mScale = sf::Vector2f( x, y );
    mSFSprite->Resize( Map::TILE_SIZE * mScale.x, Map::TILE_SIZE * mScale.y );
}

void CDisplayable::SetScale( float uniform )
{
    SetScale( uniform, uniform );
}

void CDisplayable::SetScale( const sf::Vector2f& newValue )
{
    mScale = newValue;
    mSFSprite->Resize( Map::TILE_SIZE * mScale.x, Map::TILE_SIZE * mScale.y );
}

sf::Vector2f CDisplayable::GetScale()
{
	return mScale;
}

void CDisplayable::SetSubRect( int left, int top, int right, int bottom )
{
    SetSubRect( sf::IntRect( left, top, right, bottom ) );
}
void CDisplayable::SetSubRect ( sf::IntRect rect )
{
    sf::Vector2f oldSize = mSFSprite->GetSize();
    mSFSprite->SetSubRect( rect );
    mSFSprite->SetCenter( (rect.Right-rect.Left) * 0.5f, (rect.Bottom-rect.Top) * 0.5f );
    mSFSprite->Resize( Map::TILE_SIZE * mScale.x, Map::TILE_SIZE * mScale.y );
}

// tox, 19 cze: e? tu jest int rot, a nie float rot? zapytac liosana
// tox, 30 jul: nie pytalem
void CDisplayable::SetRotation(float rot){
    mSFSprite->SetRotation((float)rot);
}

float CDisplayable::GetRotation()
{
    return mSFSprite->GetRotation();
}

void CDisplayable::SetColor( const sf::Color & color )
{
    mSFSprite->SetColor( color );
}

void CDisplayable::SetColor( float r, float g, float b, float a )
{
    mSFSprite->SetColor( sf::Color( (sf::Uint8)(r * 255), (sf::Uint8)(g * 255), (sf::Uint8)(b * 255), (sf::Uint8)(a * 255) ) );
}

void CDisplayable::GetColor( float* r, float* g, float* b, float* a )
{
	float m = 1.0f / 255.0f;

	*r = m * mSFSprite->GetColor().r;
	*g = m * mSFSprite->GetColor().g;
	*b = m * mSFSprite->GetColor().b;
	*a = m * mSFSprite->GetColor().a;
}

void CDisplayable::Draw( sf::RenderWindow* renderWindow )
{
	if(m_canDraw == false)
		return;

    if ( mAnimationState )
    {
        SAnimationFrame frame = mAnimationState->GetCurrentFrame();

        
        sf::Image* img = gResourceManager.GetImage( frame.imageName );
        if ( !img )
        {
            fprintf( stderr, "warning: CDisplayable::Draw: animation: unable to obtain image `%s'\n", frame.imageName.c_str() );
        }
        else
        {
            sf::IntRect rect = frame.rect;
            if ( rect.Left == 0 && rect.Top == 0 && rect.Right == 0 && rect.Bottom == 0 )
            {
                // umawiamy sie, ze (0,0,0,0) oznacza caly obrazek
                rect.Right = img->GetWidth();
                rect.Bottom = img->GetHeight();
            }

            mSFSprite->SetImage( *img );
            mSFSprite->SetSubRect( rect );
            if ( frame.offset.x != 0 || frame.offset.y != 0 ) 
            {                
                mSFSprite->SetCenter( frame.offset );

                sf::Vector2f scale = mScale;
                if ( frame.normalSize.x != 0 || frame.normalSize.y != 0 )
                {
                    scale.x *= (float)frame.normalSize.x / (float)Map::TILE_SIZE;
                    scale.y *= (float)frame.normalSize.y / (float)Map::TILE_SIZE;
                }
                scale.x *= (frame.rect.Right - frame.rect.Left);
                scale.y *= (frame.rect.Bottom - frame.rect.Top);
                mSFSprite->Resize( scale.x, scale.y );
            }
            else
            {
                mSFSprite->SetCenter( ( rect.Right - rect.Left ) * 0.5f, ( rect.Bottom - rect.Top ) * 0.5f );
                mSFSprite->Resize( Map::TILE_SIZE * mScale.x, Map::TILE_SIZE * mScale.y );
            }

            mSFSprite->FlipX( frame.isFlipped );
        }
    }

    // przycinanie
    if ( mClipPlaneEnabled )
	{
		glEnable( GL_CLIP_PLANE0 );
		glClipPlane( GL_CLIP_PLANE0, mClipPlane );
	}
	if ( mClipRectEnabled )
	{
		glEnable( GL_SCISSOR_TEST );
		glScissor( mClipRect[0], mClipRect[1], mClipRect[2], mClipRect[3] );
	}

    renderWindow->Draw( *mSFSprite );

    // wylaczenie przycinania
	glDisable( GL_CLIP_PLANE0 );
	glDisable( GL_SCISSOR_TEST );
}

SAnimationState* CDisplayable::GetAnimationState()
{
    return mAnimationState;
}

const std::string& CDisplayable::GetImageName()
{
    return mImageName;
}

void CDisplayable::SetCanDraw(bool draw){
	m_canDraw = draw;
}

bool CDisplayable::GetCanDraw(){
	return m_canDraw;
}

sf::Sprite * CDisplayable::GetSFSprite()
{
    return mSFSprite;
}

void CDisplayable::SetClipPlane( double a, double b, double c, double d )
{
	mClipPlaneEnabled = true;
	mClipPlane[0] = a;
	mClipPlane[1] = b;
	mClipPlane[2] = c;
	mClipPlane[3] = d;
}

void CDisplayable::SetClipRect(int left, int top, int right, int bottom)
{
	mClipRectEnabled = true;
	mClipRect[0] = left;
	mClipRect[1] = gGameOptions.GetHeight() - bottom;
	mClipRect[2] = right - left;
	mClipRect[3] = bottom - top;
}

void CDisplayable::SetClipRect(const sf::FloatRect &rect)
{
	SetClipRect( (int) rect.Left, (int) rect.Top, (int) rect.Right, (int) rect.Bottom );
}

