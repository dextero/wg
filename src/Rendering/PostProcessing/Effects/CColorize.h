#ifndef __PFX_CCOLORIZE_H__
#define __PFX_CCOLORIZE_H__

#include "../CPostFX.h"
#include "../../../CGame.h"

namespace pfx
{        
    namespace effect
    {
        class CColorize : public CPostFX
        {
        public:
            explicit CColorize(sf::Color color=sf::Color(200,40,40)) : shd(0), c(color), initShd(false) {}
            ~CColorize() { if(shd) delete shd; }

            void SetColor(sf::Color color) { c = color; }
            sf::Color GetColor() { return c; }
            
            virtual void Apply(PFX_TYPE::Type type, float dt)
            {
                switch(type)
                {
                case PFX_TYPE::SHADERS:
                    shd->SetParameter("color", c.r/255.f, c.g/255.f, c.b/255.f);
                    gGame.GetRenderWindow()->Draw(*shd);
                    break;
                case PFX_TYPE::TEXTURE:
                    rtt.SetImage(GetScreen());
                    SetSpriteToScreen(rtt);
                    rtt.SetColor(c);
                    gGame.GetRenderWindow()->Draw(rtt);
                case PFX_TYPE::NONE:
                    ;
                };
            }
            virtual void Init(PFX_TYPE::Type type)
            {
                switch(type)
                {
                case PFX_TYPE::SHADERS:
                    if (initShd)
                        return;
                    shd = new sf::PostFX();
                    assert(shd != NULL);
                    shd->LoadFromFile("data/effects/postfx/colorize.sfx");
                    shd->SetTexture("framebuffer", NULL);
                case PFX_TYPE::TEXTURE:
                case PFX_TYPE::NONE:
                    ;
                };
            }
        private:
            sf::PostFX *shd;
            sf::Sprite rtt;
            sf::Color c;
            bool initShd;
        };
    };
};

#endif //__PFX_CCOLORIZE_H__//
