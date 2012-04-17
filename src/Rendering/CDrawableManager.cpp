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
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../CGameOptions.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

// poczatkowy rozmiar bufora na geometrie swiatla (w swiatlojednostkach)
#define INIT_LIGHTS_GEOMETRY_SIZE 8

template<> CDrawableManager* CSingleton<CDrawableManager>::msSingleton = 0;

CDrawableManager::CDrawableManager() :
	mNormalMappingContrast(1.0f),
	mLightingEnabled(false),
	mLightsGeometrySize(INIT_LIGHTS_GEOMETRY_SIZE)
{
    fprintf(stderr,"CDrawableManager::CDrawableManager()\n");
	mLightTexture = gResourceManager.GetImage("data/effects/light.png");
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
	/* Z automatu nie oswietlamy GUI, cieni i particli */
	displayable->SetLighted(zIndex >= Z_MAPSPRITE_FG && zIndex != Z_SHADOWS && zIndex != Z_PARTICLE);

    return displayable;
}

void CDrawableManager::DrawFrame(sf::RenderWindow* wnd)
{
	// do edytora
	if (wnd == NULL)
		wnd = gGame.GetRenderWindow();

	bool shaderLighting = mLightingEnabled && gShaderManager.shadersAvailable() && gGameOptions.GetShaders();
	bool fixedPipelineLighting = mLightingEnabled && !shaderLighting;

	if (fixedPipelineLighting)
	{
		// damork, TODO: kod renderujacy do tekstury quady ze swiatlami
	}

	/* TEMP - do testow 
	DrawLightsGeometry();
	for ( DrawableLists::reverse_iterator it1 = mLayers.rbegin() ; it1 != mLayers.rend() ; it1++ )
    {
        const DrawableList& list = ( *it1 );
        for ( DrawableList::const_iterator it2 = list.begin() ; it2 != list.end() ; it2++ )
        {
            IDrawable* drawable = ( *it2 );
			if (drawable->IsVisible())
			{
				int z = drawable->GetZIndex();
				if (z >= Z_TILE) continue;
				gShaderManager.activateDefault();
				drawable->Draw( wnd );
			}
        } 
    }
	return;*/

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
				
				if (shaderLighting && displayable != NULL && displayable->GetLighted() &&
					displayable->GetSFSprite()->GetImage() != NULL)
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

#define COLOR_TO_FLOATS(c) { float(c.r)/255, float(c.g)/255, float(c.b)/255, float(c.a)/255 }

void CDrawableManager::DrawWithNormalMapping(sf::RenderWindow* wnd, CDisplayable* displayable, const sf::Image* normalmap)
{
	int id = gShaderManager.activate("normalmapping");
	if (id >= 0)
	{
		SLight* lights[3];
		this->GetStrongestLights(lights, 3, displayable->GetPosition());

		sf::Vector3f position[] = { 
			lights[0]->mPosition, 
			lights[1]->mPosition, 
			lights[2]->mPosition };

		float color[3][4] = {
			COLOR_TO_FLOATS(lights[0]->mColor),
			COLOR_TO_FLOATS(lights[1]->mColor),
			COLOR_TO_FLOATS(lights[2]->mColor) };

		float radius[] = { 
			lights[0]->mRadius, 
			lights[1]->mRadius, 
			lights[2]->mRadius };

		gShaderManager.setUniformArray(id, "lpos", position, 3);
		gShaderManager.setUniform4fArray(id, "lcolor", (float*) color, 3);
		gShaderManager.setUniformArray(id, "lradius", radius, 3);
		gShaderManager.setUniform(id, "ambient", mNormalMappingAmbient);
		gShaderManager.setUniform(id, "nmcontrast", mNormalMappingContrast);
		gShaderManager.bindTexture0(id, "tex");
		gShaderManager.bindTexture1(id, "normalmap", normalmap);
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

		sf::Vector3f position[] = { 
			lights[0]->mPosition, 
			lights[1]->mPosition, 
			lights[2]->mPosition,
			lights[3]->mPosition,
			lights[4]->mPosition };

		float color[5][4] = {
			COLOR_TO_FLOATS(lights[0]->mColor),
			COLOR_TO_FLOATS(lights[1]->mColor),
			COLOR_TO_FLOATS(lights[2]->mColor),
			COLOR_TO_FLOATS(lights[3]->mColor),
			COLOR_TO_FLOATS(lights[4]->mColor) };

		float radius[] = { 
			lights[0]->mRadius, 
			lights[1]->mRadius, 
			lights[2]->mRadius,
			lights[3]->mRadius,
			lights[4]->mRadius };

		gShaderManager.setUniformArray(id, "lpos", position, 5);
		gShaderManager.setUniform4fArray(id, "lcolor", (float*) color, 5);
		gShaderManager.setUniformArray(id, "lradius", radius, 5);
		gShaderManager.setUniform(id, "ambient", mAmbient);
		gShaderManager.bindTexture0(id, "tex");
	}
	displayable->Draw(wnd);
}

void CDrawableManager::FillLightsUV()
{
	memset(mLightsGeometry, 0, sizeof(vertexDesc) * mLightsGeometrySize * 6);
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
	const sf::FloatRect& cameraRect = gGame.GetRenderWindow()->GetDefaultView().GetRect();
	unsigned quadsToRender = 0;

	for (unsigned i = 0; i < mLights.size(); i++)
	{
		SLight* l = mLights[i];
		SLight l2 = *(mLights[i]);
		float radius = sqrtf( std::max( 0.0f, l->mRadius * l->mRadius - l->mPosition.z * l->mPosition.z ) );

		if (l->mPosition.x + radius > cameraRect.Left &&
			l->mPosition.y + radius > cameraRect.Top &&
			l->mPosition.x - radius < cameraRect.Right &&
			l->mPosition.y - radius < cameraRect.Bottom)
		{
			mLightsGeometry[quadsToRender * 6 + 0].x = l->mPosition.x - radius;
			mLightsGeometry[quadsToRender * 6 + 0].y = l->mPosition.y - radius;
			mLightsGeometry[quadsToRender * 6 + 1].x = l->mPosition.x + radius;
			mLightsGeometry[quadsToRender * 6 + 1].y = l->mPosition.y - radius;
			mLightsGeometry[quadsToRender * 6 + 2].x = l->mPosition.x - radius;
			mLightsGeometry[quadsToRender * 6 + 2].y = l->mPosition.y + radius;
			mLightsGeometry[quadsToRender * 6 + 3].x = l->mPosition.x - radius;
			mLightsGeometry[quadsToRender * 6 + 3].y = l->mPosition.y + radius;
			mLightsGeometry[quadsToRender * 6 + 4].x = l->mPosition.x + radius;
			mLightsGeometry[quadsToRender * 6 + 4].y = l->mPosition.y - radius;
			mLightsGeometry[quadsToRender * 6 + 5].x = l->mPosition.x + radius;
			mLightsGeometry[quadsToRender * 6 + 5].y = l->mPosition.y + radius;

			for (unsigned j = 0; j < 6; j++)
			{
				mLightsGeometry[quadsToRender * 6 + j].r = l->mColor.r;
				mLightsGeometry[quadsToRender * 6 + j].g = l->mColor.g;
				mLightsGeometry[quadsToRender * 6 + j].b = l->mColor.b;
				mLightsGeometry[quadsToRender * 6 + j].a = l->mColor.a;
			}

			++ quadsToRender;
		}
	}

	glPushAttrib(GL_VIEWPORT_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-cameraRect.Left, -cameraRect.Top, 0.0f);
	glViewport(0, 0, 512, 512);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);

	if (quadsToRender > 0)
	{
		if (mLightTexture != NULL)
			mLightTexture->Bind();

		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );

		glVertexPointer( 2, GL_FLOAT, sizeof(vertexDesc), &(mLightsGeometry[0].x) );
		glTexCoordPointer( 2, GL_FLOAT, sizeof(vertexDesc), &(mLightsGeometry[0].u) );
		glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(vertexDesc), &(mLightsGeometry[0].r) );

		glDrawArrays( GL_TRIANGLES, 0, mLightsGeometrySize * 6 );

		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
	}

	glPopMatrix();
	glPopAttrib();
}
