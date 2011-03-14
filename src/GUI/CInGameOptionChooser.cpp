#include "CInGameOptionChooser.h"
#include "CButton.h"
#include "CRoot.h"
#include "../Utils/StringUtils.h"
#include "../Logic/CLogic.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/CPlayer.h"
#include "../Rendering/CCamera.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Utils/MathsFunc.h"

#include <SFML/Graphics/RenderWindow.hpp>

CInGameOptionChooser::CInGameOptionChooser()
:	mIsVisible(false),
    mRadius(0.f)
{
    gGame.AddFrameListener(this);
}

CInGameOptionChooser::~CInGameOptionChooser()
{
    //todo: remove mButtons
}


void CInGameOptionChooser::FrameStarted(float secondsPassed)
{
    Update();
}


void CInGameOptionChooser::SetRadius(float radius)
{
    mRadius = radius;
}

void CInGameOptionChooser::SetOptionSize(const sf::Vector2f &size)
{
    mOptionSize = size;
}


void CInGameOptionChooser::SetOptionImages(const std::string & normal, const std::string & mouseOver)
{
    mOptionImageNormal = normal;
    mOptionImageMouseOver = mouseOver;
	for (size_t i = 0; i < mButtons.size(); i++) {
		mButtons[i]->SetImage(normal,mouseOver);
    }
}

void CInGameOptionChooser::SetOptionFont(const std::string & name, float size)
{
    mOptionFont = name;
    mOptionFontSize = size;
    for (size_t i = 0; i < mButtons.size(); i++) {
        mButtons[i]->SetFont(name, size, GUI::UNIT_PIXEL);
    }
}

void CInGameOptionChooser::SetOptionColor(const sf::Color color)
{
    mOptionColor = color;
    for (size_t i = 0; i < mButtons.size(); i++) {
        mButtons[i]->SetColor(color);
    }
}

void CInGameOptionChooser::SetOptions(const std::vector<std::wstring> & options)
{
    for (size_t i = 0 ; i < options.size() ; i++) {
        if (mButtons.size() <= i) {
            mButtons.push_back(gGUI.CreateButton(std::string("igoc-" + StringUtils::ToString(i), true, Z_GUI1)));
            mButtons[i]->SetPadding(40.f, 0.f, 0.f, 0.f);
            mButtons[i]->SetImage(mOptionImageNormal, mOptionImageMouseOver);
            mButtons[i]->SetFont(mOptionFont, mOptionFontSize, GUI::UNIT_PIXEL);
            mButtons[i]->SetColor(mOptionColor);
        }
        mButtons[i]->SetText(options[i]);
    }
    while (mButtons.size() > options.size()) {
        mButtons.back()->Remove();
        mButtons.pop_back();
    }
}

void CInGameOptionChooser::Show()
{
	SaveCursorPosition();
	UpdatePosition();
    mIsVisible = true;
}

void CInGameOptionChooser::Hide()
{
	if (mIsVisible)
		RestoreCursorPosition();
	mIsVisible = false;
}

void CInGameOptionChooser::Update()
{
	if (gLogic.GetState() != L"playing")
		Hide();

	for (size_t i = 0; i < mButtons.size(); i++)
		mButtons[i]->SetVisible(mIsVisible);

	if (mIsVisible) {
	    for (unsigned i = 0; i < mButtons.size(); i++) {

            CPlayer * player = gPlayerManager.GetPlayerByNumber(0);
            if (player) {
                sf::Vector2f center = gGUI.ConvertToGlobalPosition(0.01f * gCamera.TileToGui(player->GetPosition()));
		        center.x += sinf(float(i)/float(mButtons.size())*2*3.1415926f) * mRadius;
                center.y -= cosf(float(i)/float(mButtons.size())*2*3.1415926f) * mRadius;

		        mButtons[i]->SetPosition(-0.5f*mOptionSize.x + center.x, 
    			        -0.5f*mOptionSize.y + center.y, 
                        mOptionSize.x, 
		                mOptionSize.y,
                        GUI::UNIT_PIXEL
                );
            }
        }
		/* ograniczamy kursor do okregu przyciskow, zeby bylo latwiej rzucac czar 
		DamorK: bez wlasnego kursora i ukrycia systemowego trudno o stabilnosc wiec komentuje

		const sf::Input& in = gGame.GetRenderWindow()->GetInput();
		float radius = mRadius;
		if ( mKeys.size() > 0 )
			radius += Maths::Length( mKeys[0]->ConvertToGlobalPosition(sf::Vector2f(0.5f,0.5f)) - 
									 mKeys[0]->ConvertToGlobalPosition(sf::Vector2f(0.0f,0.0f)) );
		sf::Vector2f relativeCursorPos;
		relativeCursorPos.x = (float) in.GetMouseX() - mCenter.x;
		relativeCursorPos.y = (float) in.GetMouseY() - mCenter.y;
		if ( Maths::Length(relativeCursorPos) > radius )
		{
			relativeCursorPos = Maths::Normalize(relativeCursorPos) * radius * 0.95f;
			relativeCursorPos += mCenter;
			gGame.GetRenderWindow()->SetCursorPosition( (unsigned) relativeCursorPos.x, (unsigned) relativeCursorPos.y );
		}
		*/

		for (size_t i = 0; i < mButtons.size(); i++) {
    		if (mIsVisible && (gGUI.GetFocusedObject() == (GUI::CGUIObject*) mButtons[i]) )
            {
                fprintf(stderr, "InGameOptionChooser: %d\n", i);
            }
        }
	}
}

/* ================= PRIVATE ===================== */

void CInGameOptionChooser::UpdatePosition()
{
    /* umieszczamy srodek 'okienka' z przyciskami do czarowania w miejscu gdzie znajduje sie cursor,
     * tak zeby w calosci miescilo sie na ekranie */

    sf::Vector2f halfSize;

    if (mButtons.size() > 0) {
        halfSize = sf::Vector2f(mRadius, mRadius) +
	            mButtons[0]->ConvertToGlobalPosition(sf::Vector2f(0.5f, 0.5f)) - 
	            mButtons[0]->ConvertToGlobalPosition(sf::Vector2f(0.0f, 0.0f));
    }

	const sf::Input& in = gGame.GetRenderWindow()->GetInput();
	sf::Vector2f center;
	center.x = Maths::Clamp((float) in.GetMouseX(), halfSize.x, gGameOptions.GetWidth() - halfSize.x);
	center.y = Maths::Clamp((float) in.GetMouseY(), halfSize.y, gGameOptions.GetHeight() - halfSize.y);

	/* przemieszczamy kursor na srodek */

	
//	gGame.GetRenderWindow()->SetCursorPosition((unsigned) center.x, (unsigned) center.y);

	/* na podstawie pozycji srodka i promienia obliczamy pozycje poszczegolnych klawiszy */

    Update();
}

void CInGameOptionChooser::SaveCursorPosition()
{
	const sf::Input& in = gGame.GetRenderWindow()->GetInput();
	mCursorPos.x = in.GetMouseX();
	mCursorPos.y = in.GetMouseY();
}

void CInGameOptionChooser::RestoreCursorPosition()
{
	gGame.GetRenderWindow()->SetCursorPosition( mCursorPos.x, mCursorPos.y );
}

