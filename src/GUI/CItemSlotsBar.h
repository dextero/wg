#ifndef __CABISLOTSBAR_H__
#define __CABISLOTSBAR_H__

#include <string>
#include "../Input/CPlayerController.h"
#include "../Logic/Items/CItem.h"

#include <SFML/System/Vector2.hpp>

class CItem;

namespace GUI
{
    class CWindow;
    class CItemSlot;
    class CTextArea;
    class CGUIObject;
    
    class CItemSlotsBar
    {
    public:
        static const unsigned int ITEM_SLOTS_COUNT = 4;
        
        CItemSlotsBar();
        ~CItemSlotsBar();

        void Init(CGUIObject* parent, unsigned playerNumber);
        void UpdatePlayerData();
        void Show();
        void Hide();
        void UpdatePosition();
        void UpdateKeyStrings();

        CItem* GetSelectedItem(unsigned num);

        void SetBarBackground(const std::string& bg);
        void SetSlotBackground(const std::string& bg);

        void SetSlotItem(unsigned int slot, CItem* item, bool updatePlayer = true);
        
        void SetBarPosPercent(sf::Vector2f pos) { mBarPosPercent = pos; }
        void SetBarSizePix(sf::Vector2f size) { mBarSizePix = size; }
        void SetSlotPosPercent(unsigned int slot, sf::Vector2f pos) { if (slot < ITEM_SLOTS_COUNT) mSlotPosPercent[slot] = pos; }
        void SetSlotSizePercent(sf::Vector2f size) { mSlotSizePercent = size; }
    private:
        unsigned mPlayerNumber;

        CWindow*        mBar;
        CWindow*        mSlotBg[ITEM_SLOTS_COUNT];
        CTextArea*      mBindKey[ITEM_SLOTS_COUNT];
        CItemSlot*      mSlot[ITEM_SLOTS_COUNT];
        
        sf::Vector2f    mBarPosPercent, mBarSizePix;
        sf::Vector2f    mSlotPosPercent[ITEM_SLOTS_COUNT], mSlotSizePercent;
    };
} // namespace GUI

#endif // __CABISLOTSBAR_H__