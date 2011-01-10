#include "CGameOptions.h"
#include "CGame.h"
#include "Utils/StringUtils.h"
#include "Utils/CXml.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "Rendering/CCamera.h"
#include "Audio/CAudioManager.h"
#include "Input/CBindManager.h"
#include "Input/CInputHandler.h"
#include "Input/CMouseCaster.h"
#include "Logic/Achievements/CAchievementManager.h"
#include "Logic/Quests/CQuestManager.h"
#include "Logic/CPlayerManager.h"
#include "Logic/CLogic.h"
#include "GUI/Localization/CLocalizator.h"

#ifdef PLATFORM_MACOSX
/* OSX */ extern "C" int AskForFullscreen_OSX(const wchar_t * title, const wchar_t * message, int maxw, int maxh);
#else
#ifdef PLATFORM_LINUX
/* GNU */
#else
/* Win */ bool AskForFullscreen_Win32(const wchar_t * title, const wchar_t * message, int maxw, int maxh)
#ifndef __EDITOR__
    ;
#else // ifdef __EDITOR__
    { return false; }
#endif
#endif
#endif
									 
bool AskForFullscreen(unsigned int &maxw, unsigned int &maxh)
{
	sf::VideoMode videoMode;
	
	maxw = 800;
	maxh = 600;

    //tox, 10 sie 2010: na niektorych sprzetach GetModesCount zwracal mi zero
	for ( size_t i = 0; i < sf::VideoMode::GetModesCount(); i++ )
	{
		videoMode = sf::VideoMode::GetMode( i );
		
		if (videoMode.Width > maxw) { maxw = videoMode.Width; maxh = videoMode.Height; }
	}

	
#ifdef PLATFORM_MACOSX
	
	// OSX	
	const std::wstring titleStr = gLocalizator.GetText("FULLSCREEN_TITLE");
	const std::wstring messageStr = gLocalizator.GetText("FULLSCREEN_MESSAGE");
	return (bool)AskForFullscreen_OSX(titleStr.c_str(), messageStr.c_str(), maxw, maxh);
	
#else
#ifdef PLATFORM_LINUX

	// Linux
	return true;

#else
	
	// Windows
	const std::wstring titleStr = gLocalizator.GetText("FULLSCREEN_TITLE");
	const std::wstring messageStr = gLocalizator.GetText("FULLSCREEN_MESSAGE");
	return AskForFullscreen_Win32(titleStr.c_str(), messageStr.c_str(), maxw, maxh);

#endif
#endif
}

template<> CGameOptions* CSingleton<CGameOptions>::msSingleton = 0;

bool FileExists( const std::string& filename )
{
	std::ifstream f( filename.c_str(), std::ios::in );
	f.close();
	return ( !f.fail() );
}

bool CGameOptions::LoadOptions()
{
	unsigned int configVer = 0;
	unsigned int userConfigVer = 0;

	if ( FileExists( "data/config.xml" ) )
	{
		CXml xml( "data/config.xml", "root" );
		configVer = xml.GetInt( NULL, "version" );
	}

	bool is_first_game = false;
	
	if ( FileExists( GetUserDir() + "/config.xml" ) )
	{
		CXml xml( GetUserDir() + "/config.xml", "root" );
		userConfigVer = xml.GetInt( NULL, "version" );
	}
    else
    {
        // nie ma user/config, stworz plik first_game, zeby gracz wybral sobie sterowanie..
        std::ofstream first_game( (GetUserDir() + "/first_game").c_str() );
        first_game.close();
		is_first_game = true;
    }

	if ( configVer + userConfigVer == 0 )
	{
		fprintf(stderr, "Cannot open config file\n");
		return false;
	}

	CXml xml( configVer > userConfigVer ? "data/config.xml" : gGameOptions.GetUserDir() + "/config.xml", "root" );

    // poczatek ladowania locale
    mLocaleLang = xml.GetString("locale","lang");
    std::string localeDir = "data/locale/";
    boost::filesystem::directory_iterator di(localeDir), di_end;

    // wszystko z data/locale
    for (; di != di_end; ++di)
    {
		if (di->leaf() == ".svn")
			continue;

        std::string file = localeDir + di->leaf();
        if (!boost::filesystem::is_directory(file))
            gLocalizator.Load(file);
    }

    // wszystko z data/locale/[jezyk], rekurencyjnie
    std::vector<std::string> dirs;
	dirs.push_back(localeDir + mLocaleLang);
	while (dirs.size() > 0)
    {
		std::string dir = dirs[dirs.size()-1];
		dirs.pop_back();

		boost::filesystem::directory_iterator di(dir),dir_end;
		for (; di != dir_end; di++){
			if (di->leaf() == ".svn")
				continue;
			std::string file = dir + "/" + di->leaf();
			if (boost::filesystem::is_directory(file))
				dirs.push_back(file);
			else
				gLocalizator.Load(file);
		}
	}
    // koniec ladowania locale

    // ladowanie achievementow
    if (FileExists(GetUserDir() + "/achievements.xml"))
        gAchievementManager.Load(GetUserDir() + "/achievements.xml");
    else
        gAchievementManager.Load("data/achievements.xml");

    // achievementy - questy
    dirs.clear();
    dirs.push_back("data/plot/achievements");
	while (dirs.size() > 0)
    {
		std::string dir = dirs[dirs.size()-1];
		dirs.pop_back();

		boost::filesystem::directory_iterator di(dir),dir_end;
		for (; di != dir_end; di++){
			if (di->leaf() == ".svn")
				continue;
			std::string file = dir + "/" + di->leaf();
			if (boost::filesystem::is_directory(file))
				dirs.push_back(file);
			else
				gQuestManager.LoadQuest(file);
		}
	}
    // koniec achievementow

    // bestiariusz w CLogic::CLogic, bo wymaga komendy konsolowej..

    gInputHandler.InitControlSets();

	mVersion = xml.GetInt( NULL, "version" );
	mWidth = xml.GetInt( "video", "width" );
	mHeight = xml.GetInt( "video", "height" );
	mBPP = xml.GetInt( "video", "bpp" );
	
	bool fullscreenFlag = ( xml.GetInt("video","fs") != 0 );
	
	if (is_first_game)
	{
		fullscreenFlag = AskForFullscreen(mWidth, mHeight);
		if (!fullscreenFlag)
		{
			mWidth = 800;
			mHeight = 600;
		}
	}
	
	mFullscreen = fullscreenFlag;

	SetVSync( xml.GetInt("video","vsync") != 0 );
    SetSoundVolume( xml.GetFloat("audio","sound",1.0f) );
    SetMusicVolume( xml.GetFloat("audio","music",1.0f) );
    m3DSound = (xml.GetInt("audio", "stereo", 1) != 0);

    // domyslne schematy sterowania
    for ( xml_node<>* node=xml.GetChild(NULL, "controlsScheme"); node; node=xml.GetSibl(node, "controlsScheme") )
    {
        size_t at = AddControlScheme(xml.GetInt(node, "player"), xml.GetString(node, "name"), xml.GetString(node, "desc"), xml.GetString(node, "menuDesc"), xml.GetInt(node, "flags"));
        for (xml_node<>* n=xml.GetChild(node, "keyBinding"); n; n=xml.GetSibl(n, "keyBinding"))
        {
            std::vector<std::pair<std::string, int> > overrides;
            int key = xml.GetInt(n, "key");

            // znajdz pasujacy override, jesli istnieje
            for (xml_node<>* o = xml.GetChild(n, "override"); o; o = xml.GetSibl(o, "override"))
            {
                overrides.push_back(std::pair<std::string, int>(xml.GetString(o, "platform"), xml.GetInt(o, "key")));

                // uwaga: z <override> czyta TYLKO "key", reszte bierze z rodzica!
#if     defined(PLATFORM_MACOSX)
                if (xml.GetString(o, "platform") == "macintosh")
#elif   defined(PLATFORM_LINUX)
                if (xml.GetString(o, "platform") == "linux")
#else //defined(PLATFORM_WINDOWS)
                if (xml.GetString(o, "platform") == "windows")
#endif
                    key = xml.GetInt(o, "key");
            }
            
            // tu pobiera keya z <keyBinding>, nie <override>, zeby to potem z powrotem zapisac
            AddControlSchemeBind( xml.GetInt(node, "player"), at, xml.GetString(n, "name"), xml.GetInt(n, "key"), xml.GetFloat(n, "lock", -1.f), overrides);

            // override idzie tu, do bindManagera. i nie dodajemy do user-specific bindings, bo by bylo duzoo..
            SetKeyBinding( StringUtils::ConvertToString(gLocalizator.Localize(xml.GetString(node, "name"))), xml.GetString(n, "name"), key, xml.GetInt(node, "player"), xml.GetFloat(n, "lock", -2.f), true );
        }
    }

    // czy obaj gracze maja jakikolwiek zestaw klawiszy do wyboru?
    if (mDefControlSchemes[0].size() == 0 || mDefControlSchemes[1].size() == 0)
    {
        // przydalby sie platform-independent MessageBox..
        fprintf(stderr, "WARNING: no control schemes found! Please update data/config.xml!\n");

        // errory drugie, zeby razily w oczy na ekranie opcji
        if (mDefControlSchemes[0].size() == 0)
        {
            AddControlScheme(0, "$CTRL_SET_USER", "$CTRL_SET_USER_DESC", "$CTRL_SET_USER_MENU_DESC", 0);
            AddControlScheme(0, "$ERR", "$ERR_NO_CTRL_SCHEMES", "$ERR_NO_CTRL_SCHEMES", cfShowOnFirstGame);
        }
        if (mDefControlSchemes[1].size() == 0)
        {
            AddControlScheme(1, "$CTRL_SET_USER", "$CTRL_SET_USER_DESC", "$CTRL_SET_USER_MENU_DESC", 0);
            AddControlScheme(1, "$ERR", "$ERR_NO_CTRL_SCHEMES", "$ERR_NO_CTRL_SCHEMES", cfShowOnFirstGame);
        }
    }

    // wybrane sterowanie
	for ( xml_node<>* node=xml.GetChild(NULL,"controls"); node; node=xml.GetSibl(node,"controls") )
        SetControls( xml.GetInt(node,"controls"), xml.GetInt(node,"player",0) );

    // user-specific keybindings
	for ( xml_node<>* node=xml.GetChild(NULL,"keyBinding"); node; node=xml.GetSibl(node,"keyBinding") )
		SetKeyBinding( xml.GetString(node,"controls"), xml.GetString(node,"name"), xml.GetInt(node,"key"), xml.GetInt(node,"player"), xml.GetFloat(node, "lock", -2.f) );
	
    return true;
}

#ifdef PLATFORM_MACOSX

/* MacOSX */
bool CanCreateWindowInFullScreen()
{
    return true;
}

#else

#ifdef PLATFORM_LINUX

/* Linux */
#include <sys/wait.h>
char APP[] = "./bin/check_fullscreen";
bool CanCreateWindowInFullScreen()
{
    int pid,status;
        
    if (!(pid=fork())) {
        fprintf(stderr, "pid=%d\n", pid);
        fprintf(stderr, "pid=%d finish\n", pid);
        int i = execlp(APP, 0);
        fprintf(stderr, "Failed to launch %s, code=%d, aborting\n", APP, i);
        _exit(-1);
    }
    while (pid!=wait(&status)) {
        // do nothing
    }
    fprintf(stderr, "status=%d\n", status);

    return (status == 0);
}

#else // PLATFORM_LINUX

/* Windows */
bool CanCreateWindowInFullScreen()
{
    return true;
}

#endif //PLATFORM_LINUX
#endif //PLATFORM_MACOSX

void CGameOptions::UpdateWindow()
{
#ifndef __EDITOR__
    if(gGame.GetRenderWindow())
    {
        gGame.GetRenderWindow()->Close();
        delete gGame.GetRenderWindow();
    }

    const unsigned int windowMode = sf::Style::Close;
    const unsigned int fullscreenMode = sf::Style::Fullscreen;

    unsigned int mode = (gGameOptions.IsFullscreen() && CanCreateWindowInFullScreen() ) ? fullscreenMode : windowMode;

    gGame.SetRenderWindow(new sf::RenderWindow( 
            sf::VideoMode(gGameOptions.GetWidth(), gGameOptions.GetHeight(), gGameOptions.GetBitsPerPixel()), 
            StringUtils::ConvertToString(gLocalizator.GetText("GAME_TITLE")), mode));

	gGame.GetRenderWindow()->UseVerticalSync( mVSync );
#else
    if (gGame.GetRenderWindow() != NULL)
    {
        mWidth = gGame.GetRenderWindow()->GetWidth();
        mHeight = gGame.GetRenderWindow()->GetHeight();
    }
#endif

	gPlayerManager.MovePlayersToFirstPlayer();
	gCamera.UpdateSettings();
	gCamera.Update( 0.0f );
}

#ifdef PLATFORM_MACOSX
extern "C" const char * CreateDirectoryIfNotExists(const char *dname);
#endif
#if _WIN32
std::wstring GetWindowsUserDir();
#endif

std::string GetUserDir()
{
#if _WIN32
	// Windows
	return CGameOptions::mModDir + "user";
	//return StringUtils::ConvertToString(GetWindowsUserDir());
#else
#ifdef PLATFORM_MACOSX
	// OSX
	return CreateDirectoryIfNotExists((std::string("~/Library/WarsztatGame")+CGameOptions::mModDir).c_str());
#else
	// Linux
	return CGameOptions::mModDir + "user";
#endif
#endif
}

CGameOptions::CGameOptions():
    mVersion(0),
    mWidth(0),
    mHeight(0),
    mBPP(0),
    mFullscreen(false),
    mVSync(true),
    m3DSound(true),
    mSoundVolume(100.f),
    mMusicVolume(100.f)
{
	mUserDir = ::GetUserDir();

    fprintf(stderr, "CGameOptions::CGameOptions(), userDir = %s\n", mUserDir.c_str());
	gInputHandler; /* zapewniamy wykonanie konstruktora CInputHandlera przed zaladowaniem opcji */

}

CGameOptions::~CGameOptions()
{
    fprintf(stderr, "CGameOptions::~CGameOptions()\n");
    SaveOptions();
}

void CGameOptions::SaveOptions()
{
    std::ofstream file( (gGameOptions.GetUserDir() + "/config.xml").c_str() );

	if(!file.is_open()){
		fprintf(stderr,"Error opening %s/config.xml for writing, options not saved!\n", gGameOptions.GetUserDir().c_str());
        return;
	}

    // TODO: dopisac inne opcje ponizej:
    file << "<root type=\"config\" version=\"" << mVersion << "\">\n"
         << "\t<!-- VideoMode Config -->\n"
         << "\t<video width=\""<< mWidth <<"\" height=\""<< mHeight << "\" fs=\"" <<mFullscreen<<"\" bpp=\""<<mBPP<<"\" vsync=\"" <<mVSync<<"\" />\n"
         << "\t<audio sound=\"" << mSoundVolume << "\" music=\"" << mMusicVolume << "\" stereo=\"" << m3DSound << "\" />\n"
		 << "\t<locale lang=\"" << mLocaleLang << "\" />\n\n";

    for ( unsigned p = 0; p < PLAYERS_CNT; ++p )
        for ( unsigned c = 0; c < mDefControlSchemes[p].size(); ++c )
            file << mDefControlSchemes[p][c].Serialize();

	for ( unsigned p = 0; p < PLAYERS_CNT; p++ )
        file << "\t<controls controls=\"" << System::Input::CBindManager::GetBindId(mControls[p], p) << "\" player=\"" << p << "\" />\n";

	file << "</root>" << std::endl;
    file.close();
}

void CGameOptions::SetVSync(bool vsync)
{
	mVSync = vsync;
	if ( gGame.GetRenderWindow() )
		gGame.GetRenderWindow()->UseVerticalSync( mVSync );
}

void CGameOptions::SetSoundVolume(float sound){
    mSoundVolume = sound;
    gAudioManager.GetSoundPlayer().SetGeneralVolume(sound);
}

void CGameOptions::SetMusicVolume(float music){
    mMusicVolume = music;
    gAudioManager.GetMusicPlayer().SetMusicVolume(music);
}

void CGameOptions::SetControls(const std::string &controls,int player)
{
	mControls[player] = controls;
	System::Input::CBindManager::SetActualBindManager( StringUtils::ConvertToWString(controls), player );

    // jesli ktos sobie zmienia sterowanie, to usun first_game
    std::remove( (GetUserDir() + "/first_game").c_str() );
}

void CGameOptions::SetControls(size_t controlsId, int player)
{
    mControls[player] = StringUtils::ConvertToString(System::Input::CBindManager::GetBindName(controlsId, player));
    System::Input::CBindManager::SetActualBindManager(controlsId, player);

    // jesli ktos sobie zmienia sterowanie, to usun first_game
    // dex, 1.09.10: stolec prawda, SetControls(size_t, int) jest uzywane
    // tylko w LoadOptions() - nie trzeba, a nawet nie wolno kasowac.
    // Inaczej nie bedzie sie pokazywac set-controls-screen, jesli
    // w data/config.xml jest tag <controls>
    //std::remove( (GetUserDir() + "/first_game").c_str() );
}

void CGameOptions::SetUserControlsFlags(int player, int flags)
{
    // znajdz schemat..
    // szukaj od konca, bo gracza powinno byc ostatnie
    for (int i = mDefControlSchemes[player].size() - 1; i >= 0; --i)
        if (mDefControlSchemes[player][i].name == "$CTRL_SET_USER")
        {
            // zastosuj flagi
            mDefControlSchemes[player][i].flags = flags;

            break;
        }
}

void CGameOptions::SetKeyBinding(const std::string &controls, const std::string &name, int key, int player, float lock, bool dontAddToUserBinds)
{
    if (key == -1)
        return;

    if (!dontAddToUserBinds)
    {
	    unsigned index = 0;

	    for ( ; index < mKeyBindings[player].size(); index++ )
		    if ( (mKeyBindings[player][index].controls == controls) && (mKeyBindings[player][index].name == name) )
			    break;

	    if ( index >= mKeyBindings[player].size() )
	    {
		    mKeyBindings[player].resize( index + 1 );
		    mKeyBindings[player][index].controls = controls;
		    mKeyBindings[player][index].name = name;
	    }

	    mKeyBindings[player][index].key = key;

        // znajdz schemat..
        // szukaj od konca, bo gracza powinno byc ostatnie
        for (int i = mDefControlSchemes[player].size() - 1; i >= 0; --i)
            if (StringUtils::ConvertToString(gLocalizator.Localize(mDefControlSchemes[player][i].name.c_str())) == controls)
            {
                // ..i binding
                size_t j = 0;
                for (; j < mDefControlSchemes[player][i].bindings.size(); ++j)
                    if (mDefControlSchemes[player][i].bindings[j].name == name)
                        break;

                keyBinding bind;
                bind.controls = controls; // i tak ma miec wartosc "Gracza"
                bind.name = name;
                bind.key = key;
                if (name == "Abi-0" || name == "Abi-1" || name == "Abi-2" || name == "Abi-3" ||
                    name == "AbiX-0" || name == "AbiX-1" || name == "AbiX-2" || name == "AbiX-4")
                    bind.lock = 0.1f;
                else
                    bind.lock = -1.f;

                // "Gracza" nie posiada override'ow

                // jesli nie ma takiego, dodaj
                if (j == mDefControlSchemes[player][i].bindings.size())
                    mDefControlSchemes[player][i].bindings.push_back(bind);
                // jesli jest, podmien
                else
                    mDefControlSchemes[player][i].bindings[j] = bind;

                break;
            }
        
        // jesli ktos sobie zmienia bindingi, to wie co i jak
        // dex: tutaj, nie na koncu, bo wczytywanie sterowania z pliku
        // "przy okazji" usuwalo -_-'
        std::remove( (GetUserDir() + "/first_game").c_str() );
    }

	System::Input::CBindManager* bm = System::Input::CBindManager::GetBindManager( StringUtils::ConvertToWString(controls), player );
	if ( bm != NULL )
		bm->Change( name, key, lock );
}

int CGameOptions::GetKeyBinding(const std::string &controls, const std::string &name, int player)
{
	for ( unsigned i = 0; i < mKeyBindings[player].size(); i++ )
		if ( (mKeyBindings[player][i].controls == controls) && (mKeyBindings[player][i].name == name) )
			return mKeyBindings[player][i].key;
	return -1;
}


size_t CGameOptions::AddControlScheme( int player, const std::string& name, const std::string& desc, const std::string& menuDesc, int flags )
{
    size_t id = mDefControlSchemes[player].size();
    // NIEzlokalizowane, bo to tylko kopia do zapisania na potem
    mDefControlSchemes[player].push_back(controlScheme(player, name, desc, menuDesc, flags));

    System::Input::CBindManager::AddBindManager(gLocalizator.Localize(name), player);
    gBindManagerByPlayer(player)->SetControlsDescription(gLocalizator.Localize(desc));
    gBindManagerByPlayer(player)->SetControlsMenuDescription(gLocalizator.Localize(menuDesc));
    gBindManagerByPlayer(player)->SetMouseLook(!!(flags & cfMouseLook));
    gBindManagerByPlayer(player)->SetShowOnFirstGame(!!(flags & cfShowOnFirstGame));
    gBindManagerByPlayer(player)->SetIsAbsolute(!!(flags & cfAbsolute));

    if (flags & cfAbsolute)
    {
        // sync-grupa
        System::Input::CBindManager::CKeySyncGroup syncGroup;
	    syncGroup.mBindings.push_back("MoveUpAbs");
	    syncGroup.mBindings.push_back("MoveDownAbs");
	    syncGroup.mBindings.push_back("MoveLeftAbs");
	    syncGroup.mBindings.push_back("MoveRightAbs");
	    syncGroup.mTimeout = 0.05f;

        gBindManagerByPlayer(player)->AddSyncGroup(syncGroup);
    }

    if (flags & cfMousecast)
    {
        // mouse-caster
        std::vector<std::wstring> abiKeys;
	    abiKeys.push_back(L"Q");
	    abiKeys.push_back(L"W");
	    abiKeys.push_back(L"E");
	    abiKeys.push_back(L"R");

	    CMouseCaster* mc = new CMouseCaster();
	    mc->Initialize( abiKeys, 80.0f );
	    mc->SetKeyImage( "data/GUI/btn-up.png", "data/GUI/btn-hover.png" );
	    mc->SetKeyFont( "data/GUI/verdana.ttf", 16.0f );
	    mc->SetKeyColor( sf::Color::White );
	    mc->SetKeySize( sf::Vector2f(60.0f,60.0f) );

        gBindManagerByPlayer(player)->SetMouseCaster(mc);
    }

    return id;
}

void CGameOptions::AddControlSchemeBind( int player, size_t id, const std::string& bindName, int key, float lock, std::vector<std::pair<std::string, int> >& overrides )
{
    keyBinding kb;
    kb.name = bindName;
    kb.key = key;
    kb.lock = lock;
    kb.overrides = overrides;
    mDefControlSchemes[player][id].bindings.push_back(kb);
}

std::string CGameOptions::mModDir;
