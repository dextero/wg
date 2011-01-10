#ifndef _CEDITORRESOURCEPANEL_H_
#define _CEDITORRESOURCEPANEL_H_

/* GUI - pojedyncze panel zasobowy edytora */

#include "../CGUIObject.h"
#include <string>
#include <vector>

class CEditorResourceSet;
class IEditorResource;

namespace GUI
{
    class CWindow;
    class CButton;
    class CTextArea;
    class CScrollPanel;
    class CScrollBar;
    class CImageBox;

	class CEditorResourcePanel : public CGUIObject
	{
        friend class CGUIObject;
    protected:
        CEditorResourcePanel( const std::string& name, CGUIObject* parent, unsigned zindex );
    public:
        void Load(CEditorResourceSet *ers);

		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );
    private:
        void SelectCallback(const std::wstring &idx);

        CEditorResourceSet *mERS;

        CScrollPanel *mScrollPanel;
        CScrollBar *mScrollBar;

        struct SResourceContainer{
            SResourceContainer(IEditorResource *r, CButton *b):
                res(r), button(b){}

            IEditorResource *res;
            CButton *button;
        };
        std::vector<SResourceContainer> mResourceContainers;

        CImageBox *mHighlighter;
	};
};

#endif// _CEDITORRESOURCEPANEL_H_

