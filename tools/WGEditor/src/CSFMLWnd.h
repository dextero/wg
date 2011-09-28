#ifndef __CSFMLWND_H__
#define __CSFMLWND_H__

#include <SFML/Graphics.hpp>
#include "wx/msw/winundef.h"
#include "wx/wx.h"

class CSFMLWnd: public wxControl, public sf::RenderWindow
{
public:
	CSFMLWnd(wxWindow* parent = NULL, wxWindowID id = -1, const wxPoint& position = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~CSFMLWnd();

    void UpdateGameWindow();

private:

    // do ograniczania fpsow sleep()em
    const unsigned int mSleepMilliseconds;

    // eventy
	DECLARE_EVENT_TABLE()

	virtual void OnUpdate();
    void OnIdle(wxIdleEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
};

#endif // __CSFMLWND_H__