/*	GUI - scrollbar
*/

#ifndef _SCROLLBAR_H_
#define _SCROLLBAR_H_

#include "CGUIObject.h"
#include <string>
#include <FastDelegate.h>
#include "../Utils/MathsFunc.h"

class CHudSprite;
typedef fastdelegate::FastDelegate0<void> slide_callback;
typedef fastdelegate::FastDelegate2<void*, unsigned, void> slide_param_callback;

namespace GUI
{
	enum guiOrientation
	{
		ORIENTATION_X,
		ORIENTATION_Y
	};

	class CScrollBar : public CGUIObject
	{
	public:
		void SetBackgroundImage( const std::string& filename );
		void SetHandleImage( const std::string& filename );

		inline void SetOrientation( guiOrientation o )	{ mOrientation = o; }
		
		inline float GetState()							{ return int(mState * (mStatesCount-1))/float(mStatesCount-1); }
		inline void SetState( float state )				{ mState = Maths::Clamp( state, 0.0f, 1.0f ); }

		inline unsigned GetStatesCount()				{ return mStatesCount; }
		inline void SetStatesCount( unsigned scount )	{ mStatesCount = scount; }

		inline float GetHandleSize()					{ return mHandleSize * 100.0f; }
		inline void SetHandleSize( float s )			{ mHandleSize = s / 100.0f; }

		inline slide_callback* GetSlideCallback()	{ return &mSlideCallback; }
		inline slide_param_callback* GetSlideParamCallback() { return &mSlideParamCallback; }
		inline void SetSlideCallbackParams( void* params ) { mSlideCallbackParams = params; }

	private:
		CScrollBar( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CScrollBar();
		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		void UpdateState( float mouseX, float mouseY );
		void GetHandleData( sf::Vector2f& position, sf::Vector2f& size );

		CHudSprite*	mBackgroundSprite;
		CHudSprite* mHandleSprite;

		float			mState;
		unsigned		mStatesCount;
		float			mHandleSize;
		guiOrientation	mOrientation;
		bool			mActive;

		slide_callback mSlideCallback;
		slide_param_callback mSlideParamCallback;
		void* mSlideCallbackParams;

		friend class CGUIObject;
	};
};

#endif//_SCROLLBAR_H_

