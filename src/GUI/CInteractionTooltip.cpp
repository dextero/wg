#include "CInteractionTooltip.h"
#include "CRoot.h"
#include "CWindow.h"
#include "CTextArea.h"
#include "../Logic/CLogic.h"
#include "../Utils/StringUtils.h"

#include <stdarg.h>

using namespace GUI;

CInteractionTooltip::CInteractionTooltip() :
        mIsVisible(false),
        mHandler(NULL),
        mCanvas(NULL) {
    fprintf(stderr, "CInteractionTooltip()\n");
    Clear();
}

CInteractionTooltip::~CInteractionTooltip() {
    fprintf(stderr, "~CInteractionTooltip()\n");
    mCanvas->Remove();
    mCanvas = NULL;
}

//CPlayer * CInteractionTooltip::GetPlayer() {
//    return mPlayer;
//}

//void CInteractionTooltip::SetPlayer(CPlayer * player) {
//    mPlayer = player;
//}

void CInteractionTooltip::Show() {
    if (mIsVisible == true) return;
    mIsVisible = true;
    fprintf(stderr, "showing\n");
    mCanvas->SetVisible(true);
}

void CInteractionTooltip::Hide() {
    mIsVisible = false;
}

void CInteractionTooltip::Clear() {
    if (mCanvas != NULL) {
        mCanvas->Remove();
    }
    mCanvas = gGUI.CreateWindow("interaction_tooltip" /* todo: player number */, true, Z_GUI4);
    mCanvas->SetBackgroundImage("data/GUI/transparent-black.png");
    mCanvas->SetVisible(false);
    mCanvas->SetPosition(5.f, 5.f, 90.f, 90.f);
    gGUI.UnregisterObject(mCanvas); // don't catch the mouse events
}

bool CInteractionTooltip::IsVisible() {
    return mIsVisible;
}

CWindow * CInteractionTooltip::GetCanvas() {
    return mCanvas;
}

void CInteractionTooltip::SetHandler(InteractionHandler * handler) {
    mHandler = handler;
}

InteractionHandler * CInteractionTooltip::GetHandler() {
    return mHandler;
}

//void CInteractionTooltip::Update(float secondsPassed)
//{
//	if (gLogic.GetState() != L"playing")
//		Hide();

  //  if (mIsVisible && mOptionHandler) {
 //       mOptionHandler->Update(secondsPassed);
//    }

//    if (mIsVisible) {
//        UpdateButtons();
//    }
//}

