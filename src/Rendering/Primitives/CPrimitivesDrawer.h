#ifndef __PRIMITIVES_DRAWER_H__
#define __PRIMITIVES_DRAWER_H__

/* Liosan, 13-04:
 * Klasa umozliwiajaca rysowanie prostych ksztaltow na ekranie,
 * glownie w celu debugowania.
 */

#include "../../Utils/CSingleton.h"
#include "../IDrawable.h"
#include "../../IFrameListener.h"

#include <SFML/System/Vector2.hpp>
#include <string>
#include <map>

namespace sf{
    class Shape;
}

#define gPrimitivesDrawer CPrimitivesDrawer::GetSingleton()

class CPrimitiveClass;

class CPrimitivesDrawer : public CSingleton<CPrimitivesDrawer>, public IDrawable, public IFrameListener {
public:
    CPrimitivesDrawer();
    virtual ~CPrimitivesDrawer();

    void DrawLine(const sf::Vector2f &from, const sf::Vector2f &to, float time = -1.0f);
    void DrawCircle(const sf::Vector2f &at, float r, float time = -1.0f);
    void DrawRect(const sf::Vector2f &topleft, const sf::Vector2f &bottomright, float time = -1.0f);

    void DrawLine(const CPrimitiveClass *pc, const sf::Vector2f &from, const sf::Vector2f &to, float time = -1.0f);
    void DrawCircle(const CPrimitiveClass *pc, const sf::Vector2f &at, float r, float time = -1.0f);
    void DrawRect(const CPrimitiveClass *pc, const sf::Vector2f &topleft, const sf::Vector2f &bottomright, float time = -1.0f);

    virtual void Draw( sf::RenderWindow* renderWindow );
    void FrameStarted(float dt);

    CPrimitiveClass *GetClass(std::string name);

    virtual bool FramesDuringPause(EPauseVariant pv) { return pv == pvLogicOnly; };
private:
    struct CPrimitive; // definicja w .cpp

    std::vector<CPrimitive*> mShapes;

    std::map<std::string, CPrimitiveClass*> mClasses;
    CPrimitiveClass *mStandardClass;
};

#endif /*__PRIMITIVES_DRAWER_H__*/
