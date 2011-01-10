#include "CEmiter.h"
#include "SEmiterTemplate.h"
#include "../../../Utils/MathsFunc.h"
#include "../../../Utils/CRand.h"
#include "../../../Logic/CPhysical.h"
#include "../../../Map/CMap.h"

part::emiter::CEmiter::CEmiter( const SEmiterTemplate &tmp, sf::Vector2f position ) :
    param(tmp),
    pos(position),
    obj(0),
    tracking(STATIC),
    mAcc(0)
{
}
//------------------
part::emiter::CEmiter::CEmiter( const SEmiterTemplate &tmp, const CPhysical* physical ) :
    param(tmp),
    obj(physical),
    tracking(PHYSICAL),
    mAcc(0)
{
}
//------------------
part::emiter::CEmiter::CEmiter( const SEmiterTemplate &tmp ) :
    param(tmp),
    obj(0),
    tracking(INVALID),
    mAcc(0)
{
}
//------------------
unsigned part::emiter::CEmiter::Emit(CParticle *ps, unsigned alive, unsigned count, float dt)
{
    static float slowing = 2.f;

    switch (tracking)
    {
    case STATIC:
        break;
    case PHYSICAL:
        if (obj == NULL) {
            tracking = INVALID;
            return 0;
        }

        pos = obj->GetPosition();
        break;
    case INVALID:
        return 0;
    };

    if (alive <= count)
    {
        mAcc += dt;
        while (mAcc > param.emitSpeed)
        {            
            ++alive;
            alive = alive > count? count : alive;
            slowing *= 4.f;
            mAcc -= param.emitSpeed * slowing;
        }
        slowing = 2.f;
        if (mAcc < 0.f)
            mAcc = 0.f;
    }   

    float ang;
    float rad = gRand.Rndf( param.radius[MIN_VALUE], param.radius[MAX_VALUE] );

    for (unsigned i=0; i < alive; ++i)
    {
        part::CParticle &p = *(ps+i);
        
        if (p.life <= 0.f)
        {
            if (param.circleForce)
            {
                ang = gRand.Rndf( param.ang[MIN_VALUE], param.ang[MAX_VALUE] );                
                
                p.pos.x = Map::TILE_SIZE * (pos.x + sin(ang) * rad);
                p.pos.y = Map::TILE_SIZE * (pos.y + cos(ang) * rad);
            }
            else
            {
                p.pos.x = pos.x * Map::TILE_SIZE;
                p.pos.y = pos.y * Map::TILE_SIZE;
            }

            p.size = gRand.Rndf( param.size[MIN_VALUE], param.size[MAX_VALUE] );
            
            p.rotation = gRand.Rndf( param.rotation[MIN_VALUE], param.rotation[MAX_VALUE] );
            
            p.life = gRand.Rndf( param.life[MIN_VALUE], param.life[MAX_VALUE] ); 

            p.color = sf::Color( gRand.Rnd( param.color[MIN_VALUE].r, param.color[MAX_VALUE].r ),
                                 gRand.Rnd( param.color[MIN_VALUE].g, param.color[MAX_VALUE].g ),
                                 gRand.Rnd( param.color[MIN_VALUE].b, param.color[MAX_VALUE].b ),
                                 static_cast<unsigned char>( Maths::Clamp(p.life * 255.f,0.f,255.f) ));

            p.textureID = gRand.Rnd( param.texture[MIN_VALUE], param.texture[MAX_VALUE] );

            //--------------------------
            p.mod.force = sf::Vector2f( gRand.Rndf( param.mod[MIN_VALUE].force.x, param.mod[MAX_VALUE].force.x  ),
                                        gRand.Rndf( param.mod[MIN_VALUE].force.y, param.mod[MAX_VALUE].force.y  ) );

            p.mod.life = gRand.Rndf( param.mod[MIN_VALUE].life, param.mod[MAX_VALUE].life );
            p.mod.rotation = gRand.Rndf( param.mod[MIN_VALUE].rotation, param.mod[MAX_VALUE].rotation );
            p.mod.size = gRand.Rndf( param.mod[MIN_VALUE].size, param.mod[MAX_VALUE].size );
        }
    }

    return alive;
}
