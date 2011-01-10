#ifndef __CEDITORRESOURCESET_H__
#define __CEDITORRESOURCESET_H__

#ifdef __EDITOR__
#   include <map>
#endif

#include <vector>
#include <string>

#include <SFML/System/Vector2.hpp> 

/**
 * Liosan, 20.11
 * Klasa zawierajaca zestaw zasobow edytora,
 * ktore beda wyswietlane w panelu i dostepne do umieszczenia na mapie
 * Takie zestawy to np. kafle, physicale czy doodahy
 */
 
class IEditorResource;

class CEditorResourceSet{
public:
    CEditorResourceSet(){}

#ifndef __EDITOR__
    virtual ~CEditorResourceSet() {}

    inline void Add(IEditorResource *er){ mResources.push_back(er); }
    inline const std::vector<IEditorResource*> &GetResources(){ return mResources; }
private:
    std::vector<IEditorResource*> mResources;
#else
    virtual ~CEditorResourceSet() {}

    // nie chce mi sie pisac tony ifdefow, zeby poprawic bledy, wiec niech to nic nie robi..
    inline void Add(IEditorResource* res) {}
    inline const std::vector<IEditorResource*> GetResources(){ return std::vector<IEditorResource*>(); }

    // wlasciwe funkcje
    inline void Add(const std::string& path, IEditorResource* res)
    {
        mResources.insert(std::pair<const std::string, IEditorResource*>(path, res));
    }

    bool Contains(const std::string& path) { return mResources.find(path) != mResources.end(); }

    // NIE SPRAWDZA obecnosci w mapie!
    IEditorResource* Get(const std::string& path) { return mResources[path]; }

    inline const std::map<const std::string, IEditorResource*>& GetResourcesMap() { return mResources; }

private:
    std::map<const std::string, IEditorResource*> mResources;
#endif
};


#endif//__CEDITORRESOURCESET_H__
