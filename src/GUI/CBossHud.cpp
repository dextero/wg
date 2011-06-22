
#include "CBossHud.h"
#include "CGUIObject.h"
#include "CWindow.h"
#include "CProgressBar.h"
#include "CImageBox.h"
#include "CRoot.h"
#include "CTextArea.h"
#include "../CGameOptions.h"
#include "../Logic/CEnemy.h"
#include "../Logic/Boss/CBossManager.h"
#include "Localization/CLocalizator.h"

#include <SFML/Graphics.hpp>

#undef CreateWindow

using namespace GUI;

CBossHud::CBossHud() :
    mHP             ( NULL ),
    mAvatar         ( NULL ),
    mHud            ( NULL ),
    mPreviousBoss   ( NULL)
{}

CBossHud::~CBossHud()
{}


void GUI::CBossHud::Init()
{
    std::string prefix("Boss_");
    mHud = gGUI.CreateWindow( prefix + "hud" );
    UpdatePosition();

    CImageBox* bg = mHud->CreateImageBox(prefix + "bg");
    bg->AddImageToSequence("data/GUI/boss-hud/bg.png");
    bg->SetPosition(0.f, 0.f, 100.f, 100.f);

    mHP = bg->CreateProgressBar(prefix + "bar");
    mHP->SetProgressImage("data/GUI/boss-hud/bar.png");
    mHP->SetMinClippingPlane(270.f, sf::Vector2f(0.f, 0.f));
    mHP->SetMaxClippingPlane(270.f, sf::Vector2f(100.f, 0.f));
    mHP->SetPosition(0.5f, 10.f, 99.f, 77.f);

    mText = bg->CreateTextArea(prefix + "name", true, Z_GUI2);
    mText->SetPosition(0.f, 0.f, 100.f, 100.f);
    mText->SetCenter(true);
    mText->SetColor(sf::Color::White);
    mText->SetFont(gLocalizator.GetFont(FONT_DIALOG), 14.f);
    mText->GetSFString()->SetStyle(sf::String::Bold);
}

void GUI::CBossHud::Update( float dt )
{
    CEnemy * boss = gBossManager.GetBoss();
    if (boss != mPreviousBoss)
        HandleNewBoss(boss);

    if (boss != NULL)
    {
        mText->SetText(gLocalizator.GetText(("PHYSICAL_" + StringUtils::ToUpper(StringUtils::ConvertToString(boss->GetUniqueId()))).c_str()));
        mHP->SetProgress( boss->GetHP() / boss->GetStats()->GetCurrAspect(aMaxHP) );
    }
}

void GUI::CBossHud::Show()
{
    UpdatePosition();
    mHud->SetVisible( gBossManager.GetBoss() != NULL );
}

void GUI::CBossHud::Hide()
{
	mHud->SetVisible( false );
}

void GUI::CBossHud::UpdatePosition()
{
    mHud->SetPosition( 10.0f, (float) gGameOptions.GetHeight() - 35.0f, (float) gGameOptions.GetWidth() - 20.0f, 25.0f, UNIT_PIXEL, false );
}

void GUI::CBossHud::HandleNewBoss( CEnemy * boss )
{
    mPreviousBoss = boss;

    if (boss != NULL)
    {
        Show();
        // inicjuj avatara;
    }
    else
        Hide();
}
