/* pseudo- jednosegmentowa- bezrozgalezieniowa "blyskawica" */

#include "ILinearEffect.h"

#ifndef STRAIGHT_LIGHTNING_H
#define STRAIGHT_LIGHTNING_H

class CStraightLightning: public ILinearEffect
{
private:
    unsigned int mTextureId;
	float mWidth;
	float mDuration;
	float mRed, mGreen, mBlue, mAlpha;

public:
    CStraightLightning(
		sf::Vector2f from, sf::Vector2f to, 
		float width, float duration, 
		float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f);
    virtual ~CStraightLightning();

    void Animate(float secondsPassed);
    void Draw(sf::RenderWindow *renderWindow);
};

#endif