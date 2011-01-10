#include "CLightning.h"

#include "../../../Utils/MathsFunc.h"
#include "../../../Utils/CRand.h"

#include "../../CDrawableManager.h"

#include <SFML/Graphics.hpp>
#ifdef PLATFORM_MACOSX
#include <gl.h>
#else
#include <GL/gl.h>
#endif
#include "../../../ResourceManager/CImage.h"

CLightning::CLightning(sf::Vector2f f, sf::Vector2f t, float duration, float red, float green, float blue): 
swapLines(false), 
timeToChange(0.05f),
mDuration(duration),
mRed(red),
mGreen(green),
mBlue(blue),
mAlpha(1.0f)
{
    from = f;
    to = t;

    GenerateBolts(linesA);
    GenerateBolts(linesB);

    gDrawableManager.RegisterDrawable(this, Z_PARTICLE);

    sf::Image *img = gResourceManager.GetImage("data/effects/lightning.png");
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->GetWidth(), img->GetHeight(), 
        0, GL_RGBA, GL_UNSIGNED_BYTE, img->GetPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

CLightning::~CLightning(){
    gDrawableManager.UnregisterDrawable(this);
}

void CLightning::GenerateBolts(std::list<line> &lines)
{
    lines.clear();

    const float distance = Maths::Length(from-to);
	float offsetAmount = Maths::Clamp(abs( distance * 0.2f ), 32.f, 256.f);
	int numOfCuits = Maths::BinaryLog((int)(distance/5));  // im wieksza liczba, tym mniej zalaman
	Maths::Clamp((float)numOfCuits, 3, 6);

    line tmp;
    tmp.a = from; tmp.b = to;
    lines.push_back(tmp);

    std::list<line> toAdd;

	// modyfikator dlugosci rozgalezien
	float mod = 0.85f;

    while(numOfCuits--)
    {
        for(std::list<line>::iterator it=lines.begin(); it!=lines.end(); it++)
        {
            line &k = *it;
            sf::Vector2f mid = (k.a+k.b)/2.0f;
            sf::Vector2f fromTo = k.a-k.b;
            fromTo = sf::Vector2f(fromTo.y, -fromTo.x);
            mid += Maths::Normalize(fromTo) * gRand.Rndf(-offsetAmount, offsetAmount);

            tmp.a = k.a;
            tmp.b = mid;
            toAdd.push_back(tmp);
            tmp.a = k.b;
            toAdd.push_back(tmp);

            if(gRand.Rnd(0, 3) == 1)
            {
				// 30.05, rAum - nieco zmodyfikowalem aby rozgalezienia
				// nie byly za czesto pod katem ~90* i wygladaly przez to lepiej ;]
                // wydluzenie
                sf::Vector2f v= mid-k.a;
                int rot = gRand.Rnd(2, 25);
                v = Maths::Rotate(v, rot);
                v *= mod;
                tmp.a = tmp.b + v;
                toAdd.push_back(tmp);
            }
        }

		mod *= 0.8f;

        lines = toAdd;
        toAdd.clear();
        offsetAmount *= 0.5f;
    }
}

void CLightning::Animate(float secondsPassed)
{
	mAlpha = Maths::Clamp( mAlpha - secondsPassed/mDuration );

	/* damork, chyba ³adniej wygl¹da jak tak nie miga, tylko powoli zanika...
	timeToChange -= secondsPassed;
	if(timeToChange <= 0.0f)
    {
        timeToChange = 0.1f;
        swapLines = !swapLines;
        if(swapLines)
            GenerateBolts(linesB);
        else
            GenerateBolts(linesA);
    }
	*/
}

void CLightning::Draw(sf::RenderWindow *renderWindow)
{
    renderWindow->PreserveOpenGLStates(true);

    //sf::Shape oneLine;
    
    const float size = 5;

    for(std::list<line>::iterator it=linesA.begin(); it!=linesA.end(); it++)
    {
        line &k = *it;
        //unsigned char blend = (swapLines? ((unsigned char)(timeToChange*5*255)) : 255);
        //oneLine = sf::Shape::Line(k.a.x,k.a.y,k.b.x,k.b.y,1.0f, sf::Color(255,255,255,blend),2.0f, sf::Color(255,255,255,blend));
        //renderWindow->Draw(oneLine);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        glColor4f(mRed, mGreen, mBlue, mAlpha);
        // bardzoooo testowe, nie widzicie glVertex2f wbrew pozorom

        sf::Vector2f a,b,c,d;
        sf::Vector2f perp = Maths::VectorRightOf(Maths::Normalize(k.a-k.b));

        sf::Vector2f offset(Maths::Normalize((k.a-k.b))*(size/2));
        perp*=size;
        a = k.a+perp;
        b = k.a-perp;
        c = k.b-perp;
        d = k.b+perp;

        glBegin(GL_QUADS);
            glTexCoord2f(1,1); glVertex3f(a.x, a.y,0);
            glTexCoord2f(1,0); glVertex3f(b.x, b.y,0);
            glTexCoord2f(0,0); glVertex3f(c.x, c.y,0);
            glTexCoord2f(0,1); glVertex3f(d.x, d.y,0);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINES);
        glVertex2f(k.a.x,k.a.y);
        glVertex2f(k.b.x,k.b.y);
        glEnd();
    }
    for(std::list<line>::iterator it=linesB.begin(); it!=linesB.end(); it++)
    {
        line &k = *it;
         //unsigned char blend = (!swapLines? ((unsigned char)(timeToChange*5*255)) : 255);
         //oneLine = sf::Shape::Line(k.a.x,k.a.y,k.b.x,k.b.y,1.0f, sf::Color(255,255,255,blend),2.0f,sf::Color(255,255,255,blend));
        //renderWindow->Draw(oneLine);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        glColor4f(mRed, mGreen, mBlue, !swapLines?1.0f:timeToChange*5);
        // bardzoooo testowe, nie widzicie glVertex2f wbrew pozorom

        sf::Vector2f a,b,c,d;
        sf::Vector2f perp = Maths::VectorRightOf(Maths::Normalize(k.a-k.b));

        sf::Vector2f offset(Maths::Normalize((k.a-k.b))*(size/2));
        perp*=size;
        a = k.a+perp;
        b = k.a-perp;
        c = k.b-perp;
        d = k.b+perp;

        glBegin(GL_QUADS);
            glTexCoord2f(1,1); glVertex3f(a.x, a.y,0);
            glTexCoord2f(1,0); glVertex3f(b.x, b.y,0);
            glTexCoord2f(0,0); glVertex3f(c.x, c.y,0);
            glTexCoord2f(0,1); glVertex3f(d.x, d.y,0);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINES);
        glVertex2f(k.a.x,k.a.y);
        glVertex2f(k.b.x,k.b.y);
        glEnd();
    }

    renderWindow->PreserveOpenGLStates(false);
}

