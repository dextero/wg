#ifndef __CANIMATEDIMAGE_H__
#define __CANIMATEDIMAGE_H__

#include "CGUIObject.h"
#include "../Rendering/CDisplayable.h"

namespace GUI
{
    class CAnimatedImage: public CGUIObject
    {
    private:
        CDisplayable*   mDisplayable;

    public:
        CAnimatedImage(const std::string& name, CGUIObject* parent, unsigned zindex);
        ~CAnimatedImage();

        inline CDisplayable* GetDisplayable() { return mDisplayable; }

        void SetVisible(bool visible, bool recursive = true);

        void SetAnimation(const std::string& name);
        void SetAnimation(SAnimation* anim);

        void UpdateSprites(float secondsPassed);
    };
}

#endif // __CANIMATEDIMAGE_H__