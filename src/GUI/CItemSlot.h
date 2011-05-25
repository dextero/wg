#ifndef __CITEMSLOT_H__
#define __CITEMSLOT_H__

#include "CGUIObject.h"
#include "../Rendering/CHudSprite.h"

class CItem;

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
        
        // jezeli drop poszedl poza sloty, to dobrze byloby zapobiec usunieciu przedmiotu
        static void UndoDrag();
    private:
        CItemSlot(const std::string &name, CGUIObject *parent, unsigned zindex);
        ~CItemSlot();

        void UpdateTooltipText(const std::string& abi);

        CItem* mSelectedItem;
        CHudSprite* mItemIcon;

        static CItem* mDraggedItem;
        static CItemSlot* mDraggedSlot;
        static CHudSprite* mDraggedIcon;

        friend class CGUIObject;
    };
} // namespace GUI

#endif // __CITEMSLOT_H__