#include "CEditorResourcePanel.h"
#include "../../Editor/CEditorResourceSet.h"
#include "../../Editor/IEditorResource.h"
#include "../../Editor/CEditor.h"
#include "../CImageBox.h"
#include "../CScrollPanel.h"
#include "../CScrollBar.h"
#include "../CButton.h"
#include "../../Utils/StringUtils.h"
#include <cassert>

using namespace GUI;

CEditorResourcePanel::CEditorResourcePanel( const std::string& name, CGUIObject* parent, unsigned zindex ):
CGUIObject(name,GUI_EDITOR_RESOURCE_PANEL,parent,zindex),
mERS(NULL),
mScrollPanel(NULL),
mHighlighter(NULL){
}

void CEditorResourcePanel::Load(CEditorResourceSet *ers){
    assert(!mScrollPanel);
    mERS = ers;
    
    mScrollBar = CreateScrollBar("scroll-bar");
	mScrollBar->SetPosition(90.0f, 0.0f, 10.0f, 100.0f);
	mScrollBar->SetBackgroundImage("data/GUI/scrollbar-v.png");
	mScrollBar->SetHandleImage("data/GUI/scrollbar-handle.png");
    mScrollBar->SetOrientation( ORIENTATION_Y );
	mScrollBar->SetHandleSize(5.0f);

    mScrollPanel = CreateScrollPanel("main-panel");
    mScrollPanel->SetPosition(0.0f,0.0f,90.0f,100.0f);
    mScrollPanel->SetScrollBarY(mScrollBar);

    const std::vector<IEditorResource*> &resources = ers->GetResources();
    for (unsigned int i = 0; i < resources.size(); i++){
        CButton *btn = mScrollPanel->CreateButton("btn-" + StringUtils::ToString(i));
		btn->SetFont("data/GUI/verdana.ttf", 10.0f);
		btn->SetColor(sf::Color::Black);
        resources[i]->ConfigureButton(btn);
        btn->GetClickParamCallback()->bind(this,&CEditorResourcePanel::SelectCallback);
        btn->SetClickCallbackParams(StringUtils::ConvertToWString(StringUtils::ToString(i)));
        mResourceContainers.push_back(SResourceContainer(resources[i],btn));
    }

    mHighlighter = mScrollPanel->CreateImageBox("highlighter");
    mHighlighter->AddImageToSequence("data/GUI/editor-resource-highlight.png");
    mHighlighter->SetVisible(false);
}

bool CEditorResourcePanel::OnMouseEvent( float x, float y, mouseEvent e ){
    return false;
}

void CEditorResourcePanel::UpdateSprites( float secondsPassed ){
    float btnSize = (mGlobalSize.x * mScrollPanel->GetSize().x - 20.0f) / 3;
    for (unsigned int i = 0; i < mResourceContainers.size(); i++){
        int row = i / 3, pos = i % 3;
        mResourceContainers[i].button->SetPosition(5.0f + (7.5f + btnSize) * pos,5.0f + (7.5f + btnSize) * row,btnSize,btnSize,UNIT_PIXEL);
    }
}

void CEditorResourcePanel::SelectCallback(const std::wstring &idx){
    int i;
    StringUtils::FromString(StringUtils::ConvertToString(idx),i);
    mHighlighter->SetVisible(true);
    sf::Vector2f pos = mResourceContainers[i].button->GetPosition(UNIT_PIXEL);
    sf::Vector2f size = mResourceContainers[i].button->GetSize(UNIT_PIXEL);
    mHighlighter->SetPosition(pos.x - 3.0f, pos.y - 3.0f, size.x + 6.0f, size.y + 6.0f,UNIT_PIXEL);
    gEditor.SetSelected(mResourceContainers[i].res);
}