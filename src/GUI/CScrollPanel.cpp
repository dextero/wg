#include "CScrollPanel.h"
#include "CScrollBar.h"
#include "CRoot.h"
#include "../Utils/MathsFunc.h"
#include <queue>

using namespace GUI;

CScrollPanel::CScrollPanel(const std::string &name, GUI::CGUIObject *parent, unsigned zindex)
:	CGUIObject( name, GUI_SCROLL_PANEL, parent, zindex),
	mContentSize( 0.0f, 0.0f ),
	mChildOffset( 0.0f, 0.0f ),
	mScrollBarState( 0.0f, 0.0f ),
	mScrollBarX( NULL ),
	mScrollBarY( NULL )
{
	SetChildsClipping( true );
}

CScrollPanel::~CScrollPanel()
{
}

bool CScrollPanel::OnMouseEvent( float x, float y, mouseEvent e )
{
    CGUIObject::OnMouseEvent(x, y, e);

	if ( e == MOUSE_PRESSED_LEFT )
		gGUI.SetActiveObject(this);

	return false;
}

void CScrollPanel::UpdateSprites( float secondsPassed )
{
	UpdateContentSize();

	mChildOffset.x = 0.0f;
	mChildOffset.y = 0.0f;

	if ( mScrollBarX )	mChildOffset.x = -mScrollBarX->GetState() * ( mContentSize.x - mInternalRect.GetWidth() );
	if ( mScrollBarY )	mChildOffset.y = -mScrollBarY->GetState() * ( mContentSize.y - mInternalRect.GetHeight() );

	for ( unsigned i = 0; i < mChilds.size(); i++ )
		mChilds[i]->SetOffset( sf::Vector2f(mChildOffset.x, mChildOffset.y) );
}

/* ==== PRIVATE METHODS ======================================*/

void CScrollPanel::UpdateContentSize()
{
	bool needToUpdate = false;

	if ( mScrollBarX && mScrollBarX->GetState() != mScrollBarState.x )
	{
		needToUpdate = true;
		mScrollBarState.x = mScrollBarX->GetState();
	}

	if ( mScrollBarY && (mScrollBarY->GetState() != mScrollBarState.y) )
	{
		needToUpdate = true;
		mScrollBarState.y = mScrollBarY->GetState();
	}

	if ( !needToUpdate )
		return;

	mContentSize.x = mInternalRect.GetWidth();
	mContentSize.y = mInternalRect.GetHeight();

	std::queue<CGUIObject*> descendants;
	CGUIObject* current = this;

	while (1)
	{
		for ( unsigned i = 0; i < current->mChilds.size(); i++ )
		{
			descendants.push( current->mChilds[i] );
		}

		if ( descendants.empty() )
			break;

		current = descendants.front();
		descendants.pop();
		mContentSize.x = std::max( mContentSize.x, current->mGlobalPosition.x + current->mGlobalSize.x - mChildOffset.x - mGlobalPosition.x );
		mContentSize.y = std::max( mContentSize.y, current->mGlobalPosition.y + current->mGlobalSize.y - mChildOffset.y - mGlobalPosition.y);
	}
}

