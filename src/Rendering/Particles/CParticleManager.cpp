#include "CParticleManager.h"
#include "../ZIndexVals.h"
#include "../../Utils/CXml.h"
#include "CParticleSystem.h"
#include "Affectors/Affectors.h"
#include "Renderers/CSpriteRender.h"
#include "Emitters/SEmiterTemplate.h"

template<> part::CParticleManager *CSingleton<part::CParticleManager>::msSingleton = 0;

void part::CParticleManager::Update(float dt)
{
    for (sit = systems.begin(); sit != systems.end();)
    {
        (*sit)->Update(dt);

        if ((*sit)->GetState() == State::CanDelete)
        {
            (*sit)->Clean();
            delete (*sit);
            systems.erase(sit++);
        }
        else
            ++sit;
    }
}
//--------------
void part::CParticleManager::RemoveFromPhysical(const CPhysical *physical, EffectHandle handle)
{
    assert(physical != NULL);

    part::emiter::CEmiter *e;
    if (handle == 0)
    {
        for (sit = systems.begin(); sit != systems.end(); ++sit)
        {
            e = reinterpret_cast<part::emiter::CEmiter *> ( (*sit)->GetEmiter() );
            if ( e->GetPhysicalIfAssigned() == physical)
                e->Reattach();
        }
    }
    else
    {
        for (sit = systems.begin(); sit != systems.end(); ++sit)
        {
            if ( (*sit)->GetEffectHandle() == handle) {
                e = reinterpret_cast<part::emiter::CEmiter *> ( (*sit)->GetEmiter() );
                if ( e->GetPhysicalIfAssigned() == physical)
                    e->Reattach();
            }
        }
    }
}

void part::CParticleManager::ClearParticles()
{
    for (sit = systems.begin(); sit != systems.end(); ++sit)
    {
        (*sit)->Clean();
        delete (*sit);
    }
    systems.clear();
}

//--------------
void part::CParticleManager::Draw( sf::RenderWindow* renderWindow )
{
    // tox, 5 luty: renderWindow jest ignorowane, particle sie same rysuja na
    // gGame.mRenderWindow...
    for (sit = systems.begin(); sit != systems.end(); ++sit)
        (*sit)->Draw();
}
//-----------------------
bool part::CParticleManager::LoadConfiguration(const std::string &xmlFile)
{
    //fprintf( stderr, "\nLoading Particle Configuration from %s file...\n", xmlFile.c_str() );

    CXml xml(xmlFile,"root");

    if (xml.GetString(0,"type") != "particles")
        return false;

    xml_node<>* node;
    xml_node<>* params;
    xml_node<>* pparams;
    std::string type;

    //-------------------------------
    // wczytanie wzorów dla emiterów
    //-------------------------------

    for (node = xml.GetChild(0,"emiter"); node; node = xml.GetSibl(node, "emiter"))
    {
        type = xml.GetString(node,"name");

        emiter::SEmiterTemplate e;

        if ((params = xml.GetChild(node, "speed"))) {
            e.emitSpeed = xml.GetFloat(params, "value");
        }

        if ((params = xml.GetChild(node, "life"))) {
            e.life[emiter::MIN_VALUE] = xml.GetFloat(params, "min");
            e.life[emiter::MAX_VALUE] = xml.GetFloat(params, "max");
        }
        
        if ((params = xml.GetChild(node, "textures"))) {
            e.texture[emiter::MIN_VALUE] = xml.GetInt(params, "start");
            e.texture[emiter::MAX_VALUE] = xml.GetInt(params, "end");

            if (   !renderer::gSpriteRenderer.IsValid( e.texture[emiter::MIN_VALUE])
                || !renderer::gSpriteRenderer.IsValid( e.texture[emiter::MAX_VALUE]) )
            {
                fprintf( stderr, "ERROR: Invalid textures range in emiter %s ( range starts at %d and end at %d )\n", 
                    type.c_str(), e.texture[emiter::MIN_VALUE], e.texture[emiter::MAX_VALUE] );
                continue;
            }
        }

        if ((params = xml.GetChild(node, "rotation"))) {
            e.rotation[emiter::MIN_VALUE] = xml.GetFloat(params, "min");
            e.rotation[emiter::MAX_VALUE] = xml.GetFloat(params, "max");
        }

        if ((params = xml.GetChild(node, "size"))) {
            e.size[emiter::MIN_VALUE] = xml.GetFloat(params, "min");
            e.size[emiter::MAX_VALUE] = xml.GetFloat(params, "max");
        }

        if ((params = xml.GetChild(node, "color"))) {
            e.color[emiter::MAX_VALUE].r = xml.GetInt(params,"r");
            e.color[emiter::MAX_VALUE].g = xml.GetInt(params,"g");
            e.color[emiter::MAX_VALUE].b = xml.GetInt(params,"b");
            
            // tymczasowo:
            e.color[emiter::MIN_VALUE] = e.color[emiter::MAX_VALUE];
        }

        if ((params = xml.GetChild(node, "circle")))
        {
            e.circleForce = true;

            e.ang[emiter::MIN_VALUE] = xml.GetFloat(params,"ang-min");
            e.ang[emiter::MAX_VALUE] = xml.GetFloat(params,"ang-max");

            e.radius[emiter::MIN_VALUE] = xml.GetFloat(params,"radius-min");
            e.radius[emiter::MAX_VALUE] = xml.GetFloat(params,"radius-max");
        }

        if ((params = xml.GetChild(node, "mods")))
        {
            if ((pparams = xml.GetChild(params, "life"))) {
                e.mod[emiter::MIN_VALUE].life = xml.GetFloat(pparams,"min");
                e.mod[emiter::MAX_VALUE].life = xml.GetFloat(pparams,"max");
            }

            if ((pparams = xml.GetChild(params, "rotation"))) {
                e.mod[emiter::MIN_VALUE].rotation = xml.GetFloat(pparams,"min");
                e.mod[emiter::MAX_VALUE].rotation = xml.GetFloat(pparams,"max");
            }

            if ((pparams = xml.GetChild(params, "size"))) {
                e.mod[emiter::MIN_VALUE].size = xml.GetFloat(pparams,"min");
                e.mod[emiter::MAX_VALUE].size = xml.GetFloat(pparams,"max");
            }

            if ((pparams = xml.GetChild(params, "force"))) {
                e.mod[emiter::MIN_VALUE].force.x = xml.GetFloat(pparams,"x-min");            
                e.mod[emiter::MIN_VALUE].force.y = xml.GetFloat(pparams,"y-min");

                e.mod[emiter::MAX_VALUE].force.x = xml.GetFloat(pparams,"y-max");
                e.mod[emiter::MAX_VALUE].force.x = xml.GetFloat(pparams,"x-max");            
            }
        }

        AddEmiter( type, e );
    };

    //-------------------------------
    // wczytanie afektorów
    //-------------------------------

    AddAffector( "foo", new affector::CFooAffector() );

    for (node = xml.GetChild(0,"affector"); node; node = xml.GetSibl(node,"affector"))
    {
        // wczytaj dane i stwórz odpowiedni affector
        type = xml.GetString(node,"type");

        if (type == "incremental")
        {
            affector::SIncrementalParams p;

            if ((params = xml.GetChild(node,"life"))) {
                p.life = xml.GetFloat(params,"value");
            }

            if ((params = xml.GetChild(node,"size"))) {
                p.size = xml.GetFloat(params,"value");
            }

            if ((params = xml.GetChild(node,"rotation"))) {
                p.rotation = xml.GetFloat(params,"value");
            }

            if ((params = xml.GetChild(node, "velocity"))) {
                p.vel.x = xml.GetFloat(params,"x");
                p.vel.y = xml.GetFloat(params,"y");
            }
            
            if ((params = xml.GetChild(node, "force"))) {
                p.force.x = xml.GetFloat(params,"x");
                p.force.y = xml.GetFloat(params,"y");
            }

            if ((params = xml.GetChild(node, "gravity"))) {
                p.grav.x = xml.GetFloat(params,"x");
                p.grav.y = xml.GetFloat(params,"y");
            }

            AddAffector(xml.GetString(node,"name"),new affector::CIncremental(p));
        }
        else if (type == "random")
        {
            affector::SRandomParams p;

            if ((params = xml.GetChild(node,"size"))) {
                p.sizeMin = xml.GetFloat(params,"min");
                p.sizeMax = xml.GetFloat(params,"max");
            }

            if ((params = xml.GetChild(node,"rotation"))) {
                p.rotMin = xml.GetFloat(params,"min");
                p.rotMax = xml.GetFloat(params,"max");
            }

            if ((params = xml.GetChild(node,"life"))) {
                p.lifeMin = xml.GetFloat(params,"min");
                p.lifeMax = xml.GetFloat(params,"max");
            }

            if ((params = xml.GetChild(node,"velocity"))) {
                p.velMin.x = xml.GetFloat(params, "x-min");
                p.velMax.x = xml.GetFloat(params,"x-max");
                p.velMin.y = xml.GetFloat(params,"y-min");
                p.velMax.y = xml.GetFloat(params,"y-max");
            }

            AddAffector(xml.GetString(node,"name"),new affector::CRandom(p));
        }
        else if (type == "spray")
        {
            affector::SSprayParams p;

            if ((params = xml.GetChild(node, "random"))) {
                p.rndRotation = (xml.GetInt(params,"value") != 0);
            }

            if ((params = xml.GetChild(node, "radius"))) {
                p.radMinX = xml.GetFloat(params, "min-x");
                p.radMaxX = xml.GetFloat(params, "max-x");
                p.radMinY = xml.GetFloat(params, "min-y");
                p.radMaxY = xml.GetFloat(params, "max-y");
            }

            AddAffector(xml.GetString(node, "name"), new affector::CSprayAffector(p));
        }
        else if (type == "pointmass")
        {
			/*
			src/Rendering/Particles/CParticleManager.cpp:270: warning: 'p.part::affector::SPointMassParams::mass' may be used uninitialized in this function
			src/Rendering/Particles/CParticleManager.cpp:270: warning: 'p.part::affector::SPointMassParams::minDistanceSquare' may be used uninitialized in this function
			src/Rendering/Particles/CParticleManager.cpp:270: warning: 'p.part::affector::SPointMassParams::maxDistanceSquare' may be used uninitialized in this function
			*/
            affector::SPointMassParams p;

			p.mass = p.maxDistanceSquare = p.minDistanceSquare = -666;		

            if ((params = xml.GetChild(node, "mass"))) {
                p.mass = xml.GetFloat(params, "value");
            }

            if ((params = xml.GetChild(node, "field"))) {
                p.minDistanceSquare = xml.GetFloat(params, "min");
                p.maxDistanceSquare = xml.GetFloat(params, "max");

                p.minDistanceSquare *= p.minDistanceSquare;
                p.maxDistanceSquare *= p.maxDistanceSquare;
            }

            AddAffector(xml.GetString(node, "name"), new affector::CPointMass(p));
        }
        else if (type == "merge")
        {
            if (ExistAffector(xml.GetString(node,"a")) && ExistAffector(xml.GetString(node,"b")))
            {
                if (ExistAffector(xml.GetString(node,"c")))
                {
                    AddAffector( xml.GetString(node,"name"), 
                        new affector::CMerge3( this->GetAffector( xml.GetString(node,"a") ),
                                               this->GetAffector( xml.GetString(node,"b") ),
                                               this->GetAffector( xml.GetString(node,"c") ) )
                                               );
                }
                else
                {
                    AddAffector(xml.GetString(node,"name"),
                        new affector::CMerge( this->GetAffector(xml.GetString(node,"a")),
                                              this->GetAffector(xml.GetString(node,"b")) ) 
                                      );

                }
            }
            else
                fprintf( stderr, "Error at affector merge - not found affectors to merge!\n");
        }
        else
            fprintf( stderr, "Not known affector type - %s\n",xml.GetString(node,"type").c_str());
    }

    //-------------------------------
    // utworzenie system cz¹steczek
    //-------------------------------

    SParticleSystemTemplate psTemplate;
    std::string affector;
    std::string emiter;
    std::string blendMode = "normal";
    for (node = xml.GetChild(0,"particlesystem"); node; node = xml.GetSibl(node,"particlesystem"))
    {
        type = xml.GetString(node,"name");
        affector = xml.GetString(node,"affector");
        emiter = xml.GetString(node,"emiter");

        if ( !ExistAffector(affector) ) {
            fprintf( stderr, "ERROR: Affector %s doesn't exist (particle system - %s)\nSetting foo affector instead.\n",affector.c_str(),type.c_str());
            psTemplate.mAffector = GetAffector("foo");
        }
        else
            psTemplate.mAffector = GetAffector(affector);

        psTemplate.mCount = xml.GetInt(node,"count");
        psTemplate.mDurationMin = 10.f;
        psTemplate.mDurationMax = 15.f;

        if ( !ExistEmiter(emiter) ) {
            fprintf( stderr, "ERROR: Emiter %s doesn't exist (particle system - %s)\nCritical error - skiping this particle system\n",emiter.c_str(),type.c_str());
            continue;
        }

        psTemplate.mEmiterTemplate = GetEmiter(emiter);

        blendMode = xml.GetString(node,"blendmode");
        if (blendMode == "" || blendMode == "normal")
            psTemplate.mBlendMode = sf::Blend::Alpha;
        else if (blendMode == "add")
            psTemplate.mBlendMode = sf::Blend::Add;
        else if (blendMode == "multiply")
            psTemplate.mBlendMode = sf::Blend::Multiply;
        else if (blendMode == "none")
            psTemplate.mBlendMode = sf::Blend::None;
        else {
            fprintf ( stderr, "WARNING: incorrect blend mode (particle system - %s, assuming normal blend mode.", type.c_str() );
            psTemplate.mBlendMode = sf::Blend::Alpha;
        }

        psTemplate.mHandle = CreateEffectHandle( type );

        AddParticleSystemTemplate( type, new SParticleSystemTemplate( psTemplate) );
    }

    //fprintf( stderr, "\nReading configuration finished.\n");

    return true;
}
//------------------------
void part::CParticleManager::Cleanup()
{
    for (sit = systems.begin(); sit != systems.end(); delete (*sit++) );
    systems.clear();

    for (ParticleSystemTemplateMap::iterator it = particleTemplates.begin(); it != particleTemplates.end();)
        delete (it++)->second;
    particleTemplates.clear();

    for (AffectorMap::iterator it = affectors.begin(); it != affectors.end();)
        delete (it++)->second;
    affectors.clear();

    for (EmiterMap::iterator it = emiters.begin(); it != emiters.end();)
        delete (it++)->second;
    emiters.clear();

}
