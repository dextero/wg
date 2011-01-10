#ifndef __CPART_SPRITE_RENDERE_H__
#define __CPART_SPRITE_RENDERE_H__

#include "../../../CGame.h"
#include "../../../Utils/CSingleton.h"
#include <SFML/Graphics.hpp>
#include "../CParticle.h"
#include <SFML/Graphics/Rect.hpp>
#include "../../../Utils/MathsFunc.h"

#define gSpriteRenderer CSpriteRenderer::GetSingleton()

namespace part
{
    namespace renderer
    {
        class CSpriteRenderer : public CSingleton<CSpriteRenderer>
        {
        public:
            CSpriteRenderer(): mTextureAtlas(NULL) {}

            bool LoadConfiguration(const std::string &file);
            /// Sprawdza czy podany indeks jest poprawny
            bool IsValid(int texture) { return (texture <= (int)mTextureOffsets.size()) && (texture > -1); }

            void SetBlendMode(sf::Blend::Mode mode) { mSprite.SetBlendMode(mode); }
            sf::Blend::Mode GetBlendMode() { return mSprite.GetBlendMode(); }

            inline void Render(CParticle *part, unsigned count)
            {
                static unsigned lastTextureID = ~0; // raczej nie mozliwe by taki byl id
                static sf::IntRect rect;
                static float w,h;              

                for (unsigned i=0; i<count; ++i)
                {
                    CParticle & p = *(part+i);

                    if (p.life <= 0.f)
                        continue;

                    // aby za czesto nie zmieniac textureID:
                    if (lastTextureID != p.textureID)
                    {
                        rect = mTextureOffsets[p.textureID];
                        mSprite.SetSubRect(rect);

                        w = static_cast<float>(rect.Right - rect.Left);
                        h = static_cast<float>(rect.Bottom - rect.Top);
                        
                        mSprite.SetCenter(w * 0.5f, h * 0.5f);

                        lastTextureID = p.textureID;
                    }

                    mSprite.Resize(p.size*w, p.size*h);
                    mSprite.SetRotation(p.rotation);
                    mSprite.SetColor(p.color);       
                    mSprite.SetPosition(p.pos.x,p.pos.y);                    

                    gGame.GetRenderWindow()->Draw(mSprite);
                }
            }

        private:
            sf::Sprite mSprite;
            sf::Image *mTextureAtlas;
            std::vector< sf::IntRect > mTextureOffsets;            
        };
    };
};
#endif //__CPART_SPRITE_RENDERE_H__//

