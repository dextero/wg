#include "CRoot.h"
#include "CWindow.h"
#include "CButton.h"
#include "CProgressBar.h"
#include "CTextArea.h"
#include "CCheckBox.h"
#include "CScrollBar.h"
#include "CScrollPanel.h"
#include "CDropList.h"
#include "CImageBox.h"
#include "CAbilitySlot.h"
#include "CAbiSlotsBar.h"
#include "CInGameOptionChooser.h"
#include "../Logic/OptionChooser/InteractionHandler.h"
#include "CItemSlot.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Logic/CLogic.h"
#include <assert.h>
#include <cstdio>
#include <SFML/Graphics/RenderWindow.hpp>
#include "../Rendering/ZIndexVals.h"
#include "../Console/CConsole.h"
#include "Localization/CLocalizator.h"
#include "../Input/CBindManager.h"
#include "../Logic/CPlayerManager.h"
#include "../Rendering/CHudSprite.h"
#include "../Rendering/CDrawableManager.h"
#include "../Utils/CXml.h"

#undef CreateWindow
#undef MOUSE_MOVED

using namespace GUI;

template<> CRoot* CSingleton<CRoot>::msSingleton = 0;

CRoot::CRoot()
:	CGUIObject("Root", GUI_ROOT, NULL, Z_GUI1),
	mActiveObject(NULL),
	mFocusedObject(NULL),
    mCursor(NULL),
    mDraggedObject(NULL),
    mDraggedImg(NULL)
{
	fprintf( stderr, "GUI::CRoot::CRoot()\n" );

	gGame.AddFrameListener( this );
	gGame.AddKeyListener( this );
	gGame.AddMouseListener( this );

    CXml xml("data/gui.xml", "root");
    for (xml_node<>* node = xml.GetChild(NULL, "fontSetting"); node; node = xml.GetSibl(node, "fontSetting")) {
        GUI::FontSetting fs;
        std::string id = xml.GetString(node, "id");
        fs.name = xml.GetString(node, "name");
        fs.size = xml.GetFloat(node, "size");
        fs.unit = xml.GetString(node, "unit") == "UNIT_PIXEL" ? GUI::UNIT_PIXEL : GUI::UNIT_PERCENT;
        mFontSettings[id] = fs;
    }

	/* TEMP */
	CTextArea* gfps = CreateTextArea("fpstext",true,Z_GUI4);
	gfps->SetPosition(2.0f,2.0f,20.0f,10.0f);
    gfps->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
	gfps->SetVisible(false);

    mCursor = CreateImageBox("cursor", true, Z_CURSOR);
    mCursor->SetVisible(true);
    mCursor->AddImageToSequence("data/GUI/cur-0.png");
    mCursor->SetSequenceState(0);
    mCursor->SetPosition(0.f, 0.f, 32.f, 32.f, UNIT_PIXEL);

    mDraggedImg = CreateImageBox("dragged-image", true, Z_DRAGGED_IMG);
    mDraggedImg->SetVisible(false);

    // dex: zeby kursor nie bral udzialu w sprawdzaniu z-index (mouse-events)
    UnregisterObject(mCursor);
    UnregisterObject(mDraggedImg);
}

CRoot::~CRoot()
{
}

FontSetting CRoot::GetFontSetting(const std::string & id, float scale)
{
    FontSetting fs;
    if (mFontSettings.find(id) != mFontSettings.end()) {
        fs = mFontSettings[id];
        fs.size *= scale;
    } else {
        fprintf(stderr, "warning: CRoot::GetFontSetting, can't find font with id=`%s'\n", id.c_str());
        fs.name = "data/GUI/verdana.ttf";
        fs.size = 14.f;
        fs.unit = GUI::UNIT_PIXEL;
    }
    return fs;
}

static std::vector<InteractionHandler *> sInteractionHandlersToLazilyUnregister;

void CRoot::FrameStarted(float secondsPassed)
{
	mGlobalSize.x = (float) gGameOptions.GetWidth();
	mGlobalSize.y = (float) gGameOptions.GetHeight();

	mInternalRect.Left =	0.0f;
	mInternalRect.Top =		0.0f;
	mInternalRect.Right =	mGlobalSize.x;
	mInternalRect.Bottom =	mGlobalSize.y;

	mClippingRect.Left =	0.0f;
	mClippingRect.Top =		0.0f;
	mClippingRect.Right =	mGlobalSize.x;
	mClippingRect.Bottom =	mGlobalSize.y;

	UpdateChilds( secondsPassed );
    CInGameOptionChooser::UpdateAll(secondsPassed);
    for (std::vector<InteractionHandler *>::iterator it = sInteractionHandlersToLazilyUnregister.begin() ;
            it != sInteractionHandlersToLazilyUnregister.end() ; it++) {
        mInteractionHandlers.erase(mInteractionHandlers.find(*it));
    }
    sInteractionHandlersToLazilyUnregister.clear();
    for (std::set<InteractionHandler *>::iterator it = mInteractionHandlers.begin() ; it != mInteractionHandlers.end() ; it++) {
        (*it)->Update(secondsPassed);
    }
        
}

void CRoot::KeyPressed(const sf::Event::KeyEvent &e)
{
	if (mActiveObject)
		mActiveObject->OnKeyEvent( e, true );
}

void CRoot::KeyReleased(const sf::Event::KeyEvent &e)
{
	if (mActiveObject)
		mActiveObject->OnKeyEvent( e, false );
}

void CRoot::MousePressed(const sf::Event::MouseButtonEvent &e)
{
	float x = (float) e.X * gGameOptions.GetWidth() / gGame.GetRenderWindow()->GetWidth();
	float y = (float) e.Y * gGameOptions.GetHeight() / gGame.GetRenderWindow()->GetHeight();

	mouseEvent eventType;
    switch (e.Button)
    {
    case sf::Mouse::Left:       eventType = MOUSE_PRESSED_LEFT; break;
    case sf::Mouse::Right:      eventType = MOUSE_PRESSED_RIGHT; break;
    case sf::Mouse::Middle:     eventType = MOUSE_PRESSED_MIDDLE; break;
    case sf::Mouse::XButton1:   eventType = MOUSE_PRESSED_X1; break;
    case sf::Mouse::XButton2:   eventType = MOUSE_PRESSED_X2; break;
    default:
        return;
    }

	SendMouseEvent( x, y, eventType );
}

void CRoot::MouseReleased(const sf::Event::MouseButtonEvent &e)
{
	float x = (float) e.X * (float) gGameOptions.GetWidth() / gGame.GetRenderWindow()->GetWidth();
	float y = (float) e.Y * (float) gGameOptions.GetHeight() / gGame.GetRenderWindow()->GetHeight();

	mouseEvent eventType;
    switch (e.Button)
    {
    case sf::Mouse::Left:       eventType = MOUSE_RELEASED_LEFT; break;
    case sf::Mouse::Right:      eventType = MOUSE_RELEASED_RIGHT; break;
    case sf::Mouse::Middle:     eventType = MOUSE_RELEASED_MIDDLE; break;
    case sf::Mouse::XButton1:   eventType = MOUSE_RELEASED_X1; break;
    case sf::Mouse::XButton2:   eventType = MOUSE_RELEASED_X2; break;
    default:
        return;
    }

	SendMouseEvent( x, y, eventType );
}

void CRoot::MouseMoved(const sf::Event::MouseMoveEvent &e)
{
	float x = (float) e.X * gGameOptions.GetWidth() / gGame.GetRenderWindow()->GetWidth();
	float y = (float) e.Y * gGameOptions.GetHeight() / gGame.GetRenderWindow()->GetHeight();

	SendMouseEvent( x, y, GUI::MOUSE_MOVED );

	//warning C4244: 'argument' : conversion from 'const int' to 'float', possible loss of data
    mCursor->SetPosition((float)e.X, (float)e.Y, mCursor->GetSize(UNIT_PIXEL).x, mCursor->GetSize(UNIT_PIXEL).y, UNIT_PIXEL);
    mDraggedImg->SetPosition((float)e.X, (float)e.Y, mDraggedImg->GetSize(UNIT_PIXEL).x, mDraggedImg->GetSize(UNIT_PIXEL).y, UNIT_PIXEL);
}

void CRoot::MouseWheelMoved( const sf::Event::MouseWheelEvent& e )
{
    SendMouseEvent(0.f, (float)e.Delta, (e.Delta < 0 ? GUI::MOUSE_WHEEL_DOWN : GUI::MOUSE_WHEEL_UP));

    // wez obiekt spod kursora
    CGUIObject* obj = GetFocusedObject();
    if (obj == NULL)
        return;

    do {
        // sprawdz, czy to nie scrollpanel
        CScrollPanel* panel = dynamic_cast<CScrollPanel*>(obj);
        if (panel != NULL)
        {
            // znajdz powiazanego scrollbara (pionowy ma pierwszenstwo)..
            CScrollBar* bar = panel->GetScrollBarY();
			//warning C4244: 'initializing' : conversion from 'const int' to 'float', possible loss of data
            float scroll = -(float)e.Delta;
            if (!bar)
            {
                bar = panel->GetScrollBarX();
                if (!bar)
                    return;
                else
                {
                    // wymus wyliczenie content-size
                    bar->SetState(bar->GetState() + 0.011f);
                    panel->ForceUpdateContentSize();
                    scroll *= panel->ConvertToGlobalPosition(panel->GetSize()).x / panel->GetContentSize().x / 5.f;
                }
            }
            else
            {
                // wymus wyliczenie content-size
                bar->SetState(bar->GetState() + 0.011f);
                panel->ForceUpdateContentSize();
                scroll *= panel->ConvertToGlobalPosition(panel->GetSize()).y / panel->GetContentSize().y / 5.f;
            }

            // ..i przesun, cofajac wymuszone przesuniecie..
            bar->SetState(bar->GetState() + scroll - 0.011f);
            return;
        }

        // jesli to nie scrollpanel, wez rodzica obiektu i probuj jeszcze raz
        if ((obj = obj->GetParent()) == NULL)
            return;
    } while (obj != this);
}

void CRoot::Cleanup()
{
	Remove();
}

void CRoot::RegisterObject(CGUIObject* object)
{
	if ( object->mType != GUI_ROOT )
	{
		std::list<CGUIObject*>::iterator it = mSortedObjects.begin();
		while ( it != mSortedObjects.end() && object->mZIndex > (*it)->mZIndex )
			++it;
		mSortedObjects.insert( it, object );
	}
}

void CRoot::UnregisterObject(CGUIObject* object)
{
	if ( object->mType != GUI_ROOT )
	{
		mSortedObjects.remove( object );
	}
}

void CRoot::SendMouseEvent(float x, float y, mouseEvent e)
{
    static mouseEvent prevEvent = MOUSE_MOVED;
    static sf::Clock prevEventClock;

	//sf::Vector2f chPos, chSize;
	std::list<CGUIObject*>::iterator it;

	CGUIObject* active = mActiveObject;
	bool activeResult = false;

	if ( active )
		activeResult = active->OnMouseEvent( x, y, e );


	for ( it = mSortedObjects.begin(); it != mSortedObjects.end(); ++it )
	{
		sf::Vector2f&	chPos =			(*it)->mGlobalPosition;
		sf::Vector2f&	chSize =		(*it)->mGlobalSize;
		sf::FloatRect&	chClippingRect =(*it)->mClippingRect;

		if (	( x >= chPos.x ) &&
				( x <= chPos.x + chSize.x ) &&
				( y >= chPos.y ) &&
				( y <= chPos.y + chSize.y ) &&
				( chClippingRect.Contains(x,y) ) &&
				( (*it)->IsVisible() ) )
		{
            // drop
            if ( e == MOUSE_RELEASED_LEFT && mDraggedObject != NULL )
            {
                // wylacz podswietlanie slotow
                /*CAbiSlotsBar* bar0 = gLogic.GetGameScreens()->GetAbiBar(0);
                CAbiSlotsBar* bar1 = gLogic.GetGameScreens()->GetAbiBar(1);

                if (bar0)
                    bar0->SetSlotBackground("data/GUI/abi-bar/abi-bar-scroll.png");
                if (bar1)
                    bar1->SetSlotBackground("data/GUI/abi-bar/abi-bar-scroll.png");
                */

                // wlasciwy drop
                bool cont = false;

                mDraggedImg->SetVisible(false);
                if (mDraggedImg->mForegroundSprite != NULL)
                {
                    gDrawableManager.DestroyDrawable(mDraggedImg->mForegroundSprite);
                    mDraggedImg->mForegroundSprite = NULL;
                }

                if (dynamic_cast<CItemSlot*>(*it) == NULL)
                    // danger! probujemy wyrzucic umiejke poza sloty!
                    CItemSlot::UndoDrag();
                
                (*it)->OnMouseEvent(x, y, MOUSE_DROP);
                
                if ((*it) != mDraggedObject)
                    cont = true;
                mDraggedObject = NULL;

                if (cont)
                    continue;
            }
            // drag
            else if ( e == MOUSE_PRESSED_LEFT && (*it)->GetDraggable() )
            {
                if ((*it)->GetDragIcon())
                {
                    mDraggedImg->mForegroundSprite = gDrawableManager.CloneHudSprite((*it)->GetDragIcon(), Z_DRAGGED_IMG);
                    mDraggedImg->SetPosition(x, y, (*it)->mGlobalSize.x, (*it)->mGlobalSize.y, UNIT_PIXEL);
                    mDraggedImg->UpdateSprites(0.f);
                    mDraggedImg->SetVisible(true);
                    mDraggedObject = *it;
                }
                
                // podswietlanie slotow
                /*if (mDraggedObject != NULL)
                {
                    CAbiSlotsBar* bar0 = gLogic.GetGameScreens()->GetAbiBar(0);
                    CAbiSlotsBar* bar1 = gLogic.GetGameScreens()->GetAbiBar(1);

                    if (bar0)
                        bar0->SetSlotBackground("data/GUI/abi-bar/abi-bar-scroll-hl.png");
                    if (bar1)
                        bar1->SetSlotBackground("data/GUI/abi-bar/abi-bar-scroll-hl.png");
                }*/
            }

            if (e == MOUSE_PRESSED_LEFT && prevEvent == MOUSE_RELEASED_LEFT && prevEventClock.GetElapsedTime() < 0.2f)
                e = MOUSE_DBL_CLICK;

            prevEvent = e;
            prevEventClock.Reset();

			if ( (*it) == active )
			{
				if ( activeResult )
					continue;
			}
			else
			{
				if ( (*it)->OnMouseEvent(x,y,e) )
					continue;
			}

			SetFocusedObject( *it );
			return;
		}
	}

	SetFocusedObject( NULL );
}

void CRoot::SetActiveObject(CGUIObject* active)
{
	if ( mActiveObject != active )
	{
		CGUIObject * old = mActiveObject;
		mActiveObject = active;

		if ( old )
		{
			old->OnMouseEvent( 0.0f, 0.0f, MOUSE_UNCLICK );
		}
	}
}

void CRoot::RegisterInteractionHandler(InteractionHandler * handler) {
    mInteractionHandlers.insert(handler);
}

void CRoot::UnregisterInteractionHandler(InteractionHandler * handler) {
    sInteractionHandlersToLazilyUnregister.push_back(handler);
}

void CRoot::SetFocusedObject(CGUIObject* focused)
{
	if ( mFocusedObject != focused )
	{
		CGUIObject * old = mFocusedObject;
		mFocusedObject = focused;

		if ( old )				old->OnMouseEvent( 0.0f, 0.0f, MOUSE_OUT );
		if ( mFocusedObject )	mFocusedObject->OnMouseEvent( 0.0f, 0.0f, MOUSE_OVER );
	}
}

void CRoot::ShowCursor(bool show)
{
#ifndef __EDITOR__
    //gGame.GetRenderWindow()->ShowMouseCursor(show);
    if (show)								mCursor->SetVisible(true);
    else
    {
        // edytor
        if (gPlayerManager.GetPlayerCount() == 0)
            return;
        if (gBindManager0->GetMouseLook() || (gBindManager0->GetMouseCaster() != NULL) ||
                (gPlayerManager.GetPlayerCount() == 2 && (gBindManager1->GetMouseLook() || (gBindManager1->GetMouseCaster() != NULL))))
            return;
        mCursor->SetVisible(false);
    }
#else
    mCursor->SetVisible(false);
#endif
}
