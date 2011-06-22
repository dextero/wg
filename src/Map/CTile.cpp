#include <cstdio>
#include <cassert>

#include "CTile.h"

#include "../Rendering/CDisplayable.h"
#include "../Rendering/ZIndexVals.h"
#include "../Rendering/CDrawableManager.h"
#include "CMap.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace Map{
		
	CTile::CTile(const std::string &code) : mDisplayable(NULL), mCode(code)
	{
	}

	CTile::~CTile()
	{
		if ( mDisplayable != NULL )
			gDrawableManager.DestroyDrawable( mDisplayable );
	}

	void CTile::InitializeTile(std::string &imageFile, int number){
		if ( mDisplayable == NULL )
			mDisplayable = gDrawableManager.CreateDisplayable( Z_TILE );
        mDisplayable->SetStaticImage( imageFile, number, 0.0f );
		mDisplayable->SetVisible( false );
		mDisplayable->SetScale( 1.01f, 1.01f );
	}
	
	void CTile::SetPosition(float x, float y){
		if ( mDisplayable != NULL ) 
			mDisplayable->SetPosition( x,y );
	}

	void CTile::SetVisible(bool visible) {
		if ( mDisplayable != NULL ) 
			mDisplayable->SetVisible(visible);
	}
}

