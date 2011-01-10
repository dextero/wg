#include "CAnimatedImage.h"
#include "../Map/CMap.h"
#include "../Rendering/CCamera.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/Animations/SAnimationState.h"

using namespace GUI;

CAnimatedImage::CAnimatedImage(const std::string& name, CGUIObject* parent, unsigned zindex):
    CGUIObject(name, GUI_ANIMATED_IMAGE, parent, zindex),
    mDisplayable(gDrawableManager.CreateDisplayable(zindex))
{
}

CAnimatedImage::~CAnimatedImage()
{
    gDrawableManager.DestroyDrawable(mDisplayable);
}


void CAnimatedImage::SetVisible(bool visible, bool recursive)
{
    CGUIObject::SetVisible(visible, recursive);
    mDisplayable->SetVisible(visible);
}

void CAnimatedImage::SetAnimation(const std::string& name)
{
    mDisplayable->SetAnimation(name);
}

void CAnimatedImage::SetAnimation(SAnimation* anim)
{
    mDisplayable->SetAnimation(anim);
}

void CAnimatedImage::UpdateSprites(float secondsPassed)
{
    if (!mIsVisible)
        return;

    // przesun i przeskaluj animacje, rozmiary w kaflach
	float invZoom = 1.0f / gCamera.GetZoom();
    sf::Vector2f size = mGlobalSize / (float)Map::TILE_SIZE * invZoom;
    sf::Vector2f pos = (gCamera.GetViewTopLeft() + ConvertToGlobalPosition(sf::Vector2f(0.f, 0.f)) * invZoom) / (float)Map::TILE_SIZE;
    // pozycja displayabli = ich srodek, wiec..
    pos += size / 2.f;

    mDisplayable->SetScale(invZoom);
    mDisplayable->SetPosition(pos);

    // clipRect, zeby w scrollPanelu jakos wygladalo
    mDisplayable->SetClipRect(mClippingRect);

    if (mDisplayable->GetAnimationState())
    {
        // przeskaluj obrazki, zeby mialy oryginalny rozmiar - jesli jest za duzy, wez 1.f
        mDisplayable->SetScale(invZoom * std::min(
            std::min(
                mDisplayable->GetAnimationState()->GetCurrentFrame().rect.GetWidth() / mGlobalSize.x,
                mDisplayable->GetAnimationState()->GetCurrentFrame().rect.GetHeight() / mGlobalSize.y),
            1.f));
        mDisplayable->GetAnimationState()->AddTime(secondsPassed);
    }
}