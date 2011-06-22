#include "CPrimitivesDrawer.h"

#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../../CGame.h"
#include "../CDrawableManager.h"
#include "../ZIndexVals.h"
#include "CPrimitiveClass.h"

template<> CPrimitivesDrawer* CSingleton<CPrimitivesDrawer>::msSingleton = 0;

struct CPrimitivesDrawer::CPrimitive{
    CPrimitive(): shape(NULL){}
    ~CPrimitive(){ if (shape != NULL) delete shape; }

    sf::Shape *shape;
    float time;
};

CPrimitivesDrawer::CPrimitivesDrawer(){
    fprintf(stderr,"CPrimitivesDrawer::CPrimitivesDrawer()\n");
    gDrawableManager.RegisterDrawable(this,Z_PRIMITIVES);
    gGame.AddFrameListener(this);

    mStandardClass = GetClass("");
    mStandardClass->borderColor = sf::Color::Red;
    mStandardClass->defaultTime = 0.5f;

    CPrimitiveClass *pc = GetClass("explosion");
    pc->borderColor = sf::Color::Blue;
    pc->defaultTime = 0.5f;

    pc = GetClass("region");
    pc->borderColor = sf::Color::Green;
    pc->defaultTime = 0.5f;
    
    pc = GetClass("region-hl");
    pc->borderColor = sf::Color::Red;
    pc->defaultTime = 0.5f;

	pc = GetClass("red-line");
	pc->borderColor = sf::Color::Red;

	pc = GetClass("blue-line");
	pc->borderColor = sf::Color::Blue;
}

CPrimitivesDrawer::~CPrimitivesDrawer(){
    fprintf(stderr,"CPrimitivesDrawer::~CPrimitivesDrawer()\n");
}


void CPrimitivesDrawer::DrawLine(const sf::Vector2f &from, const sf::Vector2f &to, float time){
    DrawLine(mStandardClass,from,to,time);
}

void CPrimitivesDrawer::DrawCircle(const sf::Vector2f &at, float r, float time){
    DrawCircle(mStandardClass,at,r,time);
}

void CPrimitivesDrawer::DrawRect(const sf::Vector2f &topleft, const sf::Vector2f &bottomright, float time){
    DrawRect(mStandardClass,topleft,bottomright,time);
}

void CPrimitivesDrawer::DrawLine(const CPrimitiveClass *pc, const sf::Vector2f &from, const sf::Vector2f &to, float time){
    CPrimitive *p = new CPrimitive();
    p->shape = new sf::Shape(sf::Shape::Line(from.x*64.0f,from.y*64.0f,to.x*64.0f,to.y*64.0f,1.0f,pc->borderColor,1.0f));
    p->shape->EnableFill(false);
    p->shape->EnableOutline(true);
    if (time >= 0.0f)
        p->time = time;
    else 
        p->time = pc->defaultTime;
    mShapes.push_back(p);
}

void CPrimitivesDrawer::DrawCircle(const CPrimitiveClass *pc, const sf::Vector2f &at, float r, float time){
    CPrimitive *p = new CPrimitive();
    p->shape = new sf::Shape (sf::Shape::Circle(at.x*64.0f,at.y*64.0f,r*64.0f,pc->borderColor,1.0f,pc->borderColor));
    p->shape->EnableFill(false);
    p->shape->EnableOutline(true);
    if (time >= 0.0f)
        p->time = time;
    else 
        p->time = pc->defaultTime;
    mShapes.push_back(p);
}

void CPrimitivesDrawer::DrawRect(const CPrimitiveClass *pc, const sf::Vector2f &topleft, const sf::Vector2f &bottomright, float time){
    CPrimitive *p = new CPrimitive();
    p->shape = new sf::Shape (sf::Shape::Rectangle(topleft.x*64.0f,topleft.y*64.0f,bottomright.x*64.0f,bottomright.y*64.0f,pc->borderColor,1.0f,pc->borderColor));
    p->shape->EnableFill(false);
    p->shape->EnableOutline(true);
    if (time >= 0.0f)
        p->time = time;
    else
        p->time = pc->defaultTime;
    mShapes.push_back(p);
}

void CPrimitivesDrawer::Draw( sf::RenderWindow* renderWindow ){
    for (unsigned int i = 0; i < mShapes.size();){
        gGame.GetRenderWindow()->Draw(*mShapes[i]->shape);
        if (mShapes[i]->time<= 0.0f){
            delete mShapes[i];
            mShapes[i] = mShapes[mShapes.size()-1];
            mShapes.pop_back();
        } else
            i++;
    }
}

void CPrimitivesDrawer::FrameStarted( float dt ){
    for (unsigned int i = 0; i < mShapes.size(); i++){
        mShapes[i]->time-=dt;
    }
}

CPrimitiveClass *CPrimitivesDrawer::GetClass(std::string name){
    if (mClasses.count(name) == 0)
        mClasses[name]=new CPrimitiveClass(name);
    return mClasses[name];
}
