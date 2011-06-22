#ifndef __CLOGIC_H__
#define __CLOGIC_H__

#include "../GUI/CMenuScreens.h"
#include "../GUI/CGameScreens.h"
#include "../Utils/CSingleton.h"
#include "../IFrameListener.h"
#include "../IKeyListener.h"
#include "../IMouseListener.h"

#include "SFML/Graphics/Image.hpp"
#include <string>
#include <map>

#define gLogic CLogic::GetSingleton()

class CCondition;

class CLogic : public CSingleton< CLogic >, IFrameListener, IKeyListener, IMouseListener
{
public:
    CLogic();
    virtual ~CLogic();

    virtual void FrameStarted( float secondsPassed );
    virtual void KeyPressed( const sf::Event::KeyEvent &e );
    virtual void MousePressed( const sf::Event::MouseButtonEvent &e );
	virtual bool FramesDuringPause(EPauseVariant pv) { return pv==pvLogicOnly; };

	void UpdatePhysicals( float secondsPassed );

    const std::wstring& GetState();
    void SetState( const std::wstring& newValue );

	void StartNewGame( const std::wstring& startFile=L"new-game" );	/* "new-game" | "new-game-hs" (hot-seat) */
	void CheckRestart();
    void ExitToMainMenu();
    void StartEditor();
    void Clear();

    void SaveGame(const std::string & name, bool thumbnail, bool savePlayerPos = true);
	void LoadGame(const std::string & name);

    void SetGameThumbnail(const sf::Image & thumbnail);
	void PrepareToSaveGame(bool savePlayerPos = true);
	bool CanLoadGame(const std::string & name);
    std::wstring GetGameInfo(const std::string & name);

    GUI::CHud* GetHud(unsigned int player);

    GUI::CGameScreens* GetGameScreens() { return &mGameScreens; };
    GUI::CMenuScreens* GetMenuScreens() { return &mMenuScreens; };

    void SetMenuScreensFlag(unsigned int flag, bool unset = false) { mMenuScreens.SetFlag(flag, unset); };

	bool ShowLoading(bool show, bool instantHide = true);    // zwraca true, jesli jeszcze pokazuje, false, gdy juz calkiem ukryty
	void ReturnToGame();

    inline void SetDifficultyFactor(float factor) { mDifficultyFactor = factor; }
    inline float GetDifficultyFactor() { return mDifficultyFactor; }
    void SetDifficultyLevel(unsigned level);
    unsigned GetDifficultyLevel() { return DifficultyFactorToLevel(mDifficultyFactor); }
    inline unsigned DifficultyFactorToLevel(float factor) { return (mDifficultyLevels.find(factor) == mDifficultyLevels.end() ? (unsigned)-1 : mDifficultyLevels[factor]); }

    inline void SetScore(unsigned score) { mScore = score; }
    inline unsigned GetScore() { return mScore; }
    void ScoreGained(unsigned score);

    // arcade mode == 2x mana, 0.5x mana-regen
    inline void SetArcadeMode(bool arcade = true) { mArcadeMode = arcade; }
    inline bool IsInArcadeMode() { return mArcadeMode; }

#ifdef __EDITOR__
    void UpdateRegions(float secondsPassed);
#endif

private:
    std::wstring mState;

	GUI::CMenuScreens mMenuScreens;
	GUI::CGameScreens mGameScreens;
	std::wstring mCurrentScreen;

    CCondition *mCond;
	std::string mSaveGameStr;
    sf::Image mGameThumbnail;
    
    float mDifficultyFactor;
    std::map<float, unsigned> mDifficultyLevels;
    unsigned mScore;
    bool mArcadeMode;
};

#endif//__CLOGIC_H__
