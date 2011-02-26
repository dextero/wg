#include "CMouseCaster.h"
#include "../GUI/CButton.h"
#include "../GUI/CRoot.h"
#include "../Utils/StringUtils.h"
#include "../Logic/CLogic.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Utils/MathsFunc.h"

#include <SFML/Graphics/RenderWindow.hpp>

CMouseCaster::CMouseCaster()
:	mState( MC_IDLE ),
    mRadius(0.f),
	mLastKey( -1 ),
	mCurrentKey( -1 )
{
}

CMouseCaster::~CMouseCaster()
{
}

void CMouseCaster::Initialize(std::vector<std::wstring> &abiKeys, float radius)
{
	for ( unsigned i = 0; i < abiKeys.size(); i++ )
	{
		GUI::CButton* btn = gGUI.CreateButton( std::string("mc-") + StringUtils::ToString(i), true, Z_GUI1 );
		btn->SetText( abiKeys[i] );
        btn->SetPadding(40.f, 0.f, 0.f, 0.f);
		mKeys.push_back( btn );
		mRadius = radius;
	}
}

void CMouseCaster::SetKeySize(const sf::Vector2f &size, GUI::guiUnit u)
{
	mKeySize[u] = size;
}

void CMouseCaster::SetKeyImage(const std::string &normal, const std::string &mouseOver)
{
	for ( unsigned i = 0; i < mKeys.size(); i++ )
		mKeys[i]->SetImage( normal, mouseOver );
}

void CMouseCaster::SetKeyFont(const std::string &name, float size, GUI::guiUnit u)
{
	for ( unsigned i = 0; i < mKeys.size(); i++ )
		mKeys[i]->SetFont( name, size, u );
}

void CMouseCaster::SetKeyColor(const sf::Color color)
{
	for ( unsigned i = 0; i < mKeys.size(); i++ )
		mKeys[i]->SetColor( color );
}

void CMouseCaster::Cast()
{
	SaveCursorPosition();
	UpdatePosition();
	mState = MC_CAST;
}

void CMouseCaster::Focus()
{
	if ( IsVisible() )
		RestoreCursorPosition();
	mLastKey = mCurrentKey;
	mState = MC_FOCUS;
}

void CMouseCaster::Stop()
{
	if ( IsVisible() )
		RestoreCursorPosition();
	mLastKey = mCurrentKey = -1;
	mState = MC_IDLE;
}

void CMouseCaster::Update()
{
	if ( gLogic.GetState() != L"playing" )
		Stop();

	for ( unsigned i = 0; i < mKeys.size(); i++ )
		mKeys[i]->SetVisible( IsVisible() );

	if ( mState == MC_CAST )
	{
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

		mLastKey = mCurrentKey;
		mCurrentKey = -1;

		for ( unsigned i = 0; i < mKeys.size(); i++ )
		if ( IsVisible() && (gGUI.GetFocusedObject() == (GUI::CGUIObject*) mKeys[i]) )
			mCurrentKey = i;
	}
}

/* ================= PRIVATE ===================== */

void CMouseCaster::UpdatePosition()
{
	/* umieszczamy srodek 'okienka' z przyciskami do czarowania w miejscu gdzie znajduje sie cursor,
	 * tak zeby w calosci miescilo sie na ekranie */

	sf::Vector2f halfSize;

	if ( mKeys.size() > 0 )
		halfSize =	sf::Vector2f( mRadius, mRadius ) +
					mKeys[0]->ConvertToGlobalPosition( sf::Vector2f(0.5f, 0.5f) ) - 
					mKeys[0]->ConvertToGlobalPosition( sf::Vector2f(0.0f, 0.0f) );

	const sf::Input& in = gGame.GetRenderWindow()->GetInput();
	sf::Vector2f center;
	center.x = Maths::Clamp( (float) in.GetMouseX(), halfSize.x, gGameOptions.GetWidth() - halfSize.x );
	center.y = Maths::Clamp( (float) in.GetMouseY(), halfSize.y, gGameOptions.GetHeight() - halfSize.y );

	/* przemieszczamy kursor na srodek */
	
	gGame.GetRenderWindow()->SetCursorPosition( (unsigned) center.x, (unsigned) center.y );

	/* na podstawie pozycji srodka i promienia obliczamy pozycje poszczegolnych klawiszy */

	for ( unsigned i = 0; i < mKeys.size(); i++ )
	{
		sf::Vector2f keyCenter = center;
		keyCenter.x += sinf( float(i)/float(mKeys.size())*2*3.1415926f ) * mRadius;
		keyCenter.y -= cosf( float(i)/float(mKeys.size())*2*3.1415926f ) * mRadius;

		mKeys[i]->SetPosition(	-0.5f*mKeySize[GUI::UNIT_PERCENT].x, 
								-0.5f*mKeySize[GUI::UNIT_PERCENT].y, 
								mKeySize[GUI::UNIT_PERCENT].x, 
								mKeySize[GUI::UNIT_PERCENT].y );

		mKeys[i]->SetPosition(	-0.5f*mKeySize[GUI::UNIT_PIXEL].x + keyCenter.x, 
								-0.5f*mKeySize[GUI::UNIT_PIXEL].y + keyCenter.y, 
								mKeySize[GUI::UNIT_PIXEL].x, 
								mKeySize[GUI::UNIT_PIXEL].y,
								GUI::UNIT_PIXEL);
	}
}

void CMouseCaster::SaveCursorPosition()
{
	const sf::Input& in = gGame.GetRenderWindow()->GetInput();
	mCursorPos.x = in.GetMouseX();
	mCursorPos.y = in.GetMouseY();
}

void CMouseCaster::RestoreCursorPosition()
{
	gGame.GetRenderWindow()->SetCursorPosition( mCursorPos.x, mCursorPos.y );
}

