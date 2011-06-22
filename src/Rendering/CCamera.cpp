#include <cstdio>

#include "CCamera.h"

#include "../Logic/CPhysical.h"
#include "../Logic/CPlayer.h"
#include "../Logic/CPlayerManager.h"
#include "../Map/CMap.h"
#include "../Map/CMapManager.h"

#include "../Commands/CameraCommands.h"
#include "../Commands/CCommands.h"

#include "../Utils/Maths.h"
#include "../Utils/StringUtils.h"
#include "../Utils/CXml.h"
#include "../CGame.h"
#include "../CGameOptions.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#if defined(PLATFORM_LINUX) && defined(__EDITOR__)
#   undef _T
#   include "../Utils/muParser/muParser.h"
#   define _T(x) L ## x
#else
#   include "../Utils/muParser/muParser.h"
#endif

template<> CCamera* CSingleton<CCamera>::msSingleton = 0;

CCamera::CCamera(): 
	mTrail(NULL), 
	mTask( CCamera::STAND ), 
    mSwayingTime(0.f),
	mZoom(1.0f),
	mParser(NULL),
	mViewArea(0.0f,0.0f,1.0f,1.0f)
{
    fprintf(stderr,"CCamera::CCamera()\n");
    
    gCommands.RegisterCommands(CameraCommands);
    gGame.AddFrameListener( this );
	LoadSettings();
}

CCamera::~CCamera()
{
    fprintf(stderr,"CCamera::~CCamera()\n");
}

void CCamera::GoTo( const sf::Vector2f& v, float swayingTime )
{
	GoTo( v.x, v.y, swayingTime );
}

void CCamera::GoTo(float x, float y, float swayingTime)
{
	mTask = CCamera::GO_TO_POINT;
	mGoTo.x = x;
	mGoTo.y = y;
	mSwayingTime = swayingTime;
}

void CCamera::TrailPhysical(CPhysical *newTrail, float swayingTime)
{
	if ( newTrail )
	{
		mTask = CCamera::TRAIL_PHYSICAL;
		mTrail = newTrail;
		mSwayingTime = swayingTime;
	}
}

void CCamera::TrailPlayers(float swayingTime)
{
	mTask = CCamera::TRAIL_PLAYERS;
	mSwayingTime = swayingTime;
	Update(0.0f);
}

void CCamera::StopCamera()
{
	mTask = CCamera::STAND;
	mSwayingTime = 0.0f;
	mTrail = NULL;
}

void CCamera::SetViewArea(float left, float top, float right, float bottom)
{
	mViewArea.Left = left * 0.01f;
	mViewArea.Top = top * 0.01f;
	mViewArea.Right = right * 0.01f;
	mViewArea.Bottom = bottom * 0.01f;
}

void CCamera::SetZoom(float zoom)
{
	gGame.GetRenderWindow()->GetDefaultView().SetHalfSize( gGameOptions.GetWidth() / 2.0f / zoom, gGameOptions.GetHeight() / 2.0f / zoom );
	mZoom = zoom;
}

sf::Vector2f CCamera::GetPosition()
{
    return mPosition;
}

sf::Vector2f CCamera::GetPositionInGame()
{
	return GetPosition() / (float) Map::TILE_SIZE;
}

sf::Vector2f CCamera::GetViewTopLeft()
{
    sf::Vector2f v;
    sf::FloatRect rect = gGame.GetRenderWindow()->GetDefaultView().GetRect();
    v.x = rect.Left;
    v.y = rect.Top;
    return v;
}

sf::Vector2f CCamera::TileToGui(sf::Vector2f tile)
{
    tile *= static_cast<float>(Map::TILE_SIZE);
    tile -= GetViewTopLeft();

    const sf::FloatRect &window = gGame.GetRenderWindow()->GetDefaultView().GetRect();

    tile.x /= window.GetWidth();
    tile.y /= window.GetHeight();

    tile.x = Maths::Clamp(tile.x * 100.f , 0.f, 100.f);
    tile.y = Maths::Clamp(tile.y * 100.f , 0.f, 100.f);

    return tile;
}

sf::FloatRect CCamera::GetViewRectInGame()
{
	sf::FloatRect r = gGame.GetRenderWindow()->GetDefaultView().GetRect();
	r.Left /= Map::TILE_SIZE;
	r.Top /= Map::TILE_SIZE;
	r.Right /= Map::TILE_SIZE;
	r.Bottom /= Map::TILE_SIZE;
	return r;
}

CCamera::cameraTask CCamera::GetCameraTask()
{
	return mTask;
}

float CCamera::GetZoom()
{
	return mZoom;
}

void CCamera::LoadSettings()
{
	CXml xml("data/camera.xml", "root");

	if ( xml.GetString(0, "type") == "camera" )
	{
		mSettings.zoom = xml.GetString("zoom");
	}
}

void CCamera::UpdateSettings()
{
	static double resX;
	static double resY;

	if ( mParser == NULL )
	{
		mParser = new mu::Parser();
		mParser->DefineVar( "resX", &resX );
		mParser->DefineVar( "resY", &resY );
		mParser->SetExpr( mSettings.zoom );
	}

	float zoom = 1.0f;

	try 
	{
		resX = gGameOptions.GetWidth();
		resY = gGameOptions.GetHeight();
		zoom = (float) mParser->Eval();
	} 
	catch ( mu::Parser::exception_type &e )
	{
		fprintf( stderr, "error - parsing of expresion %s failed: %s\n", mSettings.zoom.c_str(), e.GetMsg().c_str() );
	}

	SetZoom( zoom );
}

void CCamera::Update(float secondsPassed)
{
	sf::Vector2f position;
	sf::Vector2f target( 0.0f, 0.0f );

	if ( mTask == CCamera::STAND )					target = GetPositionInGame(); 
	else if ( mTask == CCamera::GO_TO_POINT )		target = mGoTo;
	else if ( mTask == CCamera::TRAIL_PHYSICAL )	target = mTrail->GetPosition();
	else if ( mTask == CCamera::TRAIL_PLAYERS )
	{
		unsigned i = 0;
		unsigned playersCnt = gPlayerManager.GetPlayerCount();

		while ( i < playersCnt )
		{
			target += gPlayerManager.GetPlayerByOrder(i)->GetPosition() / (float) playersCnt;
			++i;
		} 

		if ( playersCnt == 0 )
			target = GetPositionInGame();
	}

	if ( mSwayingTime > 0.0f )
	{
		position = Maths::Lerp( GetPositionInGame(), target, std::min( secondsPassed / mSwayingTime, 1.0f ) );
		mSwayingTime -= secondsPassed;
	}
	else
		position = target;

	SetPosition( position );
}

void CCamera::FrameStarted(float secondsPassed)
{
	Update(secondsPassed);
}

void CCamera::SetPosition(sf::Vector2f v)
{
    std::vector< CQuadTreeSceneNode > & quadTreeNodes = gScene.GetQuadTreeNodes();
    if ( quadTreeNodes.size() == 0 )
		return;

	const sf::FloatRect &window = gGame.GetRenderWindow()->GetDefaultView().GetRect();

    sf::FloatRect sceneRect = quadTreeNodes[0].GetBoundingRect();
	sf::FloatRect sceneRect_;
	sceneRect_.Left = sceneRect.Left - mViewArea.Left * window.GetWidth() / Map::TILE_SIZE;
	sceneRect_.Top = sceneRect.Top - mViewArea.Top * window.GetHeight() / Map::TILE_SIZE;
	sceneRect_.Right = sceneRect.Right + (1.0f - mViewArea.Right) * window.GetWidth() / Map::TILE_SIZE;
	sceneRect_.Bottom = sceneRect.Bottom + (1.0f - mViewArea.Bottom) * window.GetHeight() / Map::TILE_SIZE;

    if (sceneRect_.GetWidth() * Map::TILE_SIZE < window.GetWidth())
        v.x = sceneRect_.GetWidth() * Map::TILE_SIZE / 2.0f;
    else {
        v.x = Maths::Clamp( v.x * Map::TILE_SIZE , 
            sceneRect_.Left * Map::TILE_SIZE + window.GetWidth() / 2.0f, 
            sceneRect_.Right * Map::TILE_SIZE - window.GetWidth() / 2.0f);
    }
    if (sceneRect_.GetHeight() * Map::TILE_SIZE < window.GetHeight())
        v.y = sceneRect_.GetHeight() * Map::TILE_SIZE / 2.0f;
    else {
        v.y = Maths::Clamp(v.y * Map::TILE_SIZE, 
            sceneRect_.Top * Map::TILE_SIZE + window.GetHeight() / 2.0f, 
            sceneRect_.Bottom * Map::TILE_SIZE - window.GetHeight() / 2.0f);
    }

    mPosition.x = v.x;
    mPosition.y = v.y;
    gGame.GetRenderWindow()->GetDefaultView().SetCenter( mPosition );
}

