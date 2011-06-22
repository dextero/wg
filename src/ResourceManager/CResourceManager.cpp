#include "CResourceManager.h"
#include "../Utils/StringUtils.h"

#include "CImage.h"
#include "CSound.h"
#include "CMusic.h"
#include "CFont.h"
#include "CImageAtlas.h"
#include "../Map/CMap.h"
#include "../Logic/Factory/CPhysicalTemplate.h"
#include "../Logic/Abilities/CAbility.h"
#include "../Logic/Factory/CEnemyTemplate.h"
#include "../Logic/Factory/CNPCTemplate.h"
#include "../Logic/Factory/CBulletTemplate.h"
#include "../Logic/Factory/CLairTemplate.h"
#include "../Logic/Factory/CWallTemplate.h"
#include "../Logic/Factory/CPlayerTemplate.h"
#include "../Logic/Factory/CLootTemplate.h"
#include "../Logic/Factory/CDoorTemplate.h"
#include "../Logic/Factory/CHookTemplate.h"
#include "../Logic/Factory/CObstacleTemplate.h"
#include "../Logic/CBestiary.h"
#include "../Utils/CXml.h"
#include <boost/filesystem/operations.hpp>

using namespace System::Resource;
using namespace Map;

template<> CResourceManager* CSingleton<CResourceManager>::msSingleton = 0;
unsigned long long CResourceManager::mResourceEnumerator = 0x1000;

CResourceManager::CResourceManager():
mErrorsOccured(0)
{
	fprintf( stderr, "CResourceManager::CResourceManager()\n" );
}

CResourceManager::~CResourceManager()
{
	fprintf( stderr, "CResourceManager::~CResourceManager()\n" );
	Clear();
}

template <typename T> T* CResourceManager::GetResource(const std::string& name, bool verbose)
{
	ResourceHandle handle = StringUtils::GetStringHash(name);
    if(mResources.count(handle) > 0)
        return (T*)mResources[handle];
    else if(LoadResource<T>(name,verbose))
        return (T*)mResources[handle];
    else
        return (T*)NULL;
}

template <typename T> T* CResourceManager::GetResource(ResourceHandle handle)
{
    if(mResources.count(handle) > 0)
        return (T*)mResources[handle];
    else
        return (T*)NULL;
}

template <typename T> ResourceHandle CResourceManager::LoadResource(const std::string& name, bool verbose, int gid)
{
    std::map<std::string,std::string> argv;
    argv["name"] = name;
    if (!verbose)
        argv["verbose"] = "no";
    return LoadResource<T>(argv, gid);
}

template <typename T> ResourceHandle CResourceManager::LoadResource(std::map<std::string,std::string>& argv, int gid)
{
    ResourceHandle handle = InvalidResourceHandle;

	if(argv["name"].size() == 0)
		handle = mResourceEnumerator++;
	else
		handle = StringUtils::GetStringHash(argv["name"]);
#ifndef __I_AM_TOXIC__
    if(mResources.count(handle) != 0)
        return handle;
#endif

    IResource* res = new T();
	res->mGid = gid;
    if(res->Load(argv))
    {
        mResources[handle] = res;
        mLoadedResourceNames.push_back(argv["name"]);
        return handle;
    }
    else
    {
        res->Drop();
        return InvalidResourceHandle;
    }
}

namespace System
{
    namespace Resource
    {

template <> ResourceHandle CResourceManager::LoadResource<CPhysicalTemplate>(std::map<std::string,std::string>& argv, int gid)
{
    ResourceHandle handle = InvalidResourceHandle;

	if(argv["name"].size() == 0)
		handle = mResourceEnumerator++;
	else
		handle = StringUtils::GetStringHash(argv["name"]);
    if(mResources.count(handle) != 0)
        return handle;

    CXml xml( argv["name"], "root" );
    const std::string &type = xml.GetString(0,"type");
    IResource *res;
    if (type == "lair")
        res = new CLairTemplate();
    else if (type == "enemy")
        res = new CEnemyTemplate();
	else if (type == "npc")
		res = new CNPCTemplate();
    else if (type == "bullet")
        res = new CBulletTemplate();
    else if (type == "wall")
        res = new CWallTemplate();
    else if (type == "player")
        res = new CPlayerTemplate();
    else if (type == "loot")
        res = new CLootTemplate();
    else if (type == "door")
        res = new CDoorTemplate();
    else if (type == "obstacle")
        res = new CObstacleTemplate();
    else if (type == "hook")
        res = new CHookTemplate();
	else {
		if ((type != "ability-tree") && (type != "trigger-effects") && (type != "droplist")){
			fprintf(stderr,"ERROR: Resource %s not found\n", argv["name"].c_str());
			mErrorsOccured++;
		}
        return InvalidResourceHandle;
	}
    res->mGid = gid;
    if(res->Load(argv))
    {
        mResources[handle] = res;
        mLoadedResourceNames.push_back(argv["name"]);
        return handle;
    }
    else
    {
		fprintf(stderr,"ERROR: Resource %s not found\n", argv["name"].c_str());
		mErrorsOccured++;
        res->Drop();
        return InvalidResourceHandle;
    }
}

    } // namespace Resource
} // namespace System

void CResourceManager::DropResource(const std::string& name)
{
	ResourceHandle handle = StringUtils::GetStringHash(name);
	if(mResources.count(handle) > 0)
	{
		if(mResources[handle] != NULL)
			mResources[handle]->Drop();
		mResources.erase(handle);
	}
}

void CResourceManager::DropResourcesByGid(int gid)
{
	std::map<ResourceHandle,IResource*>::iterator tmp;
	std::map<ResourceHandle,IResource*>::iterator i = mResources.begin();
	while(i != mResources.end())
	{
		if((*i).second && (*i).second->mGid == (ResourceHandle)(gid))
		{
			tmp = i;
			i++;
			(*tmp).second->Drop();
			mResources.erase(tmp);
		}
		i++;
	}
	mResources.clear();
}

void CResourceManager::Clear()
{
	while(mResources.size())
	{
		if(mResources.begin()->second)
			mResources.begin()->second->Drop();
		mResources.erase(mResources.begin());

	}
}

ResourceHandle CResourceManager::LoadImage(const std::string& name, int gid) {return LoadResource<CImage>(name, false, gid);};
ResourceHandle CResourceManager::LoadMusic(const std::string& name, int gid) {return LoadResource<CMusic>(name, false, gid);};
ResourceHandle CResourceManager::LoadSound(const std::string& name, int gid) {return LoadResource<CSound>(name, false, gid);};
ResourceHandle CResourceManager::LoadFont(const std::string& name, int gid) {return LoadResource<CFont>(name, false, gid);};
ResourceHandle CResourceManager::LoadMap(const std::string& name, int gid) {return LoadResource<CMap>(name, false, gid);};
ResourceHandle CResourceManager::LoadPhysicalTemplate(const std::string& name, int gid) {return LoadResource<CPhysicalTemplate>(name, false, gid);};
ResourceHandle CResourceManager::LoadAbility(const std::string& name, int gid) {return LoadResource<CAbility>(name,false, gid);};
ResourceHandle CResourceManager::LoadAtlas(const std::string& name, int gid) { return LoadResource<CImageAtlas>( name, false, gid ); };

ResourceHandle CResourceManager::LoadImage(std::map<std::string,std::string>& argv, int gid) {return LoadResource<CImage>(argv, gid);};
ResourceHandle CResourceManager::LoadMusic(std::map<std::string,std::string>& argv, int gid) {return LoadResource<CMusic>(argv, gid);};
ResourceHandle CResourceManager::LoadSound(std::map<std::string,std::string>& argv, int gid) {return LoadResource<CSound>(argv, gid);};
ResourceHandle CResourceManager::LoadFont(std::map<std::string,std::string>& argv, int gid) {return LoadResource<CFont>(argv, gid);};
ResourceHandle CResourceManager::LoadMap(std::map<std::string,std::string>& argv, int gid) {return LoadResource<CMap>(argv, gid);};
ResourceHandle CResourceManager::LoadPhysicalTemplate(std::map<std::string,std::string>& argv, int gid) {return LoadResource<CPhysicalTemplate>(argv, gid);};
ResourceHandle CResourceManager::LoadAbility(std::map<std::string,std::string>& argv, int gid) {return LoadResource<CAbility>(argv, gid);};
ResourceHandle CResourceManager::LoadAtlas(std::map<std::string,std::string>& argv, int gid) { return LoadResource<CImageAtlas>(argv, gid); };

CImage* CResourceManager::GetImage(const std::string& name) {return (CImage*)GetResource<CImage>(name);};
CMusic* CResourceManager::GetMusic(const std::string& name) {return (CMusic*)GetResource<CMusic>(name);};
CSound* CResourceManager::GetSound(const std::string& name) {return (CSound*)GetResource<CSound>(name);};
sf::Font* CResourceManager::GetFont(const std::string& name) {return (sf::Font*)GetResource<CFont>(name);};
CMap* CResourceManager::GetMap(const std::string& name) {return (CMap*)GetResource<CMap>(name);};
CPhysicalTemplate* CResourceManager::GetPhysicalTemplate(const std::string& name) {return (CPhysicalTemplate*)GetResource<CPhysicalTemplate>(name);};
CAbility* CResourceManager::GetAbility(const std::string& name) {return (CAbility*)GetResource<CAbility>(name);};
CImageAtlas* CResourceManager::GetAtlas(const std::string& name) { return (CImageAtlas*)GetResource<CImageAtlas>(name); }
CImageAtlas* CResourceManager::TryGetAtlas(const std::string& name) { return (CImageAtlas*)GetResource<CImageAtlas>(name,false); }
CImagePart CResourceManager::GetImagePart(const std::string& name, int number) { return GetAtlas(name)->GetImage(number); }

CImage* CResourceManager::GetImage(ResourceHandle handle) {return (CImage*)GetResource<CImage>(handle);};
CMusic* CResourceManager::GetMusic(ResourceHandle handle) {return (CMusic*)GetResource<CMusic>(handle);};
CSound* CResourceManager::GetSound(ResourceHandle handle) {return (CSound*)GetResource<CSound>(handle);};
sf::Font* CResourceManager::GetFont(ResourceHandle handle) {return (sf::Font*)GetResource<CFont>(handle);};
CMap* CResourceManager::GetMap(ResourceHandle handle) {return (CMap*)GetResource<CMap>(handle);};
CPhysicalTemplate* CResourceManager::GetPhysicalTemplate(ResourceHandle handle) {return (CPhysicalTemplate*)GetResource<CPhysicalTemplate>(handle);};
CAbility* CResourceManager::GetAbility(ResourceHandle handle) {return (CAbility*)GetResource<CAbility>(handle);};
CImageAtlas* CResourceManager::GetAtlas(ResourceHandle handle) { return (CImageAtlas*)GetResource<CImageAtlas> (handle); }
CImagePart CResourceManager::GetImagePart(ResourceHandle handle, int number) { return GetAtlas(handle)->GetImage( number ); }

void CResourceManager::ForceLoadAll(bool fullDebug){
	std::vector<std::string> foundFiles, foundDirectories;
	foundDirectories.push_back("data");
	while (foundDirectories.size() > 0){
		std::string dir = foundDirectories[foundDirectories.size()-1];
		foundDirectories.pop_back();
		boost::filesystem::directory_iterator di(dir),dir_end;
		for (; di != dir_end; di++){
			if (di->leaf() == ".svn")
				continue;
			std::string file = dir + "/" + di->leaf();
			if (boost::filesystem::is_directory(file))
				foundDirectories.push_back(file);
			else
				foundFiles.push_back(file);
		}
	}
	fprintf(stderr,"Initiating loading sequence for %d files...\n",(int)(foundFiles.size()));
	for (size_t i = 0; i < foundFiles.size(); i++){
		std::string f = foundFiles[i];
		if (f.find("/old") == std::string::npos){
            if (f.rfind(".xml")!=std::string::npos){
                if (f.find("data/maps/")==std::string::npos){
                    if ((f.find("data/physicals") != std::string::npos) ||
                        (f.find("data/player") != std::string::npos) ||
                        (f.find("data/loots") != std::string::npos)){
                        fprintf(stderr,"Template: %s\n",f.c_str());
                        LoadResource<CPhysicalTemplate>(f);
					} else if (f.find("data/abilities") != std::string::npos){
                        fprintf(stderr,"Ability: %s\n",f.c_str());
                        LoadResource<CAbility>(f);
                    } else {
                        if (fullDebug) fprintf(stderr,"ignored .xml file: %s\n",f.c_str());
                    }
                } else if (f.find("data/maps/toxic")==std::string::npos){
                    fprintf(stderr,"Map: %s\n",f.c_str());
                    LoadResource<CMap>(f);
                }
            } else if (f.rfind(".png")!=std::string::npos){
                if (fullDebug) fprintf(stderr,"Image: %s\n",f.c_str());
                LoadResource<CImage>(f);
            }
		}
	}
	fprintf(stderr,"Loading sequence complete\n");
}


void CResourceManager::GetAllStartingWith(const std::string &path, const std::string &type, std::vector<std::string> &out){
    for (unsigned int i = 0; i < mLoadedResourceNames.size(); i++)
        if (mLoadedResourceNames[i].find(path) == 0)
            if (mResources[StringUtils::GetStringHash(mLoadedResourceNames[i])]->GetType() == type)
                out.push_back(mLoadedResourceNames[i]);
}
