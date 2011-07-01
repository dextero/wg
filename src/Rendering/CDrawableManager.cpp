#include "CDrawableManager.h"

#include "CHudSprite.h"
#include "CHudStaticText.h"
#include "CDisplayable.h"

#include "IDrawable.h"
#include "ZIndexVals.h"

#include "../CGame.h"

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
			if (drawable->GetZIndex() == Z_PLAYER){
				gShaderManager.activate("test");
			}
#endif /* WG_SHADERS */
            if (drawable->IsVisible())
                drawable->Draw( wnd );
#ifdef WG_SHADERS
			if (drawable->GetZIndex() == Z_PLAYER){
				gShaderManager.activate("");
			}
#endif /* WG_SHADERS */
        } 
    }
}


