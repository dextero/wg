#include "CInGameOptionChooser.h"
#include "CButton.h"
#include "CRoot.h"
#include "../Utils/StringUtils.h"
#include "../Logic/CLogic.h"
#include "../Logic/CActor.h"
#include "../Logic/OptionChooser/IOptionChooserHandler.h"
#include "../Rendering/CCamera.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Utils/Maths.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <stdarg.h>

InGameOptionChooserVector CInGameOptionChooser::msActiveChoosers;

CInGameOptionChooser * CInGameOptionChooser::CreateChooser() {
    CInGameOptionChooser * chooser = new CInGameOptionChooser();
    msActiveChoosers.push_back(chooser);
    return chooser;
}

CInGameOptionChooser::CInGameOptionChooser() :
        mIsVisible(false),
        mTitle(NULL),
        mRadius(0.f),
        mOptionHandler(NULL) {
    fprintf(stderr, "CInGameOptionChooser()\n");
}

CInGameOptionChooser::~CInGameOptionChooser() {
    fprintf(stderr, "~CInGameOptionChooser()\n");
    while (mButtons.size() > 0) {
        mButtons.back()->Remove();
        mButtons.pop_back();
    }
    if (mOptionHandler) {
        mOptionHandler->SetChooser(NULL);
        mOptionHandler->mReferenceCounter--;
        if (mOptionHandler->mReferenceCounter == 0) {
            delete mOptionHandler;
        }
    }

    for (InGameOptionChooserVector::iterator it = msActiveChoosers.begin() ; it != msActiveChoosers.end() ; it++) {
        if (*it == this) {
            *it = msActiveChoosers.back();
            msActiveChoosers.pop_back();
            break;
        }
    }
}

void CInGameOptionChooser::UpdateAll(float secondsPassed) {
    for (InGameOptionChooserVector::iterator it = msActiveChoosers.begin() ; it != msActiveChoosers.end() ; it++) {
        (*it)->Update(secondsPassed);
    }
}

void CInGameOptionChooser::SetRadius(float radius) {
    mRadius = radius;
}

void CInGameOptionChooser::SetOptionSize(const sf::Vector2f &size) {
    mOptionSize = size;
}

void CInGameOptionChooser::SetOptionImages(const std::string & normal, const std::string & mouseOver) {
    mOptionImageNormal = normal;
    mOptionImageMouseOver = mouseOver;
	for (size_t i = 0; i < mButtons.size(); i++) {
		mButtons[i]->SetImage(normal, mouseOver);
    }
    if (mTitle) {
        mTitle->SetImage(normal, mouseOver);
    }
}

void CInGameOptionChooser::SetOptionFont(const GUI::FontSetting & fs) {
    mOptionFontSetting = fs;
    for (size_t i = 0; i < mButtons.size(); i++) {
        mButtons[i]->SetFont(fs);
    }
    if (mTitle) {
//        mTitle->SetFont(name, size + 2.0f, GUI::UNIT_PIXEL);
        mTitle->SetFont(fs);
    }
}

void CInGameOptionChooser::SetOptionColor(const sf::Color color) {
    mOptionColor = color;
    for (size_t i = 0; i < mButtons.size(); i++) {
        mButtons[i]->SetColor(color);
    }
    if (mTitle) {
        mTitle->SetColor(color);
    }
}

void CInGameOptionChooser::SetTitle(const std::string & title) {
    if (mTitle == NULL) {
        mTitle = gGUI.CreateButton(std::string("igoc-title", true, Z_GUI1));
        mTitle->SetPadding(40.f, 0.f, 0.f, 0.f);
        // te tutaj to trzeba chyba jednak na sztywno ustalic czy w inny sposob uzyskiwac
        // - a na razie tak na lapu-capu zostawiam:
        mTitle->SetImage(mOptionImageNormal, mOptionImageMouseOver);
//        mTitle->SetFont(mOptionFont, mOptionFontSize + 2.0f, GUI::UNIT_PIXEL);
        mTitle->SetFont(mOptionFontSetting);
        mTitle->SetColor(mOptionColor);
        mTitle->SetCenter(true);
    }
    mTitle->SetText(StringUtils::ConvertToWString(title));
}

void CInGameOptionChooser::SetOptions(const std::vector<std::string> & options)
{
    for (size_t i = 0 ; i < options.size() ; i++) {
        if (mButtons.size() <= i) {
            mButtons.push_back(gGUI.CreateButton(std::string("igoc-" + StringUtils::ToString(i), true, Z_GUI1)));
            mButtons[i]->SetPadding(40.f, 0.f, 0.f, 0.f);
            mButtons[i]->SetImage(mOptionImageNormal, mOptionImageMouseOver);
//            mButtons[i]->SetFont(mOptionFont, mOptionFontSize, GUI::UNIT_PIXEL);
            mButtons[i]->SetFont(mOptionFontSetting);
            mButtons[i]->SetColor(mOptionColor);
            mButtons[i]->GetClickIntCallback()->bind(this, &CInGameOptionChooser::OptionSelectedThroughMouse);
            mButtons[i]->SetClickIntCallbackParam((int)i);
        }
        mButtons[i]->SetText(StringUtils::ConvertToWString(options[i]));
    }
    while (mButtons.size() > options.size()) {
        mButtons.back()->Remove();
        mButtons.pop_back();
    }
}

void CInGameOptionChooser::SetOptions(const char * first, ...) {
    std::vector<std::string> options;
    const char * str;
    va_list vl;

    str = first;

    va_start(vl, first);

    do {
        options.push_back(str);
        str = va_arg(vl, const char*);
    } while (str != NULL);

    va_end(vl);
    SetOptions(options);
}

void CInGameOptionChooser::SetOption(const char * first) {
    SetOptions(first, NULL);
}
void CInGameOptionChooser::SetOptions(const char * first, const char * second) {
    SetOptions(first, second, NULL);
}
void CInGameOptionChooser::SetOptions(const char * first, const char * second, const char * third) {
    SetOptions(first, second, third, NULL);
}
void CInGameOptionChooser::SetOptions(const char * first, const char * second, const char * third, const char * fourth) {
    SetOptions(first, second, third, fourth, NULL);
}

void CInGameOptionChooser::SetOptionHandler(IOptionChooserHandler * handler)
{
    if (mOptionHandler != NULL) {
        mOptionHandler->SetChooser(NULL);
        mOptionHandler->mReferenceCounter--;
        if (mOptionHandler->mReferenceCounter == 0) {
            delete mOptionHandler;
        }
    }
    mOptionHandler = handler;
    mOptionHandler->mReferenceCounter++;
    mOptionHandler->SetChooser(this);
}

CPlayer * CInGameOptionChooser::GetPlayer()
{
    return mPlayer;
}

void CInGameOptionChooser::SetPlayer(CPlayer * player)
{
    mPlayer = player;
}

void CInGameOptionChooser::Show()
{
	SaveCursorPosition();
	UpdatePosition();
    mIsVisible = true;
    UpdateButtons();
}

void CInGameOptionChooser::Hide()
{
    if (mIsVisible)
        RestoreCursorPosition();
    mIsVisible = false;
    UpdateButtons();
}

bool CInGameOptionChooser::IsVisible()
{
    return mIsVisible;
}

void CInGameOptionChooser::OptionSelectedThroughMouse(int selected)
{
    this->OptionSelected((size_t)selected);
}

void CInGameOptionChooser::OptionSelected(size_t selected)
{
    if (selected >= mButtons.size()) // cos tu nie gra 
        return;
    if (mOptionHandler) {
        mOptionHandler->OptionSelected(selected);
        mOptionHandler->SetChooser(NULL);
        mOptionHandler->mReferenceCounter--;
        if (mOptionHandler->mReferenceCounter == 0) {
            delete mOptionHandler;
        }
        mOptionHandler = NULL;
    }
    Hide();
}

void CInGameOptionChooser::UpdateButtons()
{
    for (size_t i = 0; i < mButtons.size(); i++) {
		mButtons[i]->SetVisible(mIsVisible);
    }
    if (mTitle) {
        mTitle->SetVisible(mIsVisible);
    }
	if (mIsVisible && mPlayer) { // w sumie czy moze byc !mActor?
        sf::Vector2f center = gGUI.ConvertToGlobalPosition(0.01f * gCamera.TileToGui(mPlayer->GetPosition()));
        if (mTitle) {
            mTitle->SetPosition(
                    -4.0f * mOptionSize.x + center.x,
                    -0.5f * mOptionSize.y + center.y - (mRadius * 1.6f),
                    mOptionSize.x * 8,
                    mOptionSize.y,
                    GUI::UNIT_PIXEL
            );
        }

	    for (unsigned i = 0; i < mButtons.size(); i++) {
            sf::Vector2f pos = center;
            pos.x = center.x + sinf(float(i)/float(mButtons.size())*2*3.1415926f) * mRadius;
            pos.y = center.y - cosf(float(i)/float(mButtons.size())*2*3.1415926f) * mRadius;

            mButtons[i]->SetPosition(-0.5f * mOptionSize.x + pos.x, 
                    -0.5f * mOptionSize.y + pos.y, 
                    mOptionSize.x, 
                    mOptionSize.y,
                    GUI::UNIT_PIXEL
            );
        }
//		for (size_t i = 0; i < mButtons.size(); i++) {
//    		if (mIsVisible && (gGUI.GetFocusedObject() == (GUI::CGUIObject*) mButtons[i]) )
//            {
//                // fprintf(stderr, "InGameOptionChooser: %d\n", i);
//            }
//        }
	}
}

void CInGameOptionChooser::Update(float secondsPassed)
{
	if (gLogic.GetState() != L"playing")
		Hide();

    if (mIsVisible && mOptionHandler) {
        mOptionHandler->Update(secondsPassed);
    }

    if (mIsVisible) {
        UpdateButtons();
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

}

void CInGameOptionChooser::SaveCursorPosition()
{
//	const sf::Input& in = gGame.GetRenderWindow()->GetInput();
//	mCursorPos.x = in.GetMouseX();
//	mCursorPos.y = in.GetMouseY();
}

void CInGameOptionChooser::RestoreCursorPosition()
{
//	gGame.GetRenderWindow()->SetCursorPosition( mCursorPos.x, mCursorPos.y );
}

