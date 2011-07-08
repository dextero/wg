#include "CDrawableManager.h"

#include "CHudSprite.h"
#include "CHudStaticText.h"
#include "CDisplayable.h"

#include "IDrawable.h"
#include "ZIndexVals.h"

#include "../CGame.h"
#include <SFML/Graphics/Image.hpp>

template<> CDrawableManager* CSingleton<CDrawableManager>::msSingleton = 0;

CDrawableManager::CDrawableManager(){
    fprintf(stderr,"CDrawableManager::CDrawableManager()\n");
    mLayers.resize( Z_MAX + 1 );
    gGame.AddFrameListener( this );
}

CDrawableManager::~CDrawableManager(){
    fprintf(stderr,"CDrawableManager::~CDrawbleManager()\n");
    
    for ( DrawableLists::iterator it1 = mLayers.begin() ; it1 != mLayers.end() ; it1++ )
    {
        /*DrawableList& list = ( *it1 );
        for ( DrawableList::iterator it2 = list.begin() ; it2 != list.end() ; it2++ )
        {
            //delete ( *it2 ); // w zasadzie czemu ma byc skasowany ;)
        }*/                    // skoro tak, to po jaka cholere byla ta petla..?
        ( *it1 ).clear();
    }
    mLayers.clear();
}

void CDrawableManager::DestroyDrawable( IDrawable* drawable )
{
    if (drawable != NULL){
        UnregisterDrawable(drawable);
        delete drawable;
    } else
        fprintf( stderr, "warning: CDrawableManager::DestroyDrawable: tried to destroy null drawable\n" );
}

void CDrawableManager::UnregisterDrawable( IDrawable *drawable)
{
    if ((drawable == NULL) || (drawable->mZIndex < 0) || (drawable->mZVectorIndex < 0))
        return;
    int zi = drawable->mZIndex, vi = drawable->mZVectorIndex;
    mLayers[zi][vi] = mLayers[zi][mLayers[zi].size()-1];
    mLayers[zi][vi]->mZVectorIndex = vi;
    mLayers[zi].pop_back();
    drawable->mZIndex = -1;
    drawable->mZVectorIndex = -1;
}

bool IsCorrectZIndex( int zIndex )
{
    if ( zIndex > Z_MAX )
    {
        fprintf( stderr, "warning, CDrawableManager::CreateMooMoo, zIndex > Z_MAX; returning null\n" );
        return false;
    }
    if ( zIndex < 0 )
    {
        fprintf( stderr, "warning: CDrawableManager::CreateMooMoo, zIndex < 0 ; returning null\n" );
        return false;
    }
    return true;
}

void CDrawableManager::RegisterDrawable( IDrawable* drawable, int zIndex )
{
    if ( !IsCorrectZIndex( zIndex ) ) return;
    if (drawable == NULL) return;

    if (drawable->mZIndex >= 0)
        UnregisterDrawable(drawable);
    mLayers[ zIndex ].push_back( drawable );
    drawable->mZIndex = zIndex;
    drawable->mZVectorIndex = (int)(mLayers[zIndex].size()-1);
}

CHudSprite* CDrawableManager::CreateHudSprite( int zIndex )
{
    if ( !IsCorrectZIndex( zIndex ) ) return NULL;

    CHudSprite* hudSprite = new CHudSprite();
    RegisterDrawable(hudSprite, zIndex);

    return hudSprite;
}

CHudSprite* CDrawableManager::CloneHudSprite(CHudSprite* sprite, int zIndex)
{
    if (!sprite)
        return NULL;

    CHudSprite* hudSprite = new CHudSprite();
    RegisterDrawable(hudSprite, (zIndex == -1 ? sprite->GetZIndex() : zIndex));
    hudSprite->GetSFSprite()->SetImage(*sprite->GetSFSprite()->GetImage());

    return hudSprite;
}

CHudStaticText* CDrawableManager::CreateHudStaticText( int zIndex )
{
    if ( !IsCorrectZIndex( zIndex ) ) return NULL;

    CHudStaticText* hudStaticText = new CHudStaticText();
    RegisterDrawable( hudStaticText, zIndex );

    return hudStaticText;
}

CDisplayable* CDrawableManager::CreateDisplayable( int zIndex )
{
    if ( !IsCorrectZIndex( zIndex ) ) return NULL;

    CDisplayable* displayable = new CDisplayable();
    RegisterDrawable( displayable, zIndex );

    return displayable;
}

#ifdef WG_SHADERS
#include "CShaderManager.h"
#include "../Utils/Maths.h"
#endif /* WG_SHADERS */

void CDrawableManager::DrawFrame(sf::RenderWindow* wnd)
{
	// do edytora
	if (wnd == NULL)
		wnd = gGame.GetRenderWindow();

    for ( DrawableLists::reverse_iterator it1 = mLayers.rbegin() ; it1 != mLayers.rend() ; it1++ )
    {
        const DrawableList& list = ( *it1 );
        for ( DrawableList::const_iterator it2 = list.begin() ; it2 != list.end() ; it2++ )
        {
            IDrawable* drawable = ( *it2 );
#ifdef WG_SHADERS
			int z = drawable->GetZIndex();
			bool useNM = (z <= Z_TILE && z >= Z_PLAYER && z != Z_SHADOWS) || z == Z_MAPSPRITE_FG;
			if (useNM){
				gShaderManager.activate("test");
                const sf::Image* img = ((CDisplayable*)drawable)->GetSFSprite()->GetImage();
                if (img)
                    gShaderManager.setUniform("uTexSize", sf::Vector2f(img->GetWidth(), img->GetHeight()));
				if (dynamic_cast<CDisplayable*>(drawable)){
					float rot = dynamic_cast<CDisplayable*>(drawable)->GetRotation();
					gShaderManager.setUniform("lpos", sf::Vector3f(
						Maths::Rotate(Maths::VectorUp(), rot).x, 
						-Maths::Rotate(Maths::VectorUp(), rot).y, 
						0.5f)
					);
				} else {
					gShaderManager.setUniform("lpos", sf::Vector3f(-0.7f, -0.7f, 0.5f));	
				}
				gShaderManager.setUniform("lcolor", sf::Color(255,255,255,255));
				float normalStrength = 1.0f;
				if (z == Z_PHYSICAL) normalStrength = 3.0f;
				else if (z == Z_TILE) normalStrength = 0.5f;
				gShaderManager.setUniform("normalStrength", normalStrength);
			}
#endif /* WG_SHADERS */
            if (drawable->IsVisible())
                drawable->Draw( wnd );
#ifdef WG_SHADERS
			gShaderManager.activate("");
#endif /* WG_SHADERS */
        } 
    }
}


