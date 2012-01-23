#ifndef __CITEMSLOT_H__
#define __CITEMSLOT_H__

#include "CGUIObject.h"
#include "../Rendering/CHudSprite.h"

class CItem;
class CAbility;
class CPlayer;

namespace GUI
{
    class CItemSlot: public CGUIObject
    {
    public:
        virtual bool OnMouseEvent( float x, float y, mouseEvent e );
        virtual void UpdateSprites( float secondsPassed );

        void SetSelectedItem(CItem* item);

        inline CHudSprite* GetIconSprite()      { return mItemIcon; }
        inline CItem* GetSelectedItem()   { return mSelectedItem; }
        inline void Clear()                     { mSelectedItem = NULL; }
        
        CHudSprite* GetDragIcon() { return mItemIcon; };

        static const int MAX_VERBOSITY = 3;
        static const int MID_VERBOSITY = 2;
        static const int LOW_VERBOSITY = 1;

        static std::wstring CreateWeaponDescription(CAbility * abi, CItem * selectedItem, CPlayer * player, int verbosity);
        
        // jezeli drop poszedl poza sloty, to dobrze byloby zapobiec usunieciu przedmiotu
        static void UndoDrag();
    private:
        CItemSlot(const std::string &name, unsigned player, CGUIObject *parent, unsigned zindex);
        ~CItemSlot();

        void UpdateTooltipText(const std::string& abi);

        CItem* mSelectedItem;
        CHudSprite* mItemIcon;

        static CItem* mDraggedItem;
        static CItemSlot* mDraggedSlot;
        static CHudSprite* mDraggedIcon;

        unsigned mPlayer;

        friend class CGUIObject;
    };
} // namespace GUI

#endif // __CITEMSLOT_H__
