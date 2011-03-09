#include "CStraightLightning.h"

#include "../../../Utils/MathsFunc.h"
#include "../../CDrawableManager.h"
#include "../../../ResourceManager/CImage.h"
#include "../../../ResourceManager/CResourceManager.h"
#include <SFML/Graphics.hpp>
#ifdef PLATFORM_MACOSX
#include <gl.h>
#else
#include <GL/gl.h>
#endif

CStraightLightning::CStraightLightning(sf::Vector2f f, sf::Vector2f t, float width, float duration, float red, float green, float blue):
mWidth(width),
mDuration(duration),
mRed(red),
mGreen(green),
mBlue(blue),
mAlpha(1.0f)
{
    from = f;
    to = t;

    gDrawableManager.RegisterDrawable(this, Z_PARTICLE);
    sf::Image *img = gResourceManager.GetImage("data/effects/straight-lightning.png");
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->GetWidth(), img->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img->GetPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

CStraightLightning::~CStraightLightning(){
    gDrawableManager.UnregisterDrawable(this);
}

void CStraightLightning::Animate(float secondsPassed)
{
	mAlpha = Maths::Clamp( mAlpha - secondsPassed/mDuration );
}

void CStraightLightning::Draw(sf::RenderWindow *renderWindow)
{
    renderWindow->PreserveOpenGLStates(true);

	glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glColor4f(mRed, mGreen, mBlue, mAlpha);

	sf::Vector2f forward = to - from;
	sf::Vector2f right = Maths::VectorRightOf( Maths::Normalize(forward) ) * mWidth / 2.0f;

	glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(1,0); glVertex3f(from.x + forward.x + right.x, from.y + forward.y + right.y, 0.0f);
        glTexCoord2f(0,0); glVertex3f(from.x + right.x, from.y + right.y, 0.0f);
        glTexCoord2f(0,1); glVertex3f(from.x - right.x, from.y - right.y, 0.0f);
        glTexCoord2f(1,1); glVertex3f(from.x + forward.x - right.x, from.y + forward.y - right.y, 0.0f);
    glEnd();

	renderWindow->PreserveOpenGLStates(false);
}

