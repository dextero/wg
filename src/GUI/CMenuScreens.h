/* Menu glowne */

#ifndef _MENU_SCREENS_
#define _MENU_SCREENS_

#include <string>
#include <vector>
#include <stack>
#include "CScreens.h"
#include "../Utils/StringUtils.h"

// flagi menu screenow
#define MENUSCREEN_SHOW_EDITOR  1

namespace GUI
{
	class CGUIObject;
	class CTextArea;
	class CKeyReader;
	class CSaveScreen;

    class CMenuScreens : public CScreens
	{
	public:
		void Show( const std::wstring& menu ); // "main-menu" | "new-game-options" | "options" | "save-screen" | 
											   // "load-screen" | "loading"
        void ShowPrevious();
        bool IsShowing() { return mPrevMenus.size() > 0; };
        void ClearPreviousMenusList() { while (!mPrevMenus.empty()) mPrevMenus.pop(); };
		void HideAll();

        void InitAll();

		void InitMainMenu();

        void InitPauseMenu();

		void InitNewGameOptions();
        void InitChooseControlsMenu();

		void InitSaveScreen();
		void UpdateSaveScreen(bool save);

		void InitOptions();
		void UpdateOptions();
		void SaveOptions();
		void CancelOptions();

		void InitBindingOptions();
		void UpdateBindingOptions( int playerNumber ); // tox, 25 luty: powinien byc unsigned, ale mi sie nie chce tego refaktorowac
		void SaveBindingOptions( const std::wstring& playerWStr );

        void InitReadmeScreen();

		void InitLoadingScreen();
		bool HideLoadingScreen(bool fade = false);

        void InitAchievementsScreen();
        void UpdateAchievementsScreen();

        void InitBestiaryScreen();
        void UpdateBestiaryScreen();
        void UpdateBestiaryTooltip(const std::wstring& templ);

		void ExitGame();

        void SetFlag(unsigned int flag, bool unset);

		CMenuScreens();
		~CMenuScreens();

        void ShowBindingOptions(const std::wstring & menu);

        //void UpdateDifficultyString(void* control, unsigned state);

	protected:
		inline std::wstring GenVideoMode( int width, int height, int bpp ) {
			return StringUtils::ToWString(width) + L" x " + StringUtils::ToWString(height) + L" x " + StringUtils::ToWString(bpp);
		}

		static const int PLAYERS_CNT = 2;

		struct keyBinding
		{
			std::string name;
			CTextArea* textArea;
			CKeyReader* keyReader;
		};

		CGUIObject * mMainMenu;
        CGUIObject * mPauseMenu;
		CGUIObject * mNewGameOptions;
        CGUIObject * mChooseControlsMenu;
		CGUIObject * mOptions;
		CGUIObject * mBindingOptions[PLAYERS_CNT];
        CGUIObject * mReadmeScreen;
        CSaveScreen * mSaveScreen;
		CGUIObject * mLoadingScreen;
        CGUIObject * mAchievementsScreen;
        CGUIObject * mBestiaryScreen;
		std::stack<std::wstring> mPrevMenus;

        // co wlaczyc po wyborze sterowania (pierwsze uruchomienie)
        std::wstring mNewGameMode;

		std::vector<keyBinding> mKeyBindings[PLAYERS_CNT];

        unsigned int mFlags;
        bool mSave;

        void StoreOptions(); // user pozmienial opcje, ale jeszcze nie kliknal 'save&exit'
        void RestoreOptions();

        void ChooseControls(const std::wstring& controls);
        void TryStartGame(const std::wstring& runMode);

        //void UpdateArcadeMode();    // sprawdza checkboxa i ew. przyspiesza gre

        //ustawienia 'dirty', czyli jeszcze nie zapisane, ale juz ruszane przez usera
        int tmpWidth;
        int tmpHeight;
        int tmpBitsPerPixel;
        bool tmpIsFullscreen;
        bool tmpIsVSync;
        bool tmp3DSound;
        float tmpSoundVolume;
        float tmpMusicVolume;
        std::wstring tmpControls0;
        std::wstring tmpControls1;


	};
};

#endif

