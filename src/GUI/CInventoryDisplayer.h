/* GUI - pojedyncze drzewko umiejetnosci */

#ifndef _CINVENTORYDISPLAYER_H_
#define _CINVENTORYDISPLAER_H_

#include "CGUIObject.h"
#include <string>

namespace System{
    namespace Resource{
        class CImage;
    }
}

namespace GUI
{
    class CWindow;
    class CImageBox;
    class CItemSlotsBar;

	class CInventoryDisplayer : public CGUIObject
	{
	public:
        void ForceReload() { mForceReload = true; }
        void UpdatePlayerData();
	private:
        static const unsigned int SLOTS_COUNT = 4;
        
		friend class CGUIObject;
		CInventoryDisplayer( const std::string& name, CGUIObject* parent, unsigned int zindex, int player );
		virtual ~CInventoryDisplayer();
		virtual void UpdateSprites( float secondsPassed );

        int mPlayer;
        bool mForceReload;
        
        CWindow* mBackground;
        CItemSlotsBar* mBar;

        friend class CAbilitySlot;
	};
};

#endif// _CINVENTORYDISPLAER_H_

