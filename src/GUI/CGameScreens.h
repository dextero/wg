/* Plansze w grze */

#ifndef _GAME_SCREENS_
#define _GAME_SCREENS_

#include "CScreens.h"
#include <string>

class CAbility;

namespace GUI
{
	class CGUIObject;
	class CWindow;
    class CImageBox;
    class CSaveScreen;
    class CEditorScreens;
	class CHud;
	class CCompass;
    class CBossHud;
    class CAbiSlotsBar;

    class CGameScreens : CScreens
	{
	public:
		void Show( const std::wstring& menu ); // "hud[0-1]" | "abilities[0-1]" | "controls[0-1]" | "game-over" | "editor"
                                               // "load-game"
        void ShowPrevious();
		void Hide( const std::wstring& menu );
		void HideAll();

		void ReturnToGame();
        void ResetCompass();

		void InitHud( unsigned playerNumber );
		void InitHuds();
		void UpdateHuds(float dt);

		void InitAbilities( unsigned playerNumber );
		void UpdateAbilities( unsigned playerNumber );
		void UpdateAbilitiesTextboxes( unsigned playerNumber );
        void SetActiveAbilityScreen( const std::wstring& name );
		void SetSlotAbility(unsigned int player, unsigned int slot, CAbility* abi);

        void InitControlListing( unsigned playerNumber );
        void UpdateControlListing( unsigned playerNumber );

		void InitGameOver();

        CHud* GetHud(unsigned int player);
        CAbiSlotsBar* GetAbiBar(unsigned int player);
        CGUIObject* GetAbilities(unsigned int player);        

		CGameScreens();
		~CGameScreens();
	protected:
		CHud * mHud[2];
        CAbiSlotsBar * mAbiSlotsBar[2];
		CCompass * mCompass;
        CBossHud * mBossHud;
		CGUIObject * mAbilities[2];
        std::wstring mActiveAbilitiesTab[2];
		CWindow	   * mAbiHelp[2];
		CGUIObject * mControls[2];
		CGUIObject * mGameOver;
        CSaveScreen * mSaveScreen;
        CEditorScreens * mEditorScreens;
	};
};

#endif

