/* GUI - panel wielooknowy */

#ifndef _MULTIPANEL_H_
#define _MULTIPANEL_H_

#include "CGUIObject.h"
#include <string>
#include <vector>

class CHudSprite;

namespace GUI
{
    class CWindow;

	class CMultipanel : public CGUIObject
	{
	public:
        CHudSprite *GetSwitcherImage( unsigned int i );
		void SetSwitcherImage( unsigned int i, const std::string &filename );

        CWindow *GetActiveWindow();
        unsigned int GetActiveWindowIndex();
        CWindow *GetWindow(unsigned int i);

        /// ile % zajmuja "przelaczacze paneli" na gorze
        /// liczba z zakresu 0-100
        void SetSwitchersToPanelRatio(float ratio);
        
		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
	private:
		friend class CGUIObject;

		CMultipanel( const std::string& name, CGUIObject* parent, unsigned int zindex, unsigned int panelCount );
		virtual ~CMultipanel();
		virtual void UpdateSprites( float secondsPassed );

        std::vector<CWindow*> mWindows;
        std::vector<CHudSprite*> mSwitchers;
        float mRatio;

        unsigned int mActiveWnd;
	};
};

#endif// _MULTIPANEL_H_

