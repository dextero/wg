/* GUI - przewijany pojemnik */

#ifndef __CSCROLLPANEL_H__
#define __CSCROLLPANEL_H__

#include "CGUIObject.h"
#include <string>
#include <SFML/Graphics/Color.hpp>

namespace sf
{
    class String;
};

namespace GUI
{
	class CScrollBar;

	class CScrollPanel : public CGUIObject
	{
	public:
		inline void  SetScrollBarX( CScrollBar* s ) { mScrollBarX = s; }
		inline void	 SetScrollBarY( CScrollBar* s ) { mScrollBarY = s; }
        inline CScrollBar* GetScrollBarX() { return mScrollBarX; }
        inline CScrollBar* GetScrollBarY() { return mScrollBarY; }

        sf::Vector2f GetContentSize() { return mContentSize; }
        void ForceUpdateContentSize() { UpdateContentSize(); }

	private:
		sf::Vector2f mContentSize;
		sf::Vector2f mChildOffset;
		sf::Vector2f mScrollBarState;

		CScrollBar* mScrollBarX;
		CScrollBar* mScrollBarY;

		CScrollPanel( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CScrollPanel();
		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		void UpdateContentSize();

		friend class CGUIObject;
	};
};

#endif // __CSCROLLPANEL_H__


