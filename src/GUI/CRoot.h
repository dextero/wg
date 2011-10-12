/* GUI Root */

#ifndef _GUI_ROOT_H_
#define _GUI_ROOT_H_

#include "../Utils/CSingleton.h"
#include "../IKeyListener.h"
#include "../IMouseListener.h"
#include "../IFrameListener.h"
#include "CGUIObject.h"
#include <list>
#undef CreateWindow

#define gGUI GUI::CRoot::GetSingleton()

namespace GUI
{
    struct FontSetting {
        std::string name;
        float size;
        guiUnit unit;
    };

	class CRoot : public CSingleton< CRoot >, public CGUIObject, IFrameListener, IKeyListener, IMouseListener 
	{
	public:
		CRoot();
		~CRoot();

		virtual void FrameStarted( float secondsPassed );
		virtual bool FramesDuringPause(EPauseVariant pv) { return true; }

		virtual void KeyPressed( const sf::Event::KeyEvent &e );
		virtual void KeyReleased( const sf::Event::KeyEvent &e );

		virtual void MousePressed( const sf::Event::MouseButtonEvent &e );
		virtual void MouseReleased( const sf::Event::MouseButtonEvent &e );
		virtual void MouseMoved( const sf::Event::MouseMoveEvent& e );
        virtual void MouseWheelMoved( const sf::Event::MouseWheelEvent& e );

		virtual void Cleanup();

		void RegisterObject( CGUIObject* object );
		void UnregisterObject( CGUIObject* object );
		void SendMouseEvent( float x, float y, mouseEvent e );
		void SetActiveObject( CGUIObject* active );	 // obiekt który "zabiera" wszystkie komunikaty myszki
		void SetFocusedObject( CGUIObject* focused );// obiekt nad którym wisi kursor
		
		inline CGUIObject* GetActiveObject()  { return mActiveObject; }
		inline CGUIObject* GetFocusedObject() { return mFocusedObject; }
        inline CGUIObject* GetLastDraggedObject() { return mDraggedObject; }

        void ShowCursor(bool show=true);

        FontSetting GetFontSetting(const std::string & id);
	private:

		CGUIObject* mActiveObject;
		CGUIObject* mFocusedObject;
		std::list<CGUIObject*> mSortedObjects;
        CImageBox* mCursor;
        CGUIObject* mDraggedObject;
        CImageBox* mDraggedImg;
        std::map<std::string, FontSetting> mFontSettings;

		friend class CGUIObject;
	};
};

#endif // _GUI_ROOT_H_

