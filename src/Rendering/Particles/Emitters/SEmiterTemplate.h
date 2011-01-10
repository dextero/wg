#ifndef __PART_SEMITERTEMPLATE_H__
#define __PART_SEMITERTEMPLATE_H__

#include <SFML/System/Vector3.hpp>

// blee, brzydki includ dla g++:
#include <string.h>

namespace part
{
    namespace emiter
    {
        enum { MIN_VALUE = 0, MAX_VALUE = 1 };

        struct SEmiterTemplate
        {
            float emitSpeed;
            
            bool circleForce;            
            float ang[2];
            float radius[2];
            
            float life[2];
            float rotation[2];
            float size[2];

            sf::Color color[2];

            unsigned texture[2];

            SParticleMod mod[2];

            SEmiterTemplate() { 
                memset(this, 0, sizeof( SEmiterTemplate )); 
                color[MIN_VALUE] = color[MAX_VALUE] = sf::Color(255,255,255);
                emitSpeed = 0.01f;
            }
        };

    };
};

#endif //__PART_SEMITERTEMPLATE_H__//
