#include "CAbilitySlot.h"
#include "CRoot.h"
#include "CButton.h"
#include "CWindow.h"
#include "CAbilityTreeDisplayer.h"
#include "../Logic/CPlayerManager.h"
#include "../Rendering/CDrawableManager.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../Logic/Abilities/CAbility.h"
#include "../Logic/Abilities/CAbilityTree.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Image.hpp>

using namespace GUI;

CAbility* CAbilitySlot::mDraggedAbility = NULL;
CHudSprite* CAbilitySlot::mDraggedIcon = NULL;

bool CAbilitySlot::OnMouseEvent( float x, float y, mouseEvent e )
{
    if (e == MOUSE_PRESSED_LEFT)
    {
        if (mAbilityIcon && mSelectedAbility)
        {
            mDraggedAbility = mSelectedAbility;
            mSelectedAbility = NULL;
            if (mDraggedIcon)
                gDrawableManager.DestroyDrawable(mDraggedIcon);
            mDraggedIcon = mAbilityIcon;
            mAbilityIcon = NULL;

            mDraggedIcon->SetVisible(false);

            SetDraggable(false);
        }
    }

    if (e == MOUSE_DROP)
    {
        CAbilitySlot* slot = NULL;
        CButton* btn = NULL;

        if ((slot = dynamic_cast<CAbilitySlot*>(gGUI.GetLastDraggedObject())) != NULL)
        {
            if (slot != this)
            {
                slot->mSelectedAbility = mSelectedAbility;
                slot->mAbilityIcon = mAbilityIcon;
                slot->UpdateSprites(0.f);
                if (slot->mAbilityIcon && slot->mSelectedAbility)
                    slot->SetDraggable(true);
            }

            mSelectedAbility = mDraggedAbility;
            mAbilityIcon = mDraggedIcon;
            if (mAbilityIcon)
                mAbilityIcon->SetVisible(true);
            UpdateSprites(0.f);
            if (mAbilityIcon && mSelectedAbility)
                SetDraggable(true);

            mDraggedAbility = NULL;
            mDraggedIcon = NULL;
        }
        else if ((btn = dynamic_cast<CButton*>(gGUI.GetLastDraggedObject())) != NULL)
        {
            for (size_t i = 0; i < gPlayerManager.GetPlayerCount(); ++i)
            {
                CWindow* wnd = dynamic_cast<CWindow*>(gGUI.FindObject("abilities" + StringUtils::ToString(i)));
                if (!wnd)
                    continue;
                std::string treesNames[] = { "abilities-fire", "abilities-electricity", "abilities-support", "abilities-special" };
                
                for (int tree = 0; tree < 4; ++tree)
                {
                    CWindow* parent = dynamic_cast<CWindow*>(wnd->FindObject(treesNames[tree]));
                    if (!parent)
                        continue;

                    CAbilityTreeDisplayer* atd = dynamic_cast<CAbilityTreeDisplayer*>(parent->FindObject("abilities"));
                    if (!atd)
                        continue;

					//src/GUI/CAbilitySlot.cpp:85: warning: unused variable 'anm'
                    //CAbilityTreeDisplayer::CAbilityNodeMirror* anm = NULL;
                    for (std::vector<CAbilityTreeDisplayer::CAbilityNodeMirror>::iterator node = atd->mNodes.begin(); node != atd->mNodes.end(); ++node)
                        if (node->button == btn)
                        {
                            mSelectedAbility = node->abiNode->ability;
                            
                            if (mAbilityIcon)
		                        gDrawableManager.DestroyDrawable(mAbilityIcon);
                            mAbilityIcon = gDrawableManager.CloneHudSprite(node->button->GetNormalSprite(), mZIndex);
                            UpdateSprites(0.f);

                            SetDraggable(true);

                            return false;
                        }
                }
            }
        }
        else
        { 
            if (mDraggedIcon)
                gDrawableManager.DestroyDrawable(mDraggedIcon);

            mDraggedAbility = NULL;
            mDraggedIcon = NULL;
        }

        return false;
    }

    return true;
}

void CAbilitySlot::UpdateSprites( float secondsPassed )
{
    if ( mAbilityIcon )
	{
		mAbilityIcon->SetVisible( mIsVisible );

		if ( mIsVisible )
		{
			if ( mAbilityIcon->GetSFSprite()->GetImage() ) 
			{
				sf::Vector2i imgSize( mAbilityIcon->GetSFSprite()->GetImage()->GetWidth(), mAbilityIcon->GetSFSprite()->GetImage()->GetHeight() );
				sf::Vector2f viewSize( (float) imgSize.x, (float) imgSize.y );

				mAbilityIcon->GetSFSprite()->SetCenter( viewSize * 0.5f );
				mAbilityIcon->GetSFSprite()->SetPosition( mGlobalPosition + mGlobalSize * 0.5f );
				mAbilityIcon->GetSFSprite()->Resize( mGlobalSize );
				mAbilityIcon->SetClipRect( mClippingRect );
			}
        }
	}
}

void CAbilitySlot::SetSelectedAbility(CAbility* abi)
{
    mSelectedAbility = abi;
    
    if (mAbilityIcon)
        gDrawableManager.DestroyDrawable(mAbilityIcon);
    mAbilityIcon = NULL;
    SetDraggable(false);
    if (abi)
    {
        mAbilityIcon = gDrawableManager.CreateHudSprite(mZIndex);
        mAbilityIcon->SetVisible(mIsVisible);
        mAbilityIcon->GetSFSprite()->SetImage(*gResourceManager.GetImage(abi->icon));
        SetDraggable(true);
    }
}

CAbilitySlot::CAbilitySlot(const std::string &name, CGUIObject *parent, unsigned zindex)
:	CGUIObject(name, GUI_ABILITY_SLOT, parent, zindex),
    mSelectedAbility(NULL),
    mAbilityIcon(NULL)
{
}

CAbilitySlot::~CAbilitySlot()
{
    if (mAbilityIcon) gDrawableManager.DestroyDrawable(mAbilityIcon);
}
