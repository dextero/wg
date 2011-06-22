#ifndef __PFX_CMOTIONBLUR_H__
#define __PFX_CMOTIONBLUR_H__

#include "../CPostFX.h"
#include "../../../ResourceManager/CResourceManager.h"
#include "../../../CGame.h"
#include "../../../Console/CConsole.h"
namespace pfx
{
    namespace effect
    {        
        class CMotionBlur : public CPostFX
        {
        public:
            explicit CMotionBlur(float dt=0.05f, sf::Color overlayColor = sf::Color(255,255,255,100)) : c(overlayColor), state(false), delta(dt), acc(0.f){ }

            virtual void Apply(PFX_TYPE::Type type, float dt)
            {
                switch(type)
                {
                case PFX_TYPE::SHADERS:
                case PFX_TYPE::TEXTURE:

                    if (state)
                    {
                        SetSpriteToScreen(last);
                        gGame.GetRenderWindow()->Draw(last);

                        acc += dt;
                        if (acc > delta)
                        {
                            last.SetImage(GetScreen());
                            acc -= delta;
                        }
                    }
                    else
                    {
                        last.SetImage(GetScreen());
                        state = true;
                    }
                case PFX_TYPE::NONE:
                    ;
                };
            }
            virtual void Init(PFX_TYPE::Type type)
            {
                switch(type)
                {
                case PFX_TYPE::SHADERS:
                case PFX_TYPE::TEXTURE:
                    last.SetColor(c);
                case PFX_TYPE::NONE:
                    ;
                };
            }
        private:
            sf::Color c;
            sf::Sprite last;
            bool state;
            float delta, acc;
        };
    };
};

#endif //__PFX_CMOTIONBLUR_H__//
