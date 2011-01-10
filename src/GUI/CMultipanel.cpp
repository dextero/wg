#include "CMultipanel.h"
#include "CWindow.h"
#include "CRoot.h"
#include "CButton.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../Utils/StringUtils.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <cassert>
#include <cstdio>

using namespace GUI;

CMultipanel::CMultipanel( const std::string& name, CGUIObject* parent, unsigned int zindex, unsigned int panelCount )
: CGUIObject(name, GUI_MULTIPANEL, parent, zindex),mRatio(10.0f),mActiveWnd(0){
    assert(panelCount > 0);
    for (size_t i = 0; i < panelCount; i++){
        mWindows.push_back(CreateWindow(name + "_" + StringUtils::ToString(i)));
    }
    for (size_t i = 0; i < panelCount; i++){
        mSwitchers.push_back(gDrawableManager.CreateHudSprite());
    }
}

CMultipanel::~CMultipanel(){
    for (size_t i = 0; i < mSwitchers.size(); i++)
        if (mSwitchers[i])
            gDrawableManager.DestroyDrawable( mSwitchers[i] );
}

CHudSprite *CMultipanel::GetSwitcherImage( unsigned int i ){
    if (mSwitchers.size() <= mActiveWnd)
        return NULL;
    else
        return mSwitchers[mActiveWnd];
}

void CMultipanel::SetSwitcherImage( unsigned int i, const std::string &filename ){
    if (mSwitchers.size() > i){
        if (!mSwitchers[i])
            mSwitchers[i] = gDrawableManager.CreateHudSprite( mZIndex );
        mSwitchers[i]->GetSFSprite()->SetImage(*gResourceManager.GetImage(filename));
    }
}

CWindow *CMultipanel::GetActiveWindow(){
    return GetWindow(mActiveWnd);
}

CWindow *CMultipanel::GetWindow(unsigned int i){
    if (mWindows.size() <= mActiveWnd)
        return NULL;
    else
        return mWindows[i];
}

unsigned int CMultipanel::GetActiveWindowIndex(){
    return mActiveWnd;
}

void CMultipanel::SetSwitchersToPanelRatio(float ratio){
    mRatio = ratio;
}

bool CMultipanel::OnMouseEvent(float x, float y, mouseEvent e){
    CGUIObject::OnMouseEvent(x, y, e);

    if (e == MOUSE_PRESSED_LEFT){
        x -= mGlobalPosition.x;
        y -= mGlobalPosition.y;
        if (y < mGlobalSize.y * mRatio / 100.0f){
            float w = mSwitchers.size() * (x / mGlobalSize.x);
            mActiveWnd = (int)w;
            fprintf(stderr,"onMouseEvent: got position %f,%f; w is %f, switching to %d\n",x,y,w,mActiveWnd);
        }
    }

	return false;
}

void CMultipanel::UpdateSprites( float secondsPassed ){
    for (size_t i = 0; i < mWindows.size(); i++){
        mWindows[i]->SetVisible((i == mActiveWnd) && mIsVisible);
        mWindows[i]->SetPosition(0,mRatio,100.0f,100.0f - mRatio);
    }
    for (size_t i = 0; i < mSwitchers.size(); i++){
        if ((mSwitchers[i]) && (mSwitchers[i]->GetSFSprite()) && (mSwitchers[i]->GetSFSprite()->GetImage())){
		    sf::Vector2i imgSize(mSwitchers[i]->GetSFSprite()->GetImage()->GetWidth(),mSwitchers[i]->GetSFSprite()->GetImage()->GetHeight());
            mSwitchers[i]->SetVisible((i == mActiveWnd) && mIsVisible);
            mSwitchers[i]->GetSFSprite()->SetPosition(mGlobalPosition);
            mSwitchers[i]->GetSFSprite()->SetScale( mGlobalSize.x / imgSize.x, (mGlobalSize.y * mRatio) / (imgSize.y * 100.0f));
        }
    }
}
