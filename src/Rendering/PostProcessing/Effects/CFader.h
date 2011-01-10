#ifndef __PFX_CFADER_H__
#define __PFX_CFADER_H__

#include "../CPostFX.h"
#include "../../../CGame.h"
#include <cmath>

namespace pfx
{        
    namespace effect
    {
        namespace PFX_FADE
        {
            enum Fade { FADE_NONE, FADE_IN, FADE_OUT };
        };

        class CFader : public CPostFX
        {
        public:
            explicit CFader(float fadeTime=2.f, sf::Color color = sf::Color(0,0,0,255) ) : fdt(fadeTime), acc(0), c(color), mode(PFX_FADE::FADE_NONE) {}
            
            PFX_FADE::Fade GetFade() { return mode; }
            void SetFade(PFX_FADE::Fade fadeType)
            {
                if (mode == fadeType)
                    return;
                mode = fadeType;
                acc = 0.f;
                if (mode == PFX_FADE::FADE_IN)
                    c.a = 255;
                else if (mode == PFX_FADE::FADE_OUT)
                    c.a = 0;
            }

            void SetFadeColor(sf::Color fader) { c = fader; }

            virtual void Apply(PFX_TYPE::Type type, float dt)
            {
                switch(type)
                {
                case PFX_TYPE::SHADERS:
                case PFX_TYPE::TEXTURE:
                    acc += dt;
                    switch(mode)
                    {
                    case PFX_FADE::FADE_OUT:
                        if (acc >= fdt)
                        {
                            c.a = 255;
                            mode = PFX_FADE::FADE_NONE;
                        }
                        else
                            Increase(&c.a);
                        break;
                    case PFX_FADE::FADE_IN:
                        if (acc >= fdt)
                        {
                            c.a = 0;
                            mode = PFX_FADE::FADE_NONE;
                        }
                        else 
                            Decrease(&c.a);
                    default:
                        break;
                    };

                    SetSpriteToScreen(f);
                    f.SetColor(c);
                    gGame.GetRenderWindow()->Draw(f);
                    break;
                default:
                    break;
                };
            }
            virtual void Init(PFX_TYPE::Type type)
            {
                switch(type)
                {
                case PFX_TYPE::SHADERS:
                case PFX_TYPE::TEXTURE:
                    acc = 0.f;
                    c.a = 0;
                    img.Create(1,1,sf::Color(255,255,255));
                    mode = PFX_FADE::FADE_NONE;
                default:
                    break;
                };
            }
        private:
            void Increase(unsigned char *d)
            {
                float a = static_cast<float>(*d);
                a = acc/fdt * 255.f;
                if (a < 0.f)
                    a = 0.f;
                if (a > 254.f)
                    a = 255.f;
                *d = static_cast<unsigned char>(floor(a));
            }
            void Decrease(unsigned char *i)
            {
                float a = static_cast<float>(*i);
                a = 255.f - (acc/fdt * 255.f);
                if (a < 0.f)
                    a = 0.f;
                if (a > 254.f)
                    a = 255.f;
                *i = static_cast<unsigned char>(ceil(a));
            }
            float fdt, acc;
            sf::Color c;
            sf::Image img;
            sf::Sprite f;
            PFX_FADE::Fade mode;
        };
    };
};

#endif //__PFX_CFADER_H__//
