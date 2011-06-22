#ifndef __CTILE_H__
#define __CTILE_H__

// kafel

#include <SFML/System/Vector2.hpp>
#include "SceneManager/CSceneNode.h"
#include <string>

class CImagePart;
class CDisplayable;

namespace Map{
	class CTile {
		public:
            CTile(const std::string &code);
			~CTile();
			void InitializeTile(std::string &imageFile, int number = -1);
			void SetPosition( float x, float y );
			void SetVisible(bool visible);

            inline const std::string &GetCode(){ return mCode; }
		protected:
			CDisplayable * mDisplayable;
            std::string mCode;
	};
}

#endif /*__CTILE_H__*/

