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
#include "CInGameOptionChooser.h"

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

	/* TEMP */
	CTextArea* gfps = CreateTextArea("fpstext",true,Z_GUI4);
	gfps->SetPosition(2.0f,2.0f,20.0f,10.0f);
	gfps->SetFont(gLocalizator.GetFont(GUI::FONT_DIALOG), 14 );
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
	if (e.Button == sf::Mouse::Left)		eventType = MOUSE_PRESSED_LEFT;
	else if (e.Button == sf::Mouse::Right )	eventType = MOUSE_PRESSED_RIGHT;
	else return;

	SendMouseEvent( x, y, eventType );
}

void CRoot::MouseReleased(const sf::Event::MouseButtonEvent &e)
{
	float x = (float) e.X * (float) gGameOptions.GetWidth() / gGame.GetRenderWindow()->GetWidth();
	float y = (float) e.Y * (float) gGameOptions.GetHeight() / gGame.GetRenderWindow()->GetHeight();

	mouseEvent eventType;
	if (e.Button == sf::Mouse::Left)		eventType = MOUSE_RELEASED_LEFT;
	else if (e.Button == sf::Mouse::Right)	eventType = MOUSE_RELEASED_RIGHT;
    else return;

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
                CAbiSlotsBar* bar0 = gLogic.GetGameScreens()->GetAbiBar(0);
                CAbiSlotsBar* bar1 = gLogic.GetGameScreens()->GetAbiBar(1);

                if (bar0)
                    bar0->SetSlotBackground("data/GUI/abi-bar/abi-bar-scroll.png");
                if (bar1)
                    bar1->SetSlotBackground("data/GUI/abi-bar/abi-bar-scroll.png");

                // wlasciwy drop
                bool cont = false;

                mDraggedImg->SetVisible(false);
                if (mDraggedImg->mForegroundSprite != NULL)
                {
                    gDrawableManager.DestroyDrawable(mDraggedImg->mForegroundSprite);
                    mDraggedImg->mForegroundSprite = NULL;
                }

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
                CButton* btn = dynamic_cast<CButton*>(*it);
                if (btn != NULL)
                {
                    mDraggedImg->mForegroundSprite = gDrawableManager.CloneHudSprite(btn->GetNormalSprite(), Z_DRAGGED_IMG);
                    mDraggedImg->SetPosition(x, y, btn->mGlobalSize.x, btn->mGlobalSize.y, UNIT_PIXEL);
                    mDraggedImg->UpdateSprites(0.f);
                    mDraggedImg->SetVisible(true);
                    mDraggedObject = *it;
                }

                CAbilitySlot* slot = dynamic_cast<CAbilitySlot*>(*it);
                if (slot != NULL)
                {
                    mDraggedImg->mForegroundSprite = gDrawableManager.CloneHudSprite(slot->GetIconSprite(), Z_DRAGGED_IMG);
                    mDraggedImg->SetPosition(x, y, slot->mGlobalSize.x, slot->mGlobalSize.y, UNIT_PIXEL);
                    mDraggedImg->UpdateSprites(0.f);
                    mDraggedImg->SetVisible(true);
                    mDraggedObject = *it;
                }

                // podswietlanie slotow
                if (mDraggedObject != NULL)
                {
                    CAbiSlotsBar* bar0 = gLogic.GetGameScreens()->GetAbiBar(0);
                    CAbiSlotsBar* bar1 = gLogic.GetGameScreens()->GetAbiBar(1);

                    if (bar0)
                        bar0->SetSlotBackground("data/GUI/abi-bar/abi-bar-scroll-hl.png");
                    if (bar1)
                        bar1->SetSlotBackground("data/GUI/abi-bar/abi-bar-scroll-hl.png");
                }
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
