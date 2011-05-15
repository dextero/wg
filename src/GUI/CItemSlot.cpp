#include "CItemSlot.h"
#include "CRoot.h"
#include "CButton.h"
#include "CWindow.h"
#include "CInventoryDisplayer.h"
#include "../Logic/CPlayerManager.h"
#include "../Rendering/CDrawableManager.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../Logic/Items/CItem.h"
#include "../Logic/Abilities/CAbility.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Image.hpp>

using namespace GUI;

CItem* CItemSlot::mDraggedItem = NULL;
CHudSprite* CItemSlot::mDraggedIcon = NULL;
CItemSlot* CItemSlot::mDraggedSlot = NULL;

bool CItemSlot::OnMouseEvent( float x, float y, mouseEvent e )
{
    if (e == MOUSE_PRESSED_LEFT)
    {
        if (mItemIcon && mSelectedItem)
        {
            mDraggedSlot = this;
            mDraggedItem = mSelectedItem;
            mSelectedItem = NULL;
            if (mDraggedIcon)
                gDrawableManager.DestroyDrawable(mDraggedIcon);
            mDraggedIcon = mItemIcon;
            mItemIcon = NULL;

            mDraggedIcon->SetVisible(false);

            SetDraggable(false);
        }
    }

    if (e == MOUSE_DROP)
    {
        CItemSlot* slot = NULL;
        CButton* btn = NULL;

        if ((slot = dynamic_cast<CItemSlot*>(gGUI.GetLastDraggedObject())) != NULL)
        {
            if (slot != this)
            {
                slot->mSelectedItem = mSelectedItem;
                slot->mItemIcon = mItemIcon;
                slot->UpdateSprites(0.f);
                if (slot->mItemIcon && slot->mSelectedItem)
                    slot->SetDraggable(true);
            }

            mSelectedItem = mDraggedItem;
            mItemIcon = mDraggedIcon;
            if (mItemIcon)
                mItemIcon->SetVisible(true);
            UpdateSprites(0.f);
            if (mItemIcon && mSelectedItem)
                SetDraggable(true);

            mDraggedItem = NULL;
            mDraggedIcon = NULL;
            mDraggedSlot = NULL;
        }
        else
        { 
            if (mDraggedIcon)
                gDrawableManager.DestroyDrawable(mDraggedIcon);

            mDraggedItem = NULL;
            mDraggedIcon = NULL;
            mDraggedSlot = NULL;
        }

        return false;
    }

    return true;
}

void CItemSlot::UpdateSprites( float secondsPassed )
{
    if ( mItemIcon )
	{
		mItemIcon->SetVisible( mIsVisible );

		if ( mIsVisible )
		{
			if ( mItemIcon->GetSFSprite()->GetImage() ) 
			{
				sf::Vector2i imgSize( mItemIcon->GetSFSprite()->GetImage()->GetWidth(), mItemIcon->GetSFSprite()->GetImage()->GetHeight() );
				sf::Vector2f viewSize( (float) imgSize.x, (float) imgSize.y );

				mItemIcon->GetSFSprite()->SetCenter( viewSize * 0.5f );
				mItemIcon->GetSFSprite()->SetPosition( mGlobalPosition + mGlobalSize * 0.5f );
				mItemIcon->GetSFSprite()->Resize( mGlobalSize );
				mItemIcon->SetClipRect( mClippingRect );
			}
        }
	}
}

void CItemSlot::SetSelectedItem(CItem* item)
{
    mSelectedItem = item;
    
    if (mItemIcon)
        gDrawableManager.DestroyDrawable(mItemIcon);
    mItemIcon = NULL;
    SetDraggable(false);
    if (item)
    {
        mItemIcon = gDrawableManager.CreateHudSprite(mZIndex);
        mItemIcon->SetVisible(mIsVisible);
        
        CAbility* abi = gResourceManager.GetAbility(item->GetAbility());
        if (abi)
        {
            mItemIcon->GetSFSprite()->SetImage(*gResourceManager.GetImage(abi->icon));
            SetDraggable(true);
        }
    }
}

void CItemSlot::UndoDrag()
{
    if (mDraggedSlot)
        mDraggedSlot->SetSelectedItem(mDraggedItem);
}


CItemSlot::CItemSlot(const std::string &name, CGUIObject *parent, unsigned zindex)
:	CGUIObject(name, GUI_ABILITY_SLOT, parent, zindex),
    mSelectedItem(NULL),
    mItemIcon(NULL)
{
}

CItemSlot::~CItemSlot()
{
    if (mItemIcon) gDrawableManager.DestroyDrawable(mItemIcon);
}
