#ifndef __PARTICLEMANAGER_H__
#define __PARTICLEMANAGER_H__

#include "../../Utils/CSingleton.h"
#include "../../IFrameListener.h"
#include "../ZIndexVals.h"
#include "../../CGame.h"
#include "../IDrawable.h"
#include "../CDrawableManager.h"

#include "CParticleSystem.h"
#include "../Effects/EffectTypes.h" // dla EffectHandle
#include "Emitters/CEmiter.h"
#include "../../Logic/CPhysical.h"

#include <list>

#define gParticleManager CParticleManager::GetSingleton()

namespace part
{
    class CParticleManager : public CSingleton<CParticleManager>, public IFrameListener, public IDrawable
    {   
    public:
        /// £aduje dane o particlach z pliku xml
        bool LoadConfiguration(const std::string &xmlFile);

        virtual void FrameStarted( float dt ) { Update(dt); }
        void Update(float dt);
        virtual void Draw( sf::RenderWindow* renderWindow ); 

        CParticleManager() 
        { 
            gGame.AddFrameListener(this);
            gDrawableManager.RegisterDrawable(this, Z_PARTICLE); // <-- tox, 5 luty, jestem brzydal
            // a jestem brzydal, bo powyzsza linijka powoduje, ze to gDrawableManager
            // ma byc odpowiedzialny za usuniecie (delete) CParticleManagera...
            //     kto aktualnie wywoluje delete CParticleManager?
        }

        bool AddParticleSystem(EffectHandle handle, float time = -1.f)
        {
            return QuickAddParticleSystem(handle, time);
        }

        bool AddParticleSystem(EffectHandle handle, const sf::Vector2f& position, float time = -1.f )
        {
            if (!QuickAddParticleSystem(handle, time))
                return false;

            reinterpret_cast<emiter::CEmiter *>(systems.back()->GetEmiter())->Reattach(position);

            return true;
        }

        bool AddParticleSystem(EffectHandle handle, CPhysical* physical, float time = -1.f )
        {
            assert(physical != NULL);

            if (handle == CreateEffectHandle("acid"))
            {
                //fprintf (stderr, "Creating acid (for at least %f ms) to %s\n",time, StringUtils::ConvertToString(physical->GetUniqueId()).c_str());
            }

            if (!QuickAddParticleSystem(handle, time))
                return false;

            reinterpret_cast<emiter::CEmiter *>(systems.back()->GetEmiter())->Reattach(physical);

            return true;
        }

        bool AddParticleSystem(EffectHandle handle, CPhysical* physicalA, CPhysical* physicalB, float time = -1.f )
        {
            assert(physicalB != NULL);
            assert(physicalA != NULL);

            if (!QuickAddParticleSystem(handle, time))
                return false;

            reinterpret_cast<emiter::CEmiter *>(systems.back()->GetEmiter())->Reattach(physicalA);

            systems.back()->AssignPhysical(physicalB);

            return true;
        }

        bool AddParticleSystem(const std::string &templateName, float time = -1.f) { return AddParticleSystem( CreateEffectHandle(templateName), time);  }
        bool AddParticleSystem(const std::string &templateName, const sf::Vector2f& position, float time = -1.f ) { return AddParticleSystem( CreateEffectHandle(templateName), position, time ); }
        bool AddParticleSystem(const std::string &templateName, CPhysical* physical, float time = -1.f ) { return AddParticleSystem( CreateEffectHandle(templateName), physical, time ); }
        bool AddParticleSystem(const std::string &templateName, CPhysical* physicalA, CPhysical* physicalB, float time = -1.f ) { return AddParticleSystem( CreateEffectHandle(templateName), physicalA, physicalB, time ); }
        
        affector::IAffector *GetAffector(EffectHandle handle) { return affectors[handle]; }
        affector::IAffector *GetAffector(const std::string &n) { return GetAffector( CreateEffectHandle(n) ); }

        unsigned int ParticleSystemsNum() { return (unsigned int)(systems.size()); }

        // usuwa efekty cz¹steczkowe od danego physicala - gdy handle == 0 usuwa wszystkie
        void RemoveFromPhysical(const CPhysical *physical, EffectHandle handle = 0);

        void ClearParticles();

    private:

        bool QuickAddParticleSystem(EffectHandle handle, float time = -1.f)
        {
            if (!ExistParticleSystemTemplate(handle))
            {
                fprintf ( stderr, "ERROR: CParticleManager - invalid particle system template handle\n" );
                return false;
            }

            systems.push_back( new CParticleSystem( *GetParticleSystemTemplate(handle) ) );
            if (time > 0.f)
                systems.back()->SetDuration ( time );

            return true;
        }

        typedef std::map<EffectHandle, CParticleSystem *> ParticleSystemMap;
        typedef std::map<EffectHandle, SParticleSystemTemplate *> ParticleSystemTemplateMap;
        typedef std::map<EffectHandle, affector::IAffector *> AffectorMap;
        typedef std::map<EffectHandle, emiter::SEmiterTemplate *> EmiterMap;
        typedef std::list<CParticleSystem *> ParticleSystemsContainer;

        AffectorMap affectors;
        ParticleSystemMap particleSystems;
        ParticleSystemTemplateMap particleTemplates;
        ParticleSystemsContainer systems;
        ParticleSystemsContainer::iterator sit; // systems iterator - dla poprawy wydajnosci
        EmiterMap emiters;
    
        //----------emiters------------
        void AddEmiter(EffectHandle handle, emiter::SEmiterTemplate &t) { emiters.insert( std::pair< EffectHandle , emiter::SEmiterTemplate *>(handle, new emiter::SEmiterTemplate(t) ) ); }
        emiter::SEmiterTemplate* GetEmiter(EffectHandle handle) { return emiters[handle]; }
        bool ExistEmiter(EffectHandle handle) { return emiters.find(handle) != emiters.end(); }

        void AddEmiter(const std::string &n, emiter::SEmiterTemplate &t) { AddEmiter( CreateEffectHandle(n), t); }
        emiter::SEmiterTemplate* GetEmiter(const std::string &n) { return GetEmiter( CreateEffectHandle(n) ); }
        bool ExistEmiter(const std::string &n) { return ExistEmiter( CreateEffectHandle(n) ); }

        //-------particlesystems-------
        void AddParticleSystemTemplate(EffectHandle handle, SParticleSystemTemplate *t) { particleTemplates.insert( std::pair< EffectHandle , SParticleSystemTemplate *>(handle,t) ); }
        SParticleSystemTemplate *GetParticleSystemTemplate(EffectHandle handle) { return particleTemplates[handle]; }
        bool ExistParticleSystemTemplate(EffectHandle handle) { return particleTemplates.find(handle) != particleTemplates.end(); }

        void AddParticleSystemTemplate(const std::string &n, SParticleSystemTemplate *t) { AddParticleSystemTemplate( CreateEffectHandle(n), t); }
        SParticleSystemTemplate *GetParticleSystemTemplate(const std::string &n) { return GetParticleSystemTemplate(CreateEffectHandle(n)); }
        bool ExistParticleSystemTemplate(const std::string &n) { return ExistParticleSystemTemplate ( CreateEffectHandle(n) ); }

        //---------affectors-----------
        void AddAffector(EffectHandle handle, affector::IAffector *a) { affectors.insert(std::pair< EffectHandle , affector::IAffector*>(handle,a)); }
        void DeleteAffector(EffectHandle handle) { AffectorMap::iterator it = affectors.find( handle ); affectors.erase(it); }
        bool ExistAffector(EffectHandle handle) { return affectors.find( handle ) != affectors.end(); }     

        void AddAffector(const std::string &n, affector::IAffector *a) { AddAffector ( CreateEffectHandle(n), a ); }
        void DeleteAffector(const std::string &n) { DeleteAffector( CreateEffectHandle(n) ); }
        bool ExistAffector(const std::string &n) { return  ExistAffector( CreateEffectHandle(n) ); }

        void Cleanup();
    };
};

#endif //__PARTICLEMANAGER_H__//

