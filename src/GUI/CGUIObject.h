/*	Klasa bazowa dla wszystkich elementow GUI *
*/

#ifndef _GUI_OBJECT_H_
#define _GUI_OBJECT_H_

#include "../Rendering/ZIndexVals.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include <FastDelegate.h>
#include "Localization/CLocale.h"
#undef MOUSE_MOVED
#undef CreateWindow

class CPlayer; 
class CAbilityTree;
class CHudSprite;

namespace GUI
{
	/* rodzaje zdarzen "generowanych" przez myszke */

	enum mouseEvent
	{
		MOUSE_PRESSED_LEFT = 0,
		MOUSE_PRESSED_RIGHT = 1,
		MOUSE_RELEASED_LEFT = 2,
		MOUSE_RELEASED_RIGHT = 3,
		MOUSE_MOVED = 4,
		MOUSE_OVER = 5,
		MOUSE_OUT = 6,
		MOUSE_UNCLICK = 7,
        MOUSE_DROP = 8,
        MOUSE_DBL_CLICK = 9,
        MOUSE_PRESSED_MIDDLE = 10,
        MOUSE_PRESSED_X1 = 11,
        MOUSE_PRESSED_X2 = 12,
        MOUSE_RELEASED_MIDDLE = 13,
        MOUSE_RELEASED_X1 = 14,
        MOUSE_RELEASED_X2 = 15
	};
    const unsigned int MOUSE_EVENT_TYPES = 16;

	/* wszystkie elementy GUI */

	enum guiType
	{
		GUI_ROOT,
		GUI_WINDOW,
		GUI_BUTTON,
		GUI_PROGRESS_BAR,
		GUI_TEXT_AREA,
		GUI_CHECK_BOX,
		GUI_SCROLL_BAR,
		GUI_DROP_LIST,
		GUI_IMAGE_BOX,
        GUI_MULTIPANEL,
        GUI_ABILITYTREEDISPLAYER,
		GUI_SCROLL_PANEL,
        GUI_EDITOR_RESOURCE_PANEL,
		GUI_KEY_READER,
        GUI_ABILITY_SLOT,
        GUI_ANIMATED_IMAGE,
        GUI_INVENTORY_DISPLAYER,
        GUI_ITEM_SLOT
	};

	/* jednostki opisujace pozycje etc. */
    // dex: po d³u¿szej walce z kompilatorem przenios³em do CLocale.h

	class CRoot;
	class CWindow;
	class CButton;
	class CProgressBar;
	class CTextArea;
	class CCheckBox;
	class CScrollBar;
	class CDropList;
	class CImageBox;
    class CMultipanel;
    class CAbilityTreeDisplayer;
	class CScrollPanel;
    class CEditorResourcePanel;
	class CKeyReader;
    class CAbilitySlot;
    class CAnimatedImage;
    class CInventoryDisplayer;
    class CItemSlot;

    typedef fastdelegate::FastDelegate0<void>                       EventCallback;
    typedef fastdelegate::FastDelegate1<const std::wstring&, void>  EventWStringCallback;
    typedef fastdelegate::FastDelegate1<int, void>                  EventIntCallback;
    typedef fastdelegate::FastDelegate1<void*, void>                EventVoidPtrCallback;

	/* klasa bazowa wszystkich elementow GUI */

	class CGUIObject
	{
	public:
		void SetPosition( float x, float y, float width, float height, guiUnit u = UNIT_PERCENT, bool fixedScale=false );		/* pobiera wspolrzedne relatywne */
		void SetPadding( float left, float top, float right, float bottom, guiUnit u = UNIT_PERCENT );	/* margines wewnetrzny */
        void SetPadding( const STextPadding& padding );
		virtual void SetVisible( bool newValue, bool recursive = true );
		void SetChildsClipping( bool clipping );
		void Remove();													/* usuwa obiekt wraz z dziecmi */
    
		CGUIObject*	FindObject( const std::string& name );				/* znajdz obiekt o wskazanej nazwie */
		
		sf::Vector2f ConvertToGlobalPosition( sf::Vector2f input );		/* przyjmuje 0..1f, zwraca pixele */
        sf::Vector2f ConvertFromGlobalPosition( sf::Vector2f input );	/* przyjmuje pixele, zwraca 0..1f */

		inline void SetOffset( const sf::Vector2f& offset )			{ mOffset = offset; }
		inline sf::Vector2f GetOffset()								{ return mOffset; }
		inline sf::Vector2f GetPosition( guiUnit u = UNIT_PERCENT )	{ return mPosition[u]; }
		inline sf::Vector2f GetSize( guiUnit u = UNIT_PERCENT )		{ return mSize[u]; }
		inline bool IsVisible()										{ return mIsVisible; }
        inline CGUIObject *GetParent()								{ return mParent; }
        inline void SetTooltip(CGUIObject* tt)                      { if (mTooltip) mTooltip->Remove(); mTooltip = tt; }
        inline CGUIObject* GetTooltip()                             { return mTooltip; }
        inline void SetDraggable(bool drag)                         { mDraggable = drag; }
        inline bool GetDraggable()                                  { return mDraggable; }
        // obrazek, jaki jest przenoszony podczas przeciagania
        virtual CHudSprite* GetDragIcon()                           { return NULL; }
		
		CWindow*				CreateWindow( const std::string& name, bool forceZ = false, int z = Z_GUI1 );	/* stworz dzieci*/
		CButton*				CreateButton( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
		CProgressBar*			CreateProgressBar( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
		CTextArea*				CreateTextArea( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
		CCheckBox*				CreateCheckBox( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
		CScrollBar*				CreateScrollBar( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
		CDropList*				CreateDropList( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
		CImageBox*				CreateImageBox( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
		CMultipanel*			CreateMultipanel( const std::string& name, int panelCount, bool forceZ = false, int z = Z_GUI1 );
        CAbilityTreeDisplayer*	CreateAbilityTreeDisplayer( const std::string& name, int player, const std::string &tree, bool forceZ = false, int z = Z_GUI1 );
		CScrollPanel*			CreateScrollPanel( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
        CEditorResourcePanel*   CreateEditorResourcePanel( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
		CKeyReader*				CreateKeyReader( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
        CAbilitySlot*           CreateAbilitySlot( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
        CAnimatedImage*         CreateAnimatedImage( const std::string& name, bool forceZ = false, int z = Z_GUI1 );
        CInventoryDisplayer*    CreateInventoryDisplayer( const std::string& name, int player, bool forceZ = false, int z = Z_GUI1);
        CItemSlot*              CreateItemSlot( const std::string& name, unsigned player, bool forceZ = false, int z = Z_GUI1 );

        void UpdateChilds( float secondsPassed );										/* uaktualnij wsp. globalne i sprite'y */ /*jesli !mIsVisible to nie uaktualnia*/

        // callbacki
        inline EventCallback*           GetEventCallback(mouseEvent event) { return &mEventCallback[event]; }
        inline EventWStringCallback*    GetEventWStringCallback(mouseEvent event) { return &mEventWStringCallback[event]; }
        inline EventVoidPtrCallback*    GetEventVoidPtrCallback(mouseEvent event) { return &mEventVoidPtrCallback[event]; }
        inline EventIntCallback*     GetEventIntCallback(mouseEvent event) { return &mEventIntCallback[event]; }

        // parametry callbackow
        inline void SetEventWStringParam(mouseEvent event, const std::wstring& param) { mEventWStringParam[event] = param; }
        inline void SetEventVoidPtrParam(mouseEvent event, void* param) { mEventVoidPtrParam[event] = param; }
        inline void SetEventIntParam(mouseEvent event, int param) { mEventIntParam[event] = param; }

	protected:
		CGUIObject( const std::string& name, guiType type, CGUIObject* parent, unsigned zindex );
		virtual ~CGUIObject();

		void RemoveChilds();

		virtual bool OnMouseEvent( float x, float y, mouseEvent e );                 	/* zwraca true jesli komunikat ma byc przeslany dalej */
		virtual void OnKeyEvent( const sf::Event::KeyEvent& e, bool pressed ) {}
		virtual void UpdateSprites( float secondsPassed ) {}

		sf::Vector2f	mPosition[UNIT_COUNT];
		sf::Vector2f	mSize[UNIT_COUNT]; 
		sf::Rect<float>	mPadding[UNIT_COUNT];

		sf::Vector2f	mGlobalPosition;
		sf::Vector2f	mGlobalSize;
		sf::Rect<float> mGlobalPadding;
		sf::Rect<float> mInternalRect;													/* wewnetrzny prostokat otaczajacy - z uwzglednieniem marginesu wewnetrznego */
		sf::Rect<float> mClippingRect;
		sf::Vector2f	mOffset;

		unsigned		mZIndex;
		unsigned		mZIndexCount;													/* ile warstw zawiera dany obiekt */
		std::string		mName;
		guiType			mType;
		CGUIObject*		mParent;
		bool            mIsVisible;
		bool			mChildsClipping;												/* czy przycinac dzieci do wewnetrznego prostokata otaczajacego */

		std::vector<CGUIObject*> mChilds;

        CGUIObject* mTooltip;
        bool mDraggable;

        // callbacki
        EventCallback           mEventCallback[MOUSE_EVENT_TYPES];
        EventWStringCallback    mEventWStringCallback[MOUSE_EVENT_TYPES];
        EventVoidPtrCallback    mEventVoidPtrCallback[MOUSE_EVENT_TYPES];
        EventIntCallback        mEventIntCallback[MOUSE_EVENT_TYPES];

        // parametry callbackow
        std::wstring            mEventWStringParam[MOUSE_EVENT_TYPES];
        void*                   mEventVoidPtrParam[MOUSE_EVENT_TYPES];
        int                     mEventIntParam[MOUSE_EVENT_TYPES];

		friend class CRoot;
		friend class CScrollPanel;
	};
};


#endif // _GUI_OBJECT_H_

