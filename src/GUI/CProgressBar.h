/* GUI - pasek postepu (wszelkie wskazniki hp, many etc.) */

#ifndef _PROGRESS_BAR_
#define _PROGRESS_BAR_

#include "CGUIObject.h"
#include "../Utils/Maths.h"
#include <string>

class CHudSprite;

namespace GUI
{
	class CProgressBar : public CGUIObject
	{
	public:
		void SetBorderImage( const std::string& filename );
		void SetProgressImage( const std::string& filename );

		/*	==========================================================
		*	Ustaw plaszczyzny obcinajace w postaci: 
		*	k¹t wektora normalnego p³aszczyzny obcinaj¹cej liczony zgodnie ze wskazówkami zegara od wektora skierowanego do góry	
		*	i punkt (w procentach) zawierajacy sie w niej przy minimalnej i maksymalnej wartosci mProgress.
		*	========================================================== */

		inline void SetMinClippingPlane( float normalAngle, sf::Vector2f point )	{ mMinCPNormalAngle = normalAngle; mMinCPPoint = point / 100.0f; }
		inline void SetMaxClippingPlane( float normalAngle, sf::Vector2f point )	{ mMaxCPNormalAngle = normalAngle; mMaxCPPoint = point / 100.0f; }
		inline void SetProgress( float progress )	{ mProgress = Maths::Clamp( progress, 0.0f, 1.0f ); }
		inline float GetProgress()					{ return mProgress; }

		// alpha, z zakresu 0..1
		inline float GetAlpha() { return mAlpha; }
		inline void SetAlpha(float a) { mAlpha = a;}
	private:
		CProgressBar( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CProgressBar();
		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		CHudSprite* mBorderSprite;
		CHudSprite* mProgressSprite;
		float mMinCPNormalAngle;
		float mMaxCPNormalAngle;
		sf::Vector2f mMinCPPoint;
		sf::Vector2f mMaxCPPoint;
		float mProgress;
		float mAlpha;
		friend class CGUIObject;
	};
};

#endif

