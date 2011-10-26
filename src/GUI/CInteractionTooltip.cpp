#include "CInteractionTooltip.h"
#include "CButton.h"
#include "CRoot.h"
#include "../Utils/StringUtils.h"
#include "../Logic/CLogic.h"
#include "../Rendering/CCamera.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Utils/Maths.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <stdarg.h>

CInteractionTooltip::CInteractionTooltip() :
        mIsVisible(false),
        mTitle("") {
    fprintf(stderr, "CInteractionTooltip()\n");
}

CInteractionTooltip::~CInteractionTooltip() {
    fprintf(stderr, "~CInteractionTooltip()\n");
}

void CInteractionTooltip::SetTitle(const std::string & title) {
    mTitle = title;
    fprintf(stderr, "SetTitle %s\n", title.c_str());
}

CPlayer * CInteractionTooltip::GetPlayer()
{
    return mPlayer;
}

void CInteractionTooltip::SetPlayer(CPlayer * player)
{
    mPlayer = player;
}

void CInteractionTooltip::Show()
{
    mIsVisible = true;
}

void CInteractionTooltip::Hide()
{
    mIsVisible = false;
}

bool CInteractionTooltip::IsVisible()
{
    return mIsVisible;
}

void CInteractionTooltip::Update(float secondsPassed)
{
	if (gLogic.GetState() != L"playing")
		Hide();

  //  if (mIsVisible && mOptionHandler) {
 //       mOptionHandler->Update(secondsPassed);
//    }

    if (mIsVisible) {
//        UpdateButtons();
    }
}

