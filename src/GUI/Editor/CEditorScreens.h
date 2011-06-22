/* Plansze w edytorze */

#ifndef _EDITOR_SCREENS_
#define _EDITOR_SCREENS_

#include <string>
#include <vector>

namespace GUI
{
	class CGUIObject;
	class CDropList;

	class CEditorScreens
	{
	public:
		void Show();
		void Hide();
        
        void Init();
        void Update();

		CEditorScreens();
		~CEditorScreens();
	protected:
        CGUIObject * mEditor;
		CDropList * mEditorMode;
	};
};

#endif

