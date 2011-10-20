#ifndef CAMERA_H_
#define CAMERA_H_

// tox: mysle, ze kamera moze roboczo byc singletonem
//
// ta klasa ma opakowac funkcjonalnosc sf::View

#include "../Utils/CSingleton.h"
#include "../IFrameListener.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string>

namespace mu {
	class Parser;
}
class CPhysical;

#define gCamera CCamera::GetSingleton()

/* to tak myslac bardziej przyszlosciowo... */
struct SCameraSettings
{
	std::string zoom;
};

class CCamera : public CSingleton<CCamera>, public IFrameListener {
public:

	enum cameraTask
	{
		STAND,
		GO_TO_POINT,
		TRAIL_PHYSICAL,
		TRAIL_PLAYERS
	};

    CCamera();
    virtual ~CCamera();

	void GoTo( const sf::Vector2f& v, float swayingTime = 0.0f );
	void GoTo( float x, float y, float swayingTime = 0.0f );
	void TrailPhysical( CPhysical* newTrail, float swayingTime = 0.0f );
	void TrailPlayers( float swayingTime = 0.0f );
	void StopCamera();

	void SetViewArea( float left, float top, float right, float bottom );	// obszar widoku - czesc ekranu niezaslonieta przez hudy etc. [%]
	void SetZoom( float zoom );

	inline void Shake(float time, float strength, float frequency) {
		mShakeTime = time;
		mShakeStrength = strength;
		mShakeFrequency = frequency;
	}

    sf::Vector2f  GetPosition();				 // pozycja w pixelach, nie kaflach
    sf::Vector2f  GetPositionInGame();
    sf::Vector2f  GetViewTopLeft();
	sf::Vector2f  TileToGui(sf::Vector2f tile);  //6.06.09, rAum: Konwertuje pozycje kaflowa na 0..100 (uzywane przez GUI)
	sf::FloatRect GetViewRectInGame();
	cameraTask	  GetCameraTask();
	float		  GetZoom();

	void LoadSettings();
	void UpdateSettings();

	void Update(float secondsPassed);
    virtual void FrameStarted(float secondsPassed);
	virtual bool FramesDuringPause(EPauseVariant pv) { return pv == pvLogicOnly; }
    
private:
	void SetPosition( sf::Vector2f v );

	sf::Vector2f mPosition;
	sf::Vector2f mGoTo;
	CPhysical*	 mTrail;
	cameraTask	 mTask;
	float		 mSwayingTime;
	float		 mZoom;
	float		 mPrimaryZoom;

	float mShakeTime;
	float mShakeStrength;
	float mShakeFrequency;

	SCameraSettings mSettings;
	mu::Parser *	mParser;

	sf::Rect<float>	mViewArea;
};

#endif /*CCAMERA_H_*/
