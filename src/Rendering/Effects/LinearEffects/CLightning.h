#ifndef __CLIGHTNING_H__
#define __CLIGHTNING_H__

#include "ILinearEffect.h"
#include <list>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../../../ResourceManager/CResourceManager.h"
#include "../../../ResourceManager/CImage.h"
#include <limits>

class CLightning: public ILinearEffect
{
private:
    struct line
    {
        sf::Vector2f a, b;
		sf::Vector2f oldA, oldB;
    };
    std::list<line> linesA, linesB;
    struct vertex
    {
        float x, y, z;
    };
//    std::vector<vertex[4]> mRenderableQuads;  // rav: pod linuxem mi tu dziczeje.
    bool swapLines;
    void GenerateBolts(std::list<line> &lines);
    float timeToChange;
	float mDuration;
    unsigned int mTextureId;

	float mRed, mGreen, mBlue, mAlpha;
public:
    CLightning(sf::Vector2f from, sf::Vector2f to, float duration, float red = 1.0f, float green = 1.0f, float blue = 1.0f);
    virtual ~CLightning();

    void Animate(float secondsPassed);
    void Draw(sf::RenderWindow *renderWindow);
};

#endif

