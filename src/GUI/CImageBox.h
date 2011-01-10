/* GUI - okno */

#ifndef _IMAGEBOX_H_
#define _IMAGEBOX_H_

#include "CGUIObject.h"
#include <string>
#include <vector>

class CHudSprite;

namespace GUI
{
	class CImageBox : public CGUIObject
	{
	public:
		void AddImageToSequence( const std::string& filename );
		void SetSequenceState( float state ); // 0 - 1
		void SetSequenceState( int img );
		void ReleaseImages();

		inline size_t GetImagesCount()					{ return mImages.size(); }

		inline void SetRotationSpeed( float speed )		{ mRotationSpeed = speed; }
		inline void SetViewSize( float size )			{ mViewSize = size / 100.0f; }
		inline void SetViewPosition( sf::Vector2f p )	{ mViewPosition = p / 100.0f; }

		inline float GetViewSize()						{ return mViewSize * 100.0f; }
		inline sf::Vector2f GetViewPosition()			{ return mViewPosition * 100.0f; }

		inline float GetRotationSpeed()                 { return mRotationSpeed; }

	private:
		CImageBox( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CImageBox();
		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		std::vector<std::string> mImages;
		float mState;
		float mRotationSpeed;
		float mViewSize;
		sf::Vector2f mViewPosition;
		bool mUpdate;

		CHudSprite* mBackgroundSprite;
		CHudSprite* mForegroundSprite;

		friend class CGUIObject;
        friend class CRoot;
        friend class CCompass;
	};
};

#endif// _IMAGEBOX_H_

