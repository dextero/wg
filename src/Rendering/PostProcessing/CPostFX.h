#ifndef __CPOSTFX_H__
#define __CPOSTFX_H__

#include <SFML/Graphics.hpp>

namespace pfx
{
    namespace PFX_TYPE
    {
       enum Type { NONE, SHADERS, TEXTURE };
    };

    class CPostFX
    {
    public:
        virtual void Init(PFX_TYPE::Type type) = 0; 
        void ApplyEffect(PFX_TYPE::Type type, float dt) { if (display) Apply(type, dt); }

        void Enable(bool enable) const { display = enable; }
        bool IsEnabled() const { return display; }

        CPostFX() : display(false), rtt(false), shd(false) {}
        virtual ~CPostFX() {}
        //-------
    protected:
        static void SetSpriteToScreen(sf::Sprite &sprite);
        static sf::Image &GetScreen();
    private:
        virtual void Apply(PFX_TYPE::Type type, float dt) = 0;
        mutable bool display;
        bool rtt, shd;
    };
};

#endif //__CPOSTFX_H__//
