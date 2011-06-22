#include "CGUIObject.h"
#include "CRoot.h"
#include "CWindow.h"
#include "CButton.h"
#include "CProgressBar.h"
#include "CTextArea.h"
#include "CCheckBox.h"
#include "Editor/CEditorResourcePanel.h"
#include "CScrollBar.h"
#include "CDropList.h"
#include "CImageBox.h"
#include "CMultipanel.h"
#include "CScrollPanel.h"
#include "CAbilityTreeDisplayer.h"
#include "CKeyReader.h"
#include "CAbilitySlot.h"
#include "CAnimatedImage.h"
#include "CInventoryDisplayer.h"
#include "CItemSlot.h"
#include "../Utils/Maths.h"
#include "../Rendering/ZIndexVals.h"
#include <iostream>
#include "../CGameOptions.h"
#undef CreateWindow

using namespace GUI;

/*	Funkcje publiczne
	=================================================================================== */

void CGUIObject::SetPosition(float x, float y, float width, float height, guiUnit u, bool fixedScale)
{
    float multiplier = ( (u == UNIT_PERCENT )? .01f : 1.0f );
	mPosition[u].x = x * multiplier;
	mPosition[u].y = y * multiplier;

    mSize[u].x = width * multiplier;
    mSize[u].y = height * multiplier; 
    if (true == fixedScale)
    {
        mSize[u].x *= ( static_cast<float>(gGameOptions.GetHeight()) / static_cast<float>(gGameOptions.GetWidth())  );
    }
}

void CGUIObject::SetPadding(float left, float top, float right, float bottom, guiUnit u)
{
	float multiplier = u == UNIT_PERCENT ? .01f : 1.0f ;
	mPadding[u].Left = left * multiplier;
	mPadding[u].Top = top * multiplier;
	mPadding[u].Right = right * multiplier;
	mPadding[u].Bottom = bottom * multiplier;
}

void CGUIObject::SetPadding( const STextPadding& padding )
{
    mPadding[UNIT_PERCENT].Left = padding.left * .01f;
    mPadding[UNIT_PERCENT].Top = padding.top * .01f;
    mPadding[UNIT_PERCENT].Right = padding.right * .01f;
    mPadding[UNIT_PERCENT].Bottom = padding.bottom * .01f;
}

void CGUIObject::SetVisible( bool newValue, bool recursive )
{
    mIsVisible = newValue;
    if (!newValue && mTooltip != NULL)
        mTooltip->SetVisible(false);

    if (recursive)
	for ( unsigned i = 0; i < mChilds.size(); i++ )
		mChilds[i]->SetVisible( newValue, recursive );
}

void CGUIObject::SetChildsClipping(bool clipping)
{
	mChildsClipping = clipping;
}

void CGUIObject::Remove()
{
	if ( mParent ) 
	{
		std::vector<CGUIObject*>::iterator it;
		for ( it = mParent->mChilds.begin(); it != mParent->mChilds.end(); ++it )
		{
			if ( (*it) == this ) 
			{
				mParent->mChilds.erase( it );
				break;
			}
		}
	}
	
	RemoveChilds();
	delete this;
}

CGUIObject* CGUIObject::FindObject(const std::string &name)
{
	CGUIObject* out;
	for ( unsigned i = 0; i < mChilds.size(); i++ ) 
	{
		if ( mChilds[i]->mName == name )                return mChilds[i];
        out = mChilds[i]->FindObject(name); if ( out )  return out;
	}
	return NULL;
}

sf::Vector2f CGUIObject::ConvertToGlobalPosition(sf::Vector2f input)
{
    input.x *= mGlobalSize.x ;
    input.y *= mGlobalSize.y ;
    input += mGlobalPosition;
    return input;
}

sf::Vector2f CGUIObject::ConvertFromGlobalPosition(sf::Vector2f input)
{
	if ((mGlobalSize.x <= 0.0f) || (mGlobalSize.y <= 0.0f))
		return input;

	input -= mGlobalPosition;
    input.x /= mGlobalSize.x ;
    input.y /= mGlobalSize.y ;
    return input;
}

/*	Funkcje tworzace nowe obiekty GUI i przypisujace je jako dzieci aktualnego "wezla" 
*	===================================================================================	*/

CWindow* CGUIObject::CreateWindow(const std::string& name, bool forceZ, int z)
{
	CWindow* wnd = new CWindow( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( (CGUIObject*) wnd );
	return wnd;
}

CButton* CGUIObject::CreateButton(const std::string& name, bool forceZ, int z)
{
	CButton* btn = new CButton( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( (CGUIObject*) btn );
	return btn;
}

CProgressBar* CGUIObject::CreateProgressBar(const std::string& name, bool forceZ, int z)
{
	CProgressBar* progress = new CProgressBar( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( (CGUIObject*) progress );
	return progress;
}

CTextArea* CGUIObject::CreateTextArea(const std::string& name, bool forceZ, int z)
{
    CTextArea* tarea = new CTextArea( name, this, forceZ ? z : (mZIndex - mZIndexCount));
	mChilds.push_back( (CGUIObject*) tarea );
	return tarea;
}

CCheckBox* CGUIObject::CreateCheckBox(const std::string &name, bool forceZ, int z)
{
	CCheckBox* chck = new CCheckBox( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( (CGUIObject*) chck );
	return chck;
}

CScrollBar* CGUIObject::CreateScrollBar(const std::string &name, bool forceZ, int z)
{
	CScrollBar* scroll = new CScrollBar( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( (CGUIObject*) scroll );
	return scroll;
}

CDropList* CGUIObject::CreateDropList(const std::string &name, bool forceZ, int z)
{
	CDropList* drop = new CDropList( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( (CGUIObject*) drop );
	return drop;
}

CImageBox* CGUIObject::CreateImageBox(const std::string &name, bool forceZ, int z)
{
	CImageBox* img = new CImageBox( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( (CGUIObject*) img );
	return img;
}

CMultipanel* CGUIObject::CreateMultipanel(const std::string& name, int panelCount, bool forceZ, int z){
	CMultipanel* mp = new CMultipanel( name, this, forceZ ? z : (mZIndex - mZIndexCount), panelCount );
	mChilds.push_back( (CGUIObject*) mp );
	return mp;
}

CAbilityTreeDisplayer* CGUIObject::CreateAbilityTreeDisplayer(const std::string& name, int player, const std::string &tree, bool forceZ, int z){
    CAbilityTreeDisplayer *atd = new CAbilityTreeDisplayer( name, this, forceZ ? z : (mZIndex - mZIndexCount), player, tree );
    mChilds.push_back( atd );
    return atd;
}

CScrollPanel* CGUIObject::CreateScrollPanel(const std::string& name, bool forceZ, int z){
	CScrollPanel *sp = new CScrollPanel( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
    mChilds.push_back( sp );
	return sp;
}

CEditorResourcePanel* CGUIObject::CreateEditorResourcePanel(const std::string& name, bool forceZ, int z){
	CEditorResourcePanel *ers = new CEditorResourcePanel( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
    mChilds.push_back( ers );
	return ers;
}

CKeyReader* CGUIObject::CreateKeyReader(const std::string &name, bool forceZ, int z)
{
	CKeyReader* kr = new CKeyReader( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( kr );
	return kr;
}

CAbilitySlot* CGUIObject::CreateAbilitySlot( const std::string& name, bool forceZ, int z )
{
	CAbilitySlot* as = new CAbilitySlot( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( as );
	return as;
}


CAnimatedImage* CGUIObject::CreateAnimatedImage( const std::string& name, bool forceZ, int z )
{
	CAnimatedImage* ai = new CAnimatedImage( name, this, forceZ ? z : (mZIndex - mZIndexCount) );
	mChilds.push_back( ai );
	return ai;
}

CInventoryDisplayer* CGUIObject::CreateInventoryDisplayer( const std::string& name, int player, bool forceZ, int z)
{
    CInventoryDisplayer* id = new CInventoryDisplayer(name, this, forceZ ? z : (mZIndex - mZIndexCount), player);
    mChilds.push_back( id );
    return id;
}

CItemSlot* CGUIObject::CreateItemSlot(const std::string& name, unsigned player, bool forceZ, int z)
{
    CItemSlot* is = new CItemSlot(name, player, this, forceZ ? z : (mZIndex - mZIndexCount));
    mChilds.push_back(is);
    return is;
}

/*	Funkcje chronione
	=================================================================================== */

void CGUIObject::UpdateChilds( float secondsPassed )
{
	sf::Vector2f myLeftUp( mInternalRect.Left, mInternalRect.Top );

	for ( unsigned i = 0; i < mChilds.size(); i++ ) 
	{
		CGUIObject *child = mChilds[i];

		child->mGlobalPosition.x = myLeftUp.x + child->mPosition[UNIT_PERCENT].x * mGlobalSize.x + child->mPosition[UNIT_PIXEL].x;
		child->mGlobalPosition.y = myLeftUp.y + child->mPosition[UNIT_PERCENT].y * mGlobalSize.y + child->mPosition[UNIT_PIXEL].y;

		child->mGlobalPosition += child->mOffset;

		child->mGlobalSize.x = child->mSize[UNIT_PERCENT].x * mGlobalSize.x + child->mSize[UNIT_PIXEL].x;
		child->mGlobalSize.y = child->mSize[UNIT_PERCENT].y * mGlobalSize.y + child->mSize[UNIT_PIXEL].y;

		child->mGlobalPadding.Left =	child->mPadding[UNIT_PERCENT].Left * child->mGlobalSize.x + child->mPadding[UNIT_PIXEL].Left;
		child->mGlobalPadding.Right =	child->mPadding[UNIT_PERCENT].Right * child->mGlobalSize.x + child->mPadding[UNIT_PIXEL].Right;
		child->mGlobalPadding.Top =		child->mPadding[UNIT_PERCENT].Top * child->mGlobalSize.y + child->mPadding[UNIT_PIXEL].Top;
		child->mGlobalPadding.Bottom =	child->mPadding[UNIT_PERCENT].Bottom * child->mGlobalSize.y + child->mPadding[UNIT_PIXEL].Bottom;

		child->mInternalRect.Left =		child->mGlobalPosition.x + child->mGlobalPadding.Left;
		child->mInternalRect.Top =		child->mGlobalPosition.y + child->mGlobalPadding.Top;
		child->mInternalRect.Right =	child->mGlobalPosition.x + child->mGlobalSize.x - child->mGlobalPadding.Right;
		child->mInternalRect.Bottom =	child->mGlobalPosition.y + child->mGlobalSize.y - child->mGlobalPadding.Bottom;

		if ( mChildsClipping )	child->mClippingRect = mInternalRect;
		else					child->mClippingRect = mClippingRect;

        child->UpdateSprites( secondsPassed );
		child->UpdateChilds( secondsPassed );
	}
}

void CGUIObject::RemoveChilds()
{
	for ( unsigned i = 0; i < mChilds.size(); i++ ) 
	{
		mChilds[i]->RemoveChilds();
		delete mChilds[i];
	}
}

bool CGUIObject::OnMouseEvent( float x, float y, mouseEvent e )
{
    // callbacki
    if (!mEventCallback[e].empty())
        mEventCallback[e]();
    if (!mEventWStringCallback[e].empty())
        mEventWStringCallback[e](mEventWStringParam[e]);
    if (!mEventVoidPtrCallback[e].empty())
        mEventVoidPtrCallback[e](mEventVoidPtrParam[e]);
    if (!mEventIntCallback[e].empty())
        mEventIntCallback[e](mEventIntParam[e]);

    if (mTooltip)
    {
        if (e == MOUSE_OVER)
        {
            sf::Vector2f pos = mGlobalPosition;//ConvertToGlobalPosition(GetPosition());// + ConvertToGlobalPosition(sf::Vector2f(HOLDER_SIZE, HOLDER_SIZE) / 500.f);
            // dex: screen == screen - rozmiar tooltipa, bo tak :)
            sf::Vector2f sizeGlobal = mTooltip->mGlobalSize;
            sf::Vector2f screen = gGUI.ConvertToGlobalPosition(sf::Vector2f(1.f, 1.f)) - sizeGlobal - mTooltip->GetOffset();
            pos = gGUI.ConvertFromGlobalPosition(sf::Vector2f((pos.x < screen.x ? pos.x : screen.x), (pos.y < screen.y ? pos.y : screen.y))) * 100.f;
            sf::Vector2f size = gGUI.ConvertFromGlobalPosition(sizeGlobal) * 100.f;
            mTooltip->SetPosition(pos.x, pos.y, 0.f, 0.f); // wait, what?

            mTooltip->SetVisible(true);
        }
        else if (e == MOUSE_OUT)
            mTooltip->SetVisible(false);
    }

    return false;
}

CGUIObject::CGUIObject(const std::string &name, GUI::guiType type, GUI::CGUIObject *parent, unsigned zindex )
:	mGlobalPosition	(0.0f,0.0f), 
	mGlobalSize		(0.0f,0.0f), 
	mGlobalPadding	(0.0f,0.0f,0.0f,0.0f),
	mInternalRect	(0.0f,0.0f,0.0f,0.0f),
	mOffset			(0.0f,0.0f),
    mZIndex			(zindex), 
	mZIndexCount	(0), 
	mName			(name), 
	mType			(type), 
	mParent			(parent), 
	mIsVisible		(true),
	mChildsClipping	(false),
    mTooltip        (NULL),
    mDraggable      (false)
{
	for ( unsigned i = 0; i < UNIT_COUNT; i++ )
	{
		mPosition[i] =	Maths::VectorZero();
		mSize[i] =		Maths::VectorZero();
		mPadding[i] =	sf::FloatRect( 0.0f, 0.0f, 0.0f, 0.0f );
	}

    // pucujemy parametry
    memset(mEventVoidPtrParam, 0, MOUSE_EVENT_TYPES * sizeof(void*));

	gGUI.RegisterObject( this );
}

CGUIObject::~CGUIObject()
{
	if ( mType != GUI_ROOT )
	{
		if ( gGUI.GetActiveObject() == this )	gGUI.SetActiveObject( NULL );
		if ( gGUI.GetFocusedObject() == this )	gGUI.SetFocusedObject( NULL );
		gGUI.UnregisterObject( this );
	}
}

