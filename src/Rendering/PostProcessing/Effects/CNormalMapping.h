#ifndef __PFX_NORMALMAPPING_H__
#define __PFX_NORMALMAPPING_H__

#include "../CPostFX.h"
#include "../../../CGame.h"

namespace pfx
{        
    namespace effect
    {
        // Efekt shader-only
        class CNormalMapping : public CPostFX
        {
        public:
            explicit CNormalMapping(sf::Color color=sf::Color(255,255,255)) : shd(0), ligthColor(color), initShd(false) {}
            ~CNormalMapping() { if(shd) delete shd; }

            void SetLightColor(sf::Color color)
            {
                if (shd){
                    ligthColor = color;
                    shd->SetParameter("lcolor", ligthColor.r/255.f, ligthColor.g/255.f, ligthColor.b/255.f, 1.f);
                }
            }
            sf::Color GetLightColor() { return ligthColor; }

            void SetLightPosition(sf::Vector3f pos)
            {
                if (shd) {
                ligthPos = pos;
                shd->SetParameter("lpos",ligthPos.x,ligthPos.y,5.1f);
                }
            }
            sf::Vector3f GetLightPosition() { return ligthPos; }
            
            virtual void Apply(PFX_TYPE::Type type, float dt)
            {
                static float a=0.f;
                a += 1.f * dt;

                switch(type)
                {
                case PFX_TYPE::SHADERS:
                    {                        
                    sf::RenderWindow *w = gGame.GetRenderWindow();
                    shd->SetParameter("lpos", sin(a), cos(a), 5.1f);
                    shd->SetParameter("tSize", (float)w->GetWidth(), (float)w->GetHeight());
                    
                    w->Draw(*shd);
                    }
                    break;
                default:
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
                    shd->LoadFromFile("data/effects/postfx/normalmapping.sfx");
                    shd->SetTexture("image", NULL);
                    SetLightColor(ligthColor);
                    SetLightPosition(ligthPos);
                case PFX_TYPE::TEXTURE:
                case PFX_TYPE::NONE:
                    ;
                };
            }
        private:
            sf::PostFX *shd;
            sf::Color ligthColor;
            sf::Vector3f ligthPos;
            bool initShd;
            //-------------
        };
    };
};

#endif //__PFX_NORMALMAPPING_H__//
