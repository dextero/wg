#include "CCompass.h"
#include "CRoot.h"
#include "CImageBox.h"
#include "../CGameOptions.h"
#include "CGUIObject.h"
#include "CWindow.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/CPlayer.h"
#include "../Logic/CEnemy.h"
#include "../Logic/Boss/CBossManager.h"
#include "../Map/CMapManager.h"
#include "../Utils/Directions.h"
#include "../Rendering/CCamera.h"
#include "../Rendering/CHudSprite.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Drawable.hpp>

#undef CreateWindow

using namespace GUI;

CCompass::CCompass() :
    mHud (NULL),
    mPos (sf::Vector2f(0.0f,0.0f)),
    mExitPos (sf::Vector2f(0.0f,0.0f)),
    mArrow (NULL),
    mBackground (NULL),
    mExitFound (false),
    mRotation (0.0f)
{};

CCompass::~CCompass(){};

void CCompass::Init()
{
    mHud = gGUI.CreateWindow("compass");
    UpdatePosition();
    mBackground = mHud->CreateImageBox("compass-bg");
    mBackground->AddImageToSequence("data/GUI/compass-bg.png");
    mBackground->SetPosition(0.0f, 0.0f, 80.0f, 80.0f);
    mArrow = mHud->CreateImageBox("compass-arrow");
    mArrow->AddImageToSequence("data/GUI/compass-arrow.png");
    mArrow->SetPosition(0.0f, 0.0f, 80.0f, 80.0f);
};

void CCompass::Update(float dt)
{
    PointExit();
};

void CCompass::Show()
{
#ifndef __EDITOR__
    UpdatePosition();
    mHud->SetVisible(mExitFound || FindExit());
#endif
};

void CCompass::Hide()
{
    mHud->SetVisible(false);
};

void CCompass::UpdatePosition()
{
  /*  mHud->SetPosition((float) gGameOptions.GetWidth() / 2 - 40.0f,
                      (float) gGameOptions.GetHeight() - 90.0f,
                      80.0f, 80.0f,
                      UNIT_PIXEL);*/
};

void CCompass::ClearExit()
{
    mExitPos = sf::Vector2f (0.0f, 0.0f);
    mExitFound = false;
};

bool CCompass::FindExit()
{
    if (!(gMapManager.GetCurrent())) return false;
    mExitPos = gMapManager.GetCurrent()->GetExitPos();
    mExitPos.x *= Map::TILE_SIZE;
    mExitPos.y *= Map::TILE_SIZE;
    mExitFound = mExitPos != sf::Vector2f (0.0f, 0.0f);
    return mExitFound;
};

inline float saturate(float v) { if(v>1)return 1;if(v<0)return 0;return v; }

void CCompass::PointExit()
{
    sf::Vector2f exitPos;

    if (gBossManager.GetBoss())
        exitPos = gBossManager.GetBoss()->GetPosition() * (float)Map::TILE_SIZE;
    else
    {
        if (!mExitFound)
            FindExit();
        exitPos = mExitPos;
    }

//	pobieranie pozycji kompasu - stare
	sf::Vector2f mPos1 = gCamera.GetPosition();
	mPos1.y += ((float) gGameOptions.GetHeight()) / 2.0f - 40.0f;

	mPos1.x /= Map::TILE_SIZE;
	mPos1.y /= Map::TILE_SIZE;

//  pobieranie pozycji gracza pierwszego - nowe
    CPlayer * player = gPlayerManager.GetPlayerByNumber(0);
    if (player) {
        mPos = gPlayerManager.GetPlayerByNumber(0)->GetPosition();
    }

	float dx = mPos.x - mPos1.x;
	float dy = mPos.y - mPos1.y;
	float l = dx*dx + dy*dy;

	float mod = saturate(1.0f - (l - 0.5f) * 2.0f);
	mHud->SetPosition((float) gGameOptions.GetWidth() / 2 - 40.0f,
		(float) gGameOptions.GetHeight() - 90.0f + 90.0f * mod,
		80.0f, 80.0f,
		UNIT_PIXEL);

	mPos.x *= Map::TILE_SIZE;
	mPos.y *= Map::TILE_SIZE;

	//warning C4244: '=' : conversion from 'int' to 'float', possible loss of data
    mRotation = (float)RotationFromVector (sf::Vector2f (exitPos.x - mPos.x, exitPos.y - mPos.y));
    mArrow->mBackgroundSprite->GetSFSprite()->SetRotation(mRotation);
};