#ifndef __CABILITYSLOT_H__
#define __CABILITYSLOT_H__

#include "CGUIObject.h"
#include "../Rendering/CHudSprite.h"

class CAbility;

namespace GUI
{
    class CAbilitySlot: public CGUIObject
    {
    public:
        virtual bool OnMouseEvent( float x, float y, mouseEvent e );
        virtual void UpdateSprites( float secondsPassed );

        void SetSelectedAbility(CAbility* abi);

        inline CHudSprite* GetIconSprite()      { return mAbilityIcon; }
        inline CAbility* GetSelectedAbility()   { return mSelectedAbility; }
        inline void Clear()                     { mSelectedAbility = NULL; }
        
        CHudSprite* GetDragIcon() { return mAbilityIcon; };
    private:
        CAbilitySlot(const std::string &name, CGUIObject *parent, unsigned zindex);
        ~CAbilitySlot();

        CAbility* mSelectedAbility;
        CHudSprite* mAbilityIcon;

        static CAbility* mDraggedAbility;
        static CHudSprite* mDraggedIcon;

        friend class CGUIObject;
    };
} // namespace GUI

#endif // __CABILITYSLOT_H__