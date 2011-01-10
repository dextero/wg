#ifndef __PFX_CHOLE_H__
#define __PFX_CHOLE_H__

#include "../CPostFX.h"
#include "../../../ResourceManager/CResourceManager.h"
#include "../../../CGame.h"
#include "../../CHudSprite.h"
#include "../../../ResourceManager/CImage.h"

namespace pfx
{
    namespace effect
    {        
        class CHole : public CPostFX
        {
        public:
            explicit CHole(float offset=0.1f) : img(NULL), foo(0), off(offset)  {}
            virtual void Apply(PFX_TYPE::Type type, float dt)
            {
                foo += dt;
                switch(type)
                {
                case PFX_TYPE::SHADERS:
                case PFX_TYPE::TEXTURE:
                    if (foo > off)
                        hole.Draw(gGame.GetRenderWindow());
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
                    // to jest jeszcze potrzebne? bo nie dziala ;)
                    // ...niepotrzebne :]
                    /*img = gResourceManager.GetImage("data/postfx/hole.png");
                    hole.GetSFSprite()->SetImage(*img);
                    hole.GetSFSprite()->SetPosition(50.f,50.f);*/
                case PFX_TYPE::NONE:
                    ;
                };
            }
        private:
            sf::Image *img;
            CHudSprite hole;
            float foo, off;
        };
    };
};

#endif //__PFX_HOLE_H__//
