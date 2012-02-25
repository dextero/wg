#include "CItemSlot.h"
#include "CRoot.h"
#include "CButton.h"
#include "CWindow.h"
#include "CInventoryDisplayer.h"
#include "CTextArea.h"
#include "Localization/CLocalizator.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/CPlayer.h"
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
	CGUIObject::OnMouseEvent(x, y, e);

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

            // usuniecie itema ze slota - update'ujemy tooltipa
            UpdateTooltipText("");
        }
    }

    if (e == MOUSE_DROP)
    {
        CItemSlot* slot = NULL;

        if ((slot = dynamic_cast<CItemSlot*>(gGUI.GetLastDraggedObject())) != NULL)
        {
            if (slot != this)
            {
                slot->mSelectedItem = mSelectedItem;
                slot->mItemIcon = mItemIcon;
                slot->UpdateSprites(0.f);
                if (slot->mItemIcon && slot->mSelectedItem)
                    slot->SetDraggable(true);

                slot->UpdateTooltipText(mSelectedItem ? mSelectedItem->GetAbility() : "");
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
            
            // zmiana umiejki w slocie! w takim razie update'ujemy tooltipa
            UpdateTooltipText(mSelectedItem ? mSelectedItem->GetAbility() : "");
        }
        else
        { 
            if (mDraggedIcon)
                gDrawableManager.DestroyDrawable(mDraggedIcon);

            mDraggedItem = NULL;
            mDraggedIcon = NULL;
            mDraggedSlot = NULL;
        }

        return true;
    }

    return false;
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
            System::Resource::CImage* img = gResourceManager.GetImage(abi->icon);
            if (img)
                mItemIcon->GetSFSprite()->SetImage(*img);
            else
            {
                fprintf(stderr, "Error: couldn't open image: %s\n", abi->icon.c_str());
                img = gResourceManager.GetImage("data/abilities/default.png");
                if (img)
                    mItemIcon->GetSFSprite()->SetImage(*img);
                else
                    fprintf(stderr, "Error: couldn't open ugly default ability icon (data/abilities/default.png). Prepare for crash.\n");
            }
            // nic nie bedzie widac przy przeciaganiu, jesli sie zepsuje obrazek, ale ok...
            SetDraggable(true);
			
            UpdateTooltipText(item->GetAbility());
        }
    }
	else
		UpdateTooltipText("");
}

void CItemSlot::UndoDrag()
{
    if (mDraggedSlot)
        mDraggedSlot->SetSelectedItem(mDraggedItem);
}


CItemSlot::CItemSlot(const std::string &name, unsigned player, CGUIObject *parent, unsigned zindex)
:	CGUIObject(name, GUI_ITEM_SLOT, parent, zindex),
    mSelectedItem(NULL),
    mItemIcon(NULL),
    mPlayer(player)
{
	CWindow* tooltip = gGUI.CreateWindow("slot" + StringUtils::ToString(this) + "_tooltip", true, Z_GUI4);
    tooltip->SetBackgroundImage("data/GUI/transparent-black.png");
    tooltip->SetVisible(false);
    tooltip->SetOffset(GetParent()->ConvertToGlobalPosition(GetSize()));

    // opis
    CTextArea* description = tooltip->CreateTextArea("desc");
    description->SetVisible(false);
    //description->SetCenter(true); // ³eeee³oooo³eeee³oooo bug alert, nie centrowac
    description->SetAutoHeight(true);
    description->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));

    // nie chcemy, zeby tooltip odbieral komunikaty myszy
    gGUI.UnregisterObject(tooltip);
    gGUI.UnregisterObject(description);
    
    SetTooltip(tooltip);
}

CItemSlot::~CItemSlot()
{
    if (mItemIcon) gDrawableManager.DestroyDrawable(mItemIcon);
}

void CItemSlot::UpdateTooltipText(const std::string& abiName)
{
    CAbility* abi = NULL;
    if (abiName.size())
        abi = gResourceManager.GetAbility(abiName);

    CPlayer * player = gPlayerManager.GetPlayerByNumber(mPlayer);

    std::wstring tooltipText = CreateWeaponDescription(abi, mSelectedItem, player, MAX_VERBOSITY);

    if (GetTooltip())
    {
        CTextArea* description = dynamic_cast<CTextArea*>(GetTooltip()->FindObject("desc"));
        if (description)
        {
            description->SetText(tooltipText);
            description->SetPosition(20.f, 20.f, 360.f, 0.f, UNIT_PIXEL);

            GetTooltip()->UpdateChilds(0.f);
            description->UpdateText();
            GetTooltip()->SetPosition(0.f, 0.f, 400.f, description->GetSize(UNIT_PIXEL).y + 30.f, UNIT_PIXEL);
        }
    }
}

std::wstring CItemSlot::CreateWeaponDescription(CAbility * abi, CItem * selectedItem, CPlayer * player, int verbosity)
{
    // dokladnosc wyswietlanych floatow - ile miejsc po przecinku
    unsigned PRECISION = 4;

    std::wstring tooltipText;
    if (abi)
    {
        tooltipText = abi->name + L"\n";

        //if (selectedItem != NULL) tooltipText += L" (" + gLocalizator.GetText("ITEM_LEVEL") + L" " + StringUtils::ToWString(selectedItem->mLevel) + L")";
        if (abi->isFocus && verbosity >= MAX_VERBOSITY) {
            tooltipText += L"\n" + CTextArea::GetNextColorString(sf::Color(231, 216, 46)) //Gold
                    + gLocalizator.GetText("ABILITY_IS_FOCUS_ABILITY")
                    + CTextArea::GetNextColorString(sf::Color::White) + L"\n";
        }
        if (verbosity >= MID_VERBOSITY) {
            tooltipText += abi->description;
        } 
        tooltipText += L"\n";

        if (verbosity >= LOW_VERBOSITY) {
            if (player)
                tooltipText += StringUtils::ConvertToWString(abi->GetEffectDescription(player)) + L"\n"
                        + CTextArea::GetNextColorString(sf::Color(100, 100, 255))
                        + L"Mana cost: " + StringUtils::ConvertToWString(abi->GetManaCostString(player)) + L"\n"
                        + CTextArea::GetNextColorString(sf::Color::White);
        }

        if (verbosity >= MID_VERBOSITY) {
            tooltipText += L"\n" + abi->runesDescription + L"\n";
        }

        if (!player)
            tooltipText += L"\nError, player not set :(";
        else
        {
            float currPower = player->GetAbilityPower(abi);
            float bestPower = currPower;
        
            std::vector<CAbility*> abis = player->GetExportedAbilities();
            std::vector<float> powerAfterUpgrade;

            tooltipText += gLocalizator.GetText("CURRENT_ITEM_POWER") 
                    + StringUtils::FloatToWString(currPower, PRECISION) + L"\n";

            if (verbosity >= MAX_VERBOSITY) {
                tooltipText += L"\n" + gLocalizator.GetText("ITEM_POWER_AFTER_UPGRADE") + L"\n";
            
                for (size_t i = 0 ; i < abis.size(); ++i)
                {
                    float upgradedPower = player->GetAbilityPowerAtNextLevelOfOther(abi, abis[i]);
                    powerAfterUpgrade.push_back(upgradedPower);
                    if (bestPower < upgradedPower)
                        bestPower = upgradedPower;
                }

                for (size_t i = 0; i < abis.size(); ++i)
                {
                    tooltipText += abis[i]->name + L": ";
                    if (powerAfterUpgrade[i] == bestPower)
                        tooltipText += CTextArea::GetNextColorString(sf::Color::Green)
                                + StringUtils::FloatToWString(powerAfterUpgrade[i], PRECISION)
                                + CTextArea::GetNextColorString(sf::Color::White);
                    else if (powerAfterUpgrade[i] < currPower)
                        tooltipText += CTextArea::GetNextColorString(sf::Color::Red)
                                + StringUtils::FloatToWString(powerAfterUpgrade[i], PRECISION)
                                + CTextArea::GetNextColorString(sf::Color::White);
                    else
                        tooltipText += StringUtils::FloatToWString(powerAfterUpgrade[i], PRECISION);

                    tooltipText += L"\n";
                }
            }
        }

        if (verbosity >= MAX_VERBOSITY) {
            tooltipText += L"\n" + gLocalizator.GetText("SLOT_DRAG_ITEM_HERE");
        }
    }
    else
        tooltipText = gLocalizator.GetText("SLOT_EMPTY");

    return tooltipText;

}
