#include "CLogic.h"
#include "CPhysicalManager.h"
#include "CPhysical.h"
#include "CPlayerManager.h"
#include "CPlayer.h"
#include "Collisions.h"
#include "CBestiary.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Console/CConsole.h"
#include "../Commands/LogicCommands.h"
#include "../Commands/CCommands.h"
#include "../Map/CMapManager.h"
#include "../Utils/Maths.h"
#include "../Utils/StringUtils.h"
#include "../Utils/FileUtils.h"
#include "../Utils/CClock.h"
#include "../Input/CBindManager.h"
#include "../Input/CPlayerController.h"
#include "Factory/CPlayerTemplate.h"
#include "Conditions/CCondition.h"
#include "Conditions/CConditionManager.h"
#include "../Logic/Abilities/CAbilityTree.h"
#include "../Logic/Quests/CQuestManager.h"
#include "../Editor/CEditor.h"
#include "../GUI/Cutscene/CCutscenePlayer.h"
#include "../GUI/Dialogs/CDialogDisplayer.h"
#include "../GUI/Messages/CMessageSystem.h"
#include "../Utils/ToxicUtils.h"
#include "../Rendering/Effects/CGraphicalEffects.h"
#include "../GUI/CRoot.h"
#include "../GUI/CHud.h"
#include "../Audio/CAudioManager.h"
#include "Boss/CBossManager.h"

#ifdef __EDITOR__
#   include "MapObjects/CRegion.h"
#endif

#include <cstdio>
#include <fstream>
#include <sstream>

template<> CLogic* CSingleton<CLogic>::msSingleton = 0;

CLogic::CLogic() :
    mState(L"main-menu"),
	mSaveGameStr(""),
    mDifficultyFactor(1.f),
    mScore(0u),
    mArcadeMode(false)
{
    fprintf( stderr, "CLogic::CLogic()\n" );

    gGame.AddFrameListener( this );
    gGame.AddKeyListener( this );
    gGame.AddMouseListener( this );

    new CPhysicalManager();
    gCommands.RegisterCommands(LogicCommands);

    // przywracanie stanu bestiariusza
	gBestiary.Load(gGameOptions.GetUserDir() + "/bestiary.save");

	mMenuScreens.Show( L"main-menu" );
	mGameScreens.HideAll();

    mCond = new CCondition();
    mCond->SetType(CCondition::ctClear);

    mDifficultyLevels.insert(std::pair<float, unsigned>(1.f,  0u));
    mDifficultyLevels.insert(std::pair<float, unsigned>(1.2f, 1u));
    mDifficultyLevels.insert(std::pair<float, unsigned>(1.5f, 2u));
    mDifficultyLevels.insert(std::pair<float, unsigned>(2.f,  3u));
    mDifficultyLevels.insert(std::pair<float, unsigned>(3.f,  4u));
}

CLogic::~CLogic()
{
    fprintf( stderr, "CLogic::~CLogic()\n" );
    delete CPhysicalManager::GetSingletonPtr();
}

void CLogic::FrameStarted( float secondsPassed )
{
#ifdef __EDITOR__
    UpdateRegions(secondsPassed);
#endif

	if ( !gGame.GetFreezed() )
		UpdatePhysicals( secondsPassed );

	if ( mState == L"playing" )
	{
		mGameScreens.UpdateHud(secondsPassed);

		if ( mCurrentScreen == L"abilities0" )		mGameScreens.UpdateAbilitiesTextboxes(0);
		else if ( mCurrentScreen == L"abilities1" )	mGameScreens.UpdateAbilitiesTextboxes(1);
	}
    else if ( mState == L"death" )
    {
        static float secondsToStateChange = 3.0;
        secondsToStateChange -= secondsPassed;
        if ( secondsToStateChange < 0 )
        {
            // dex: save przeniesione do CPlayer::Kill (automatyczny
            // zapis po smierci jest, ale postac w save dalej istnieje)
            mState = L"death-ready-to-restart";
            gGame.SetFreezed(true);
			mGameScreens.HideAll();
			mGameScreens.Show( L"game-over" );
            secondsToStateChange = 3.0;
        }
	}
}

void CLogic::MousePressed( const sf::Event::MouseButtonEvent &e )
{
    //CheckRestart();
}

void CLogic::KeyPressed( const sf::Event::KeyEvent &e )
{
	/* zarzadzanie planszami gracza - dzienniki etc. */

	if ( mState == L"playing" )
	{
		std::wstring newScreen = mCurrentScreen;

        if (e.Code == sf::Key::Escape)
        {
            if (mCurrentScreen != L"")          { newScreen = L""; }
            else
            {
                if (gGame.GetConsoleOpened())
                    gConsole.SetVisible(false);
                else
                {
                    gCommands.ParseCommand(L"capture-screen");
                    gGame.SetPause(true);
                    mMenuScreens.Show(L"pause-menu");
                    mGameScreens.Hide(L"hud");
                    SetState(L"pause-menu");
                }
            }
            if (gDialogDisplayer.GetDisplaying())
                gDialogDisplayer.ExitDialog();
        }
        // ekrany pomocy
        else if (gBindManagerByPlayer(0) &&
            (gBindManagerByPlayer(0)->GetKeyBindings().find("Help") != gBindManagerByPlayer(0)->GetKeyBindings().end()) &&
            e.Code == gBindManagerByPlayer(0)->GetKeyBindings().find("Help")->second)
        {
			if ( mCurrentScreen != L"controls0" )	{ newScreen = L"controls0"; }
			else									{ newScreen = L""; }
        }
        else if (gPlayerManager.GetPlayerByNumber(1) && gBindManagerByPlayer(1) &&
            (gBindManagerByPlayer(1)->GetKeyBindings().find("Help") != gBindManagerByPlayer(1)->GetKeyBindings().end()) &&
            e.Code == gBindManagerByPlayer(1)->GetKeyBindings().find("Help")->second)
        {
			if ( mCurrentScreen != L"controls1" )	{ newScreen = L"controls1"; }
			else									{ newScreen = L""; }
        }
        // ekrany umiejetnosci
        else if (gBindManagerByPlayer(0) &&
            (gBindManagerByPlayer(0)->GetKeyBindings().find("Abilities") != gBindManagerByPlayer(0)->GetKeyBindings().end()) &&
            e.Code == gBindManagerByPlayer(0)->GetKeyBindings().find("Abilities")->second)
        {
			if ( mCurrentScreen != L"abilities0" )	{ newScreen = L"abilities0"; }
			else									{ newScreen = L""; }
        }
        else if (gPlayerManager.GetPlayerByNumber(1) && gBindManagerByPlayer(1) &&
            (gBindManagerByPlayer(1)->GetKeyBindings().find("Abilities") != gBindManagerByPlayer(1)->GetKeyBindings().end()) &&
            e.Code == gBindManagerByPlayer(1)->GetKeyBindings().find("Abilities")->second)
        {
			if ( mCurrentScreen != L"abilities1" )	{ newScreen = L"abilities1"; }
			else									{ newScreen = L""; }
        }


		if ( mCurrentScreen != newScreen )
		{
			mGameScreens.Hide( mCurrentScreen );
			mGameScreens.Show( mCurrentScreen = newScreen );
            gGUI.ShowCursor(newScreen != L"");
		}

		if (gGame.GetFreezed() != (newScreen != L""))
			System::Input::CBindManager::ReleaseKeys();

        if (!gDialogDisplayer.GetDisplaying() && gCutscenePlayer.GetState() != GUI::CUTSCENE_PLAYING)
		    gGame.SetFreezed( newScreen != L"" );
	}
    else if (mState == L"editor")
    {
        // olejmy menu - jest konsola
        switch (e.Code)
        {
        case sf::Key::Escape:
            gConsole.SetVisible(!gGame.GetConsoleOpened());
            break;
        default:
            break;
        }
    }
    else if (e.Code == sf::Key::Escape)
    {
        if (mMenuScreens.IsShowing())
            mMenuScreens.ShowPrevious();
        if (!mMenuScreens.IsShowing() && GetState() == L"pause-menu")
			ReturnToGame();
	}


	if (e.Code == sf::Key::F10)
		gCommands.ParseCommand( L"print-screen" );
}

void CLogic::UpdatePhysicals( float secondsPassed )
{
	const std::vector< CPhysical* > &physicals = gPhysicalManager.GetPhysicals();

    for ( unsigned int i = 0; i < physicals.size() ; ++i )
        physicals[i]->Update( secondsPassed );

    Collisions::CheckAllCollisions();
}

const std::wstring& CLogic::GetState()
{
    return mState;
}

void CLogic::SetState( const std::wstring& newValue )
{
    mState = newValue;

    // dex: klikniecie 'kontynnuj' z menu pauzy nie pokazywalo hudow
    if (mState == L"playing")
        mGameScreens.Show(L"hud");
}

void CLogic::StartNewGame( const std::wstring& startFile )
{
    gGame.SetPause(false);
    gGame.SetFreezed(false);

    // dopisujac czyszczenie czegokolwiek - dopisz tez w LoadGame
    gGraphicalEffects.ClearEffects();
    gMessageSystem.Clear();
    gConditionManager.Clear();
    
    // czyszczenie slotow
    for (unsigned int player = 0; player < gPlayerManager.GetPlayerCount(); ++player)
        for (unsigned int i = 0; i < ABI_SLOTS_COUNT; ++i)
            mGameScreens.SetSlotAbility(player, i, NULL);

	gCommands.ParseCommand( std::wstring(L"exec ") + startFile );
    if (ToxicUtils::isGameInCrimsonMode) {
        gCommands.ParseCommand(L"exec crimson-mode");
    }
	mState = L"playing";
	mMenuScreens.HideAll();
    mMenuScreens.ClearPreviousMenusList();
	mGameScreens.Show( L"hud" );

	//for (unsigned int i = 0; i < gPlayerManager.GetPlayerCount(); ++i){
	//	this->GetHud(i)->ResetSequences();
	//}
}

void CLogic::CheckRestart()
{
    if ( mState == L"death-ready-to-restart" )
    {
        ExitToMainMenu();
    }
}

void CLogic::ExitToMainMenu()
{
    // zmiana muzyki na ta, ktora powinna byc w menu
    Audio::CPlayList playlist;
    if (playlist.Load(L"data/music/main-menu.xml"))
        gAudioManager.GetMusicPlayer().SetPlayList(playlist);

    Clear();
    gCommands.ParseCommand(L"capture-screen");
    mState = L"main-menu";
	mGameScreens.HideAll();
    mMenuScreens.ClearPreviousMenusList();
	mMenuScreens.Show( L"main-menu" );
}

void CLogic::StartEditor()
{
    Clear();
    gEditor.Init();
    mState = L"editor";
    mMenuScreens.HideAll();
    mGameScreens.Show(L"editor");
}

void CLogic::Clear()
{
	gCutscenePlayer.StopCutscene();
	gPhysicalManager.Clear(true);
	gMapManager.SetCurrentMapAsVisited();
}

void CLogic::SetGameThumbnail(const sf::Image & thumbnail)
{
    mGameThumbnail = thumbnail;
}

void CLogic::PrepareToSaveGame(bool savePlayerPos)
{
	if (mState == L"death")
		return;

    if (mState == L"editor")
	{
        mSaveGameStr = "";
        return;
    }

    if (gMapManager.GetCurrent() == NULL)
        return;

	float xp = 0.0f;
	std::ostringstream ss("");
    ss << "#" << gClock.GenerateTimestamp() << "\n";
	ss << "exec pre-new-game\n";

    // to MUSI byc przed stawianiem graczy na mape
    ss << "set-arcade-mode " << (mArcadeMode ? "on" : "off") << "\n";

	for (unsigned i = 0; i < gPlayerManager.GetPlayerCount(); i++)
	{
		if (CPlayer* player = gPlayerManager.GetPlayerByNumber(i))
		{
			fprintf(stderr, "skillpoints = %d\n", player->GetSkillPoints());
			xp = player->GetTotalXP();

			ss << "new-player " << i << "\n";
			ss << "add-skill-points player" << i << " " << (int)(player->GetSkillPoints()) << "\n";
			const std::vector<CAbilityTree*> *trees = player->GetBoundAbilityTrees();
			const std::vector<std::string> *treeCodes = player->GetAbiCodes();
            std::vector<std::string> activatedAbilities;
			for (size_t t = 0; t < trees->size();t++){
				std::string tname = treeCodes->at(t);
				const std::vector<SAbilityNode> &nodes = trees->at(t)->GetAbilityNodes();
				std::vector<SAbilityInstance> *abiIntances = player->GetAbilityPerformer().GetAbilities();
				for (size_t n = 0; n < nodes.size(); n++){
					int level = 0;
					for (size_t a = 0; a < abiIntances->size(); a++){
						if (abiIntances->at(a).abiNode == &nodes[n]){
							level = abiIntances->at(a).level - nodes[n].startLevel;
                            // aktywna umiejka
                            if (nodes[n].sharedTrigger && abiIntances->at(a).triggerEnabled)
                                activatedAbilities.push_back("set-active-ability " + StringUtils::ToString<unsigned int>(i) + " " + tname + " " + nodes[n].code + "\n");
							break;
						}
					}
					if (level > 0)
						ss << "force-buy-ability player" << i << " " << tname << " " << nodes[n].code << " " << level << "\n";
				}
			}
            // aktywne umiejki musza byc na koncu, bo inaczej wykupywane nastepne nadpisuja ustawione
            for (size_t a = 0; a < activatedAbilities.size(); ++a)
                ss << activatedAbilities[a];

            // pasek umiejek (na razie nie usuwam tyh zsharedTriggerowanych)
            for (size_t slot = 0; slot < ABI_SLOTS_COUNT; ++slot)
            {
                // BRZYDKOOOOO
                SAbilityInstance* abiInstance = player->GetAbilityPerformer().GetAbility((unsigned int)player->GetController()->GetSelectedAbilities()[slot]);
                if (!abiInstance)
                    break;
                const SAbilityNode* abi = abiInstance->abiNode;
                for (size_t tree = 0; tree < player->GetBoundAbilityTrees()->size(); ++tree)
                    if (player->GetBoundAbilityTrees()->at(tree)->GetAbilityNodes()[player->FindAbilityInTree(tree, abi->code)].ability == abi->ability)
                    {
                        ss << "set-slot-ability " << i << " " << slot << " " << player->GetAbiCodes()->at(tree).c_str() << " " << abi->code.c_str() << "\n";
                        break;
                    }
            }
		}
	}
	ss << "add-xp " << xp << " ignore-skill-points silent\n";
    ss << "set-difficulty-factor " << mDifficultyFactor << "\n";
    ss << "set-score " << mScore << "\n";
	ss << "preload-map " << gMapManager.GetCurrent()->GetFilename() << " entry false\n";

    // flagi do questow
    for (std::map<std::wstring, bool>::const_iterator i = gQuestManager.GetFlags().begin(); i != gQuestManager.GetFlags().end(); ++i)
        if (i->second)
            ss << "define-quest-flag " << StringUtils::ConvertToString(i->first) << "\n";

    // physicale
    const std::vector< CPhysical *>& physicals = gPhysicalManager.GetPhysicals();
    for (std::vector< CPhysical * >::const_iterator it = physicals.begin();
         it != physicals.end(); ++it)
    {
        // nie ozywiajmy martwych, to irytujace, gdy po loadzie
        // boss znowu zyje, chociaz go ubilismy :)
        if ((*it)->IsDead())
            continue;

		std::string filename;
        if ((*it)->GetCategory() == PHYSICAL_MONSTER ||
            (*it)->GetCategory() == PHYSICAL_LAIR ||
            (*it)->GetCategory() == PHYSICAL_OBSTACLE ||
			(*it)->GetCategory() == PHYSICAL_ITEM)
        {
            filename = (*it)->GetTemplate()->GetFilename();
        }
        else
            continue;


        ss << "spawn-physical-rot " << filename << " " << StringUtils::ConvertToString((*it)->GetUniqueId()) << " " 
			<< (*it)->GetPosition().x << " " << (*it)->GetPosition().y << " " << (*it)->GetRotation() << "\n";
    }

    // zyjace bossy
    ss << gBossManager.SerializeLivingBosses();

    // to na razie nie..
    /*CEnemy* boss = gBossManager.GetBoss();
    if (boss)
        ss << "start-boss-fight " << StringUtils::ConvertToString(boss->GetUniqueId()) << "\n";*/

    // pozycje graczy
    if (savePlayerPos)
        for (unsigned i = 0; i < gPlayerManager.GetPlayerCount(); i++)
	        if (CPlayer* player = gPlayerManager.GetPlayerByNumber(i))
                ss << "set-physical-position player" << i << " " << player->GetPosition().x << " " << player->GetPosition().y << "\n";

	ss << "exec post-new-game\n";
    //tox, 27 maja: todo: uwzgledniac crimson-mode
	mSaveGameStr = ss.str();
}

void CLogic::SaveGame(const std::string & name, bool thumbnail, bool savePlayerPos){
    // dex: wrzucam wywolanie tutaj
    PrepareToSaveGame(savePlayerPos);

	remove(name.c_str());
	FILE *outfile = fopen(name.c_str(), "w");
    if ( !outfile )
		return;
	fputs(mSaveGameStr.c_str(), outfile); 
	fclose(outfile);

    if (thumbnail)
    {
        std::string thumbnailName =
            std::string(name.begin(), name.end() - 5) + std::string(".jpg");
        if (!mGameThumbnail.SaveToFile(thumbnailName))
        {
            gConsole.Printf( L"Unable to save game thumbnail: %s", thumbnailName.c_str() );
        }
    }
}

void CLogic::LoadGame(const std::string & name)
{
	if (!CanLoadGame(name)) return;

    gGame.SetPause(false);
    gGame.SetFreezed(false);

    // dopisujac czyszczenie czegokolwiek - dopisz tez w NewGame
    gGraphicalEffects.ClearEffects();
    gMessageSystem.Clear();
    gConditionManager.Clear();

    gCommands.ParseCommand( L"exec " + StringUtils::ConvertToWString(name) );

    for (size_t i = 0; i < gPlayerManager.GetPlayerCount(); ++i)
    {
        // dex: powinno wystarczyc, chocby playera stratowal kon i przejechal walec, pare razy
        gPlayerManager.GetPlayerByNumber(i)->GetStats()->DoHeal(gPlayerManager.GetPlayerByNumber(i)->GetStats()->GetBaseAspect(aMaxHP));
    }

	mState = L"playing";
	mMenuScreens.HideAll();
    mMenuScreens.ClearPreviousMenusList();
	mGameScreens.HideAll();
	mGameScreens.Show( L"hud" );
}

bool CLogic::CanLoadGame(const std::string & name){
	return FileUtils::FileExists(name);
}

std::wstring CLogic::GetGameInfo( const std::string & name )
{
    FILE *infile = fopen(name.c_str(), "rb");
    if (!infile)
        return StringUtils::ConvertToWString(name);

    char buf[4096];
	std::string info(fgets(buf, 4096, infile) != NULL ? buf : "");

    if (info.length() >= 13)
    {
        info.insert(13, 1, ':');
        info.insert(11, 1, ':');
        info.insert( 9, 1, ' ');
        info.insert( 7, 1, '.');
        info.insert( 5, 1, '.');
        info.erase(0, 1);
    }

    return StringUtils::ConvertToWString(info);
}

GUI::CHud* CLogic::GetHud(unsigned int player)
{
    return mGameScreens.GetHud(player);
}

bool CLogic::ShowLoading(bool show, bool instantHide){
	if(show){
		mMenuScreens.Show(L"loading");
        return true;
	} else {
		return mMenuScreens.HideLoadingScreen(!instantHide);
	}
}
void CLogic::ReturnToGame()
{
	mMenuScreens.HideAll();
    mMenuScreens.ClearPreviousMenusList();
	mGameScreens.Show(mCurrentScreen = L"");
	mGameScreens.Show(L"hud");
	gGame.SetPause(false);

	SetState(L"playing");
}

void CLogic::SetDifficultyLevel(unsigned level)
{
    for (std::map<float, unsigned>::iterator i = mDifficultyLevels.begin(); i != mDifficultyLevels.end(); ++i)
        if (i->second == level)
        {
            mDifficultyFactor = i->first;
            return;
        }
}

void CLogic::ScoreGained(unsigned score)
{
    mScore += (unsigned)(score * mDifficultyFactor);
}

#ifdef __EDITOR__
    void CLogic::UpdateRegions(float secondsPassed)
    {
        const std::vector< CPhysical* > &physicals = gPhysicalManager.GetPhysicals();

        for ( unsigned int i = 0; i < physicals.size() ; ++i )
            if (dynamic_cast<CDynamicRegion*>(physicals[i]))
                physicals[i]->Update(secondsPassed);
    }
#endif
