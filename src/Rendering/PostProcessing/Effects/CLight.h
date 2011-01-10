#ifndef __PFX_CLIGHT_H__
#define __PFX_CLIGHT_H__

#include "../CPostFX.h"
#include "../../../ResourceManager/CResourceManager.h"
#include "../../../CGame.h"
#include "../../../ResourceManager/CImage.h"

namespace pfx
{
    namespace effect
    {        
        class CLight : public CPostFX
        {
        public:
            CLight(): shd(NULL) { initShd = initRTT = false; }
            ~CLight() { if (shd) delete shd; }

            virtual void Apply(PFX_TYPE::Type type,float)
            {
                switch(type)
                {
                case PFX_TYPE::SHADERS:
                    gGame.GetRenderWindow()->Draw(*shd);
                    break;
                case PFX_TYPE::TEXTURE:
                    SetSpriteToScreen(sprite);
                    gGame.GetRenderWindow()->Draw(sprite);
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
                    initShd = true;
                    shd->LoadFromFile("data/effects/postfx/darken.sfx");
                    shd->SetTexture("framebuffer", NULL);
                    return;
                case PFX_TYPE::TEXTURE:
                   if (initRTT)
                        return;
                    initRTT = true;
                    sprite.SetImage(*gResourceManager.GetImage("data/effects/postfx/darken.png"));
                    sprite.SetBlendMode(sf::Blend::Multiply);
                case PFX_TYPE::NONE:
                    ;
                };
            }
        private:
            bool initShd, initRTT;
            sf::PostFX *shd;
            sf::Sprite sprite;
        };
    };
};

#endif //__PFX_CLIGHT_H__//
