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
#include "../Utils/Maths.h"
#include "../Rendering/CCamera.h"
#include "../Rendering/CHudSprite.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Drawable.hpp>

#undef CreateWindow

using namespace GUI;

CCompass::CCompass() :
    mHud (NULL),
    mPos (sf::Vector2f(0.0f,0.0f)),
    mBackground (NULL),
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
    mArrows.clear(); //todo: remove any Arrows already contained?
};

void CCompass::Update(float dt)
{
    PointExit();
};

void CCompass::Show()
{
#ifndef __EDITOR__
    UpdatePosition();
    mHud->SetVisible(FindExit());
    for (size_t i = 0 ; i < mArrows.size() ; i++) {
        mArrows[i]->SetVisible(true);
    }
#endif
};

void CCompass::Hide()
{
    fprintf(stderr, "Hide()\n");
    mHud->SetVisible(false);
    ClearExit();
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
    for (size_t i = 0 ; i < mArrows.size() ; i++) {
        mArrows[i]->SetVisible(false);
    }
};

bool CCompass::FindExit()
{
    if (!(gMapManager.GetCurrent())) return false;
    return !(gMapManager.GetCurrent()->GetExitsPositions().empty());
};

inline float saturate(float v) { if(v>1)return 1;if(v<0)return 0;return v; }

void CCompass::PointExit()
{
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

    if (gMapManager.GetCurrent()) {
        std::vector<sf::Vector2f> exits = gMapManager.GetCurrent()->GetExitsPositions();
        size_t index = 0;
        for (std::vector<sf::Vector2f>::iterator it = exits.begin() ; it != exits.end() ; it++) {
            sf::Vector2f exitPos = *it;
            exitPos.x *= Map::TILE_SIZE;
            exitPos.y *= Map::TILE_SIZE;

            float rotation = RotationFromVector(sf::Vector2f (exitPos.x - mPos.x, exitPos.y - mPos.y));
            float distance = Maths::Length(exitPos - mPos) / Map::TILE_SIZE;
            float dotAlpha = distance > 32.0 ? 30 : 250 - (220 * (distance / 32.0));
            if (mArrows.size() <= index) {
                CImageBox * arrow = mHud->CreateImageBox("compass-arrow-" + StringUtils::ToString(index));
                arrow->AddImageToSequence("data/GUI/compass-green-dot.png");
                arrow->SetPosition(0.0f, 0.0f, 80.0f, 80.0f);
                mArrows.push_back(arrow);
            }
            sf::Sprite * sprite = mArrows[index]->mBackgroundSprite->GetSFSprite();
            sprite->SetRotation(rotation);
            sprite->SetColor(sf::Color(255, 255, 255, dotAlpha));
            index++;
        }
        for (size_t i = index ; i < mArrows.size() ; i++) {
            mArrows[i]->SetVisible(false);
        }
    }
};
