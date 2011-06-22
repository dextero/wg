#ifndef __CRESOURCEMANAGER_H__
#define __CRESOURCEMANAGER_H__

#include "../Utils/CSingleton.h"

#include "IResource.h"

#include <map>
#include <string>
#include <string>

#define gResourceManager System::Resource::CResourceManager::GetSingleton()

class CPhysicalTemplate;
class CAbility;
class CImagePart;

namespace sf{
    class Font;
}

namespace Map{
    class CMap;
}

namespace System
{
	namespace Resource
	{
        class CImage;
        class CSound;
        class CMusic;
        class CFont;
        class CImageAtlas;
		/** @brief Manager zasobow.
		*	@author Jarosław Dutka
		*	@date 2009.01.03
		*	@version 1.0
		*/
		class CResourceManager : public CSingleton< CResourceManager >
		{
		private:
			static unsigned long long mResourceEnumerator;
			std::map<ResourceHandle,IResource*> mResources;
            std::vector<std::string> mLoadedResourceNames;
			int mErrorsOccured;

			template <typename T> T* GetResource(const std::string& name, bool verbose = true);
			template <typename T> T* GetResource(ResourceHandle handle);
			template <typename T> ResourceHandle LoadResource(const std::string& name, bool verbose = true, int gid = 0);
			template <typename T> ResourceHandle LoadResource(std::map<std::string,std::string>& argv, int gid = 0);
		public:
			CResourceManager();
			virtual ~CResourceManager();

			ResourceHandle LoadImage(const std::string& name, int gid = 0);
			ResourceHandle LoadMusic(const std::string& name, int gid = 0);
			ResourceHandle LoadSound(const std::string& name, int gid = 0);
			ResourceHandle LoadFont(const std::string& name, int gid = 0);
			ResourceHandle LoadMap(const std::string& name, int gid = 0);
			ResourceHandle LoadPhysicalTemplate(const std::string& name, int gid = 0);
			ResourceHandle LoadAbility(const std::string& name, int gid = 0);
            ResourceHandle LoadAtlas(const std::string& name, int gid = 0);

			ResourceHandle LoadImage(std::map<std::string,std::string>& argv, int gid = 0);
			ResourceHandle LoadMusic(std::map<std::string,std::string>& argv, int gid = 0);
			ResourceHandle LoadSound(std::map<std::string,std::string>& argv, int gid = 0);
			ResourceHandle LoadFont(std::map<std::string,std::string>& argv, int gid = 0);
			ResourceHandle LoadMap(std::map<std::string,std::string>& argv, int gid = 0);
			ResourceHandle LoadPhysicalTemplate(std::map<std::string,std::string>& argv, int gid = 0);
			ResourceHandle LoadAbility(std::map<std::string,std::string>& argv, int gid = 0);
            ResourceHandle LoadAtlas(std::map<std::string,std::string>& argv, int gid = 0);

			CImage* GetImage(const std::string& name);
			CMusic* GetMusic(const std::string& name);
			CSound* GetSound(const std::string& name);
			sf::Font* GetFont(const std::string& name);
			Map::CMap* GetMap(const std::string& name);
			CPhysicalTemplate* GetPhysicalTemplate(const std::string& name);
			CAbility* GetAbility(const std::string& name);
            CImageAtlas* GetAtlas(const std::string& name);
            CImageAtlas* TryGetAtlas(const std::string &name);
            CImagePart GetImagePart(const std::string& name, int number);

			CImage* GetImage(ResourceHandle handle);
			CMusic* GetMusic(ResourceHandle handle);
			CSound* GetSound(ResourceHandle handle);
			sf::Font* GetFont(ResourceHandle handle);
			Map::CMap* GetMap(ResourceHandle handle);
			CPhysicalTemplate* GetPhysicalTemplate(ResourceHandle handle);
			CAbility* GetAbility(ResourceHandle handle);
            CImageAtlas* GetAtlas(ResourceHandle handle);
            CImagePart GetImagePart(ResourceHandle handle, int number);

			void DropResource(const std::string& name);
			void DropResourcesByGid(int gid);
			void Clear();

			void ForceLoadAll(bool fullDebug = true);
            void GetAllStartingWith(const std::string &path, const std::string &type, std::vector<std::string> &out);

			inline int GetErrorsOccured(){ return mErrorsOccured; }
		};
	}
}
#endif //__CRESOURCEMANAGER_H__

