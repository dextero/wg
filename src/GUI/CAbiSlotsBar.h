#ifndef __CABISLOTSBAR_H__
#define __CABISLOTSBAR_H__

#include <string>
#include "../Input/CPlayerController.h"

class CAbility;

namespace GUI
{
    class CWindow;
    class CAbilitySlot;
    class CTextArea;
    
    class CAbiSlotsBar
    {
    public:
        CAbiSlotsBar();
        ~CAbiSlotsBar();

        void Init(unsigned playerNumber);
        void UpdatePlayerData();
        void Show();
        void Hide();
        void UpdatePosition();
        void UpdateKeyStrings();

        CAbility* GetSelectedAbility(unsigned num);

        void SetBarBackground(const std::string& bg);
        void SetSlotBackground(const std::string& bg);

        void SetSlotAbility(unsigned int slot, CAbility* abi);
    private:
        unsigned mPlayerNumber;

        CWindow*        mBar;
        // ABI_SLOTS_COUNT zdefiniowane w CPlayerController.h
        CWindow*        mSlotBg[ABI_SLOTS_COUNT];
        CTextArea*      mBindKey[ABI_SLOTS_COUNT];
        CAbilitySlot*   mSlot[ABI_SLOTS_COUNT];
    };
} // namespace GUI

#endif // __CABISLOTSBAR_H__