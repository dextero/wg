#ifndef __CIMAGEATLAS_H__
#define __CIMAGEATLAS_H__

#include "IResource.h"
#include <SFML/Graphics/Image.hpp>
#include "CImagePart.h"
#include <assert.h>

namespace System
{
	namespace Resource
	{
        class CResourceManager;
        class CImage;

        class CImageAtlas : public IResource
        {
        private:
            friend class CResourceManager;

            std::vector<sf::IntRect> mOffsets;
            CImage *mAtlas;
            std::string mName;

            bool Load(std::map<std::string,std::string>& argv);
            void Drop();
        public:
            CImageAtlas() : mAtlas(NULL), mName("?") {}
            
            CImage *GetImage() { return mAtlas; }
            void SetImage(CImage *img) { mAtlas = img; }

            CImagePart GetImage(int which) const { assert(mAtlas != NULL); return CImagePart(mAtlas, mOffsets[which]); }
            CImagePart operator[](int which) const { return GetImage(which); }
            const sf::IntRect &GetOffset(int which) const { return mOffsets[which]; }
            int GetRectCount(){ return (int)mOffsets.size(); }

            const std::string& GetAtlasName() const { return mName; }

            std::string GetType();
        };
	}
}

#endif //__CIMAGEATLAS_H__//

