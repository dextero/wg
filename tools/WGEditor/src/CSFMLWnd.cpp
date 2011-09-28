#include "CSFMLWnd.h"

#include "Audio/CAudioManager.h"
#include "GUI/CRoot.h"
#include "Logic/CPhysicalManager.h"
#include "Map/CMapManager.h"
#include "Rendering/CDrawableManager.h"
#include "Rendering/CCamera.h"
#include "CGameOptions.h"
#include "CGame.h"

#ifdef __WXGTK__
    #include <gdk/gdkx.h>
    #include <gtk/gtk.h>
    #include <wx/gtk/win_gtk.h>
#endif

BEGIN_EVENT_TABLE(CSFMLWnd, wxControl)
    EVT_IDLE(CSFMLWnd::OnIdle)
    EVT_PAINT(CSFMLWnd::OnPaint)
    EVT_ERASE_BACKGROUND(CSFMLWnd::OnEraseBackground)
END_EVENT_TABLE()

CSFMLWnd::CSFMLWnd(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style):
	wxControl(parent, id, position, size, style),
    mSleepMilliseconds(1000u / 30u) // ms / klatek - troche wiecej niz 30, zeby byl czas na inne rzeczy
{
    #ifdef __WXGTK__

        // GTK implementation requires to go deeper to find the
        // low-level X11 identifier of the widget
        gtk_widget_realize(m_wxwindow);
        gtk_widget_set_double_buffered(m_wxwindow, false);
        GdkWindow* win = GTK_PIZZA(m_wxwindow)->bin_window;
        XFlush(GDK_WINDOW_XDISPLAY(win));
        sf::RenderWindow::Create(GDK_WINDOW_XWINDOW(win));

    #else

        // Tested under Windows XP only (should work with X11
        // and other Windows versions - no idea about MacOS)
        sf::RenderWindow::Create(GetHandle());

    #endif

    CGame::Init(this);
}

CSFMLWnd::~CSFMLWnd()
{
    sf::RenderWindow::Close();
}


void CSFMLWnd::UpdateGameWindow()
{
    gGameOptions.SetWidth(GetSize().x);
    gGameOptions.SetHeight(GetSize().y);

    GetDefaultView().SetHalfSize(GetSize().x / 2, GetSize().y / 2);
}


// eventy

void CSFMLWnd::OnUpdate()
{
    gGame.MainLoopStep();

    // rysowanie sceny - MainLoopStep w edytorze tego nie ma :)
    glClear(GL_COLOR_BUFFER_BIT);
    gDrawableManager.DrawFrame(this);

    // zzzzz
    wxMilliSleep(mSleepMilliseconds);
}

void CSFMLWnd::OnIdle(wxIdleEvent& event)
{
    // Send a paint message when the control is idle, to ensure maximum framerate
    Refresh();
}

void CSFMLWnd::OnPaint(wxPaintEvent& event)
{
    // Prepare the control to be repainted
    wxPaintDC dc(this);

    // Let the derived class do its specific stuff
    OnUpdate();

    // Display on screen
    Display();
}

void CSFMLWnd::OnEraseBackground(wxEraseEvent& event)
{
}