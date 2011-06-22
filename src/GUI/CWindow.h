/* GUI - okno */

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "CGUIObject.h"
#include <string>

class CHudSprite;

namespace GUI
{
	class CWindow : public CGUIObject
	{
	public:
		void SetBackgroundImage( const std::string& filename );

	private:
		CWindow( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CWindow();
		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		CHudSprite* mBackgroundSprite;
		friend class CGUIObject;
	};
};

#endif// _WINDOW_H_

