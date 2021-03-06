#ifndef __CIMAGE_H__
#define __CIMAGE_H__

#include "IResource.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace System
{
	namespace Resource
	{
        class CImageAtlas;

		class CImage: public IResource, public sf::Image
		{
		private:
			CImage* mNormalMap;
            sf::IntRect mRect;
            CImageAtlas *mAtlas;
            bool notified;
            static bool metaNotified;

            void MetaNotify() const;
		public:
            CImage();

			inline void SetNormalMap(CImage* nm)	{ mNormalMap = nm; }
			inline CImage* GetNormalMap() const		{ return mNormalMap; }
            void LoadAtlas(CImageAtlas *atlas);
            const sf::IntRect &GetRect(int number) ;

			bool Load(std::map<std::string,std::string>& argv);
			void Drop();
			std::string GetType();

			static bool dontLoadDataHack;
			static bool smoothEnabled;
		};
	}
}

#endif //__CIMAGE_H__

