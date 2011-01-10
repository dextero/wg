#ifndef CSaveScreen_h__
#define CSaveScreen_h__

#include "CGUIObject.h"
#include <vector>

namespace GUI
{
    class CSaveSlot;
    class CScreens;

	class CSaveScreen
    {
    public:
        CSaveScreen(CScreens * parent);
        ~CSaveScreen();

        void SetVisible(bool visible = true);

        void UpdateSlots(bool save);
        void SlotPressed(const std::wstring & params);

    private:
        void UpdateDifficultyBar(void* hoveredBtnNum);

        void UpdateSlot(int slot, const std::wstring & caption,
                        const std::string & gameName,
                        const std::string & thumbnail);
        void SelectSlot(void* slotNum);

        CGUIObject * mSaveScreen;
        std::vector< CWindow * > mSlots;
        CGUIObject * mDiffTxt, * mDiffBar, * mLoadButton;
        bool mSave;
    };

} // namespace GUI

#endif // CSaveScreen_h__
