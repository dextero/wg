#include "CInteractionTooltip.h"
#include "CRoot.h"
#include "CWindow.h"
#include "CTextArea.h"
#include "../Logic/CLogic.h"
#include "../Utils/StringUtils.h"
#include "../Logic/OptionChooser/InteractionHandler.h"

#include <stdarg.h>

static int interactionTooltipIdGenerator = 0;

using namespace GUI;

CInteractionTooltip::CInteractionTooltip() :
        mIsVisible(false),
        mHandler(NULL),
        mCanvas(NULL),
        mId(0) {
    fprintf(stderr, "CInteractionTooltip()\n");
    Clear();
}

CInteractionTooltip::~CInteractionTooltip() {
    fprintf(stderr, "~CInteractionTooltip()\n");
    mCanvas->Remove();
    mCanvas = NULL;
}

void CInteractionTooltip::Show() {
    if (mIsVisible == true) return;
    mIsVisible = true;
    mCanvas->SetVisible(true);
}

void CInteractionTooltip::Hide() {
    mIsVisible = false;
    mCanvas->SetVisible(false);
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
    mIsVisible = false;
    mId = ++interactionTooltipIdGenerator;
    if (mHandler != NULL) {
        delete (mHandler);
        mHandler = NULL;
    }
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

int CInteractionTooltip::GetId() {
    return mId;
}

