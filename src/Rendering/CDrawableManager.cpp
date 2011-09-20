#include "CDrawableManager.h"

#include "CHudSprite.h"
#include "CHudStaticText.h"
#include "CDisplayable.h"
#include "SLight.h"

#include "IDrawable.h"
#include "ZIndexVals.h"

#include "../CGame.h"
#include "CCamera.h"
#include "CShaderManager.h"
#include "../Utils/Maths.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector3.hpp>

#define INIT_LIGHTS_GEOMETRY_SIZE 8

template<> CDrawableManager* CSingleton<CDrawableManager>::msSingleton = 0;

CDrawableManager::CDrawableManager() :
	mNormalMappingContrast(1.0f),
	mLightingEnabled(false),
	mLightsGeometrySize(INIT_LIGHTS_GEOMETRY_SIZE)
{
    fprintf(stderr,"CDrawableManager::CDrawableManager()\n");
	mLightsGeometry = new vertexDesc[mLightsGeometrySize * 6];
	FillLightsUV();

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

void CDrawableManager::DrawFrame(sf::RenderWindow* wnd)
{
	// do edytora
	if (wnd == NULL)
		wnd = gGame.GetRenderWindow();

	bool shaderLighting = mLightingEnabled && gShaderManager.shadersAvailable();
	bool fixedPipelineLighting = mLightingEnabled && !shaderLighting;

	if (fixedPipelineLighting)
	{
		// damork, TODO: kod renderujacy do tekstury quady ze swiatlami
	}

    for ( DrawableLists::reverse_iterator it1 = mLayers.rbegin() ; it1 != mLayers.rend() ; it1++ )
    {
        const DrawableList& list = ( *it1 );
        for ( DrawableList::const_iterator it2 = list.begin() ; it2 != list.end() ; it2++ )
        {
            IDrawable* drawable = ( *it2 );
			if (drawable->IsVisible())
			{
				int z = drawable->GetZIndex();
				CDisplayable * displayable = dynamic_cast<CDisplayable *>(drawable);
				
				if (shaderLighting && displayable != NULL && displayable->GetSFSprite()->GetImage() != NULL && 
					z >= Z_MAPSPRITE_FG && z != Z_SHADOWS && z != Z_PARTICLE)
				{
					const sf::Image * normalmap = displayable->GetSFSprite()->GetNormalMap();
					if (normalmap != NULL)	DrawWithNormalMapping(wnd, displayable, normalmap);
					else					DrawWithPerPixelLighting(wnd, displayable);
				}
				else
				{
					gShaderManager.activateDefault();
					drawable->Draw( wnd );
				}
			}
        } 
    }
}

SLight* CDrawableManager::CreateLight()
{
	/* Sprawdz czy mamy wystarczajaco duzo miejsca w 'buforze geometrii' zeby pomiescic nowe swiatlo
	 * Jesli nie - zaalokuj dwa razy wiecej niz obecnie. [Potrzebne dla swiatelek na fixedpipeline] */
	if (mLights.size() >= mLightsGeometrySize)
	{
		mLightsGeometrySize *= 2;
		delete [] mLightsGeometry;
		mLightsGeometry = new vertexDesc[mLightsGeometrySize * 6];
		FillLightsUV();
	}

	SLight* light = new SLight();
	mLights.push_back(light);
	return light;
}
 
void CDrawableManager::DestroyLight(SLight *light)
{
	for (unsigned i = 0; i < mLights.size(); i++)
	if (mLights[i] == light)
	{
		std::swap(mLights[i], mLights[mLights.size()-1]);
		mLights.pop_back();
		delete light;
		return;
	}
}

void CDrawableManager::DestroyAllLights()
{
	for (unsigned i = 0; i < mLights.size(); i++)
		delete mLights[i];
	mLights.clear();
}

inline float lenSQ(const sf::Vector2f& p1, const sf::Vector3f& p2) {
	return (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) + (p2.z)*(p2.z);
}

inline bool strongerLight(const sf::Vector2f& pos, SLight* l1, SLight* l2) {
	return lenSQ(pos,l1->mPosition) * l2->mRadius < lenSQ(pos,l2->mPosition) * l1->mRadius;
}

void CDrawableManager::GetStrongestLights(SLight** out, unsigned count, const sf::Vector2f& pos)
{
	if (mLights.size() <= count)
	{
		unsigned i = 0;
		for (;i < mLights.size(); i++)	out[i] = mLights[i];
		for (;i < count; i++)			out[i] = SLight::BLACK_LIGHT();
	}
	else
	{
		SLight* current;
		unsigned last = 0;

		out[0] = mLights[0];
		for (unsigned i = 1; i < mLights.size(); i++)
		{
			current = mLights[i];

			if (last < count - 1)
				out[++last] = current;
			else if (strongerLight(pos, current, out[last]))
				out[last] = current;
			else continue;

			for (unsigned j = last; j > 0 && strongerLight(pos, current, out[j-1]); j--)
			{
				out[j] = out[j-1];
				out[j-1] = current;
			}
		}
	}
}

void CDrawableManager::DrawWithNormalMapping(sf::RenderWindow* wnd, CDisplayable* displayable, const sf::Image* normalmap)
{
	int id = gShaderManager.activate("normalmapping");
	if (id >= 0)
	{
		SLight* lights[3];
		this->GetStrongestLights(lights, 3, displayable->GetPosition());
		gShaderManager.setUniform(id, "lpos[0]", lights[0]->mPosition);
		gShaderManager.setUniform(id, "lpos[1]", lights[1]->mPosition);
		gShaderManager.setUniform(id, "lpos[2]", lights[2]->mPosition);
		gShaderManager.setUniform(id, "lcolor[0]", lights[0]->mColor);
		gShaderManager.setUniform(id, "lcolor[1]", lights[1]->mColor);
		gShaderManager.setUniform(id, "lcolor[2]", lights[2]->mColor);
		gShaderManager.setUniform(id, "lradius[0]", lights[0]->mRadius);
		gShaderManager.setUniform(id, "lradius[1]", lights[1]->mRadius);
		gShaderManager.setUniform(id, "lradius[2]", lights[2]->mRadius);
		gShaderManager.setUniform(id, "ambient", mAmbient);
		gShaderManager.setUniform(id, "nmcontrast", mNormalMappingContrast);
		gShaderManager.setUniform(id, "invModelMatrix", displayable->GetSFSprite()->GetInverseMatrix());
		gShaderManager.bindTexture(id, "normalmap", normalmap);
	}
	displayable->Draw(wnd);
	gShaderManager.clearBoundTextures();
}

void CDrawableManager::DrawWithPerPixelLighting(sf::RenderWindow *wnd, CDisplayable *displayable)
{
	int id = gShaderManager.activate("perpixel-lighting");
	if (id >= 0)
	{
		SLight* lights[5];
		this->GetStrongestLights(lights, 5, displayable->GetPosition());
		gShaderManager.setUniform(id, "lpos[0]", lights[0]->mPosition);
		gShaderManager.setUniform(id, "lpos[1]", lights[1]->mPosition);
		gShaderManager.setUniform(id, "lpos[2]", lights[2]->mPosition);
		gShaderManager.setUniform(id, "lpos[3]", lights[3]->mPosition);
		gShaderManager.setUniform(id, "lpos[4]", lights[4]->mPosition);
		gShaderManager.setUniform(id, "lcolor[0]", lights[0]->mColor);
		gShaderManager.setUniform(id, "lcolor[1]", lights[1]->mColor);
		gShaderManager.setUniform(id, "lcolor[2]", lights[2]->mColor);
		gShaderManager.setUniform(id, "lcolor[3]", lights[3]->mColor);
		gShaderManager.setUniform(id, "lcolor[4]", lights[4]->mColor);
		gShaderManager.setUniform(id, "lradius[0]", lights[0]->mRadius);
		gShaderManager.setUniform(id, "lradius[1]", lights[1]->mRadius);
		gShaderManager.setUniform(id, "lradius[2]", lights[2]->mRadius);
		gShaderManager.setUniform(id, "lradius[3]", lights[3]->mRadius);
		gShaderManager.setUniform(id, "lradius[4]", lights[4]->mRadius);
		gShaderManager.setUniform(id, "ambient", mAmbient);
	}
	displayable->Draw(wnd);
}

void CDrawableManager::FillLightsUV()
{
	for (unsigned i = 0; i < mLightsGeometrySize; i++)
	{
		mLightsGeometry[i * 6 + 0].u = 0.0f;
		mLightsGeometry[i * 6 + 0].v = 0.0f;
		mLightsGeometry[i * 6 + 1].u = 1.0f;
		mLightsGeometry[i * 6 + 1].v = 0.0f;
		mLightsGeometry[i * 6 + 2].u = 0.0f;
		mLightsGeometry[i * 6 + 2].v = 1.0f;
		mLightsGeometry[i * 6 + 3].u = 0.0f;
		mLightsGeometry[i * 6 + 3].v = 1.0f;
		mLightsGeometry[i * 6 + 4].u = 1.0f;
		mLightsGeometry[i * 6 + 4].v = 0.0f;
		mLightsGeometry[i * 6 + 5].u = 1.0f;
		mLightsGeometry[i * 6 + 5].v = 1.0f;
	}
}

void CDrawableManager::DrawLightsGeometry()
{
	sf::Vector2f cameraPos = gCamera.GetViewTopLeft();

	for (unsigned i = 0; i < mLights.size(); i++)
	{
		
	}
}