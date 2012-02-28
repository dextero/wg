#include "CGameOptions.h"
#include "CGame.h"
#include "Utils/FileUtils.h"
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
#include "Input/CPlayerController.h"
#include "Logic/Achievements/CAchievementManager.h"
#include "Logic/Quests/CQuestManager.h"
#include "Logic/CPlayerManager.h"
#include "Logic/CPlayer.h"
#include "Logic/CLogic.h"
#include "GUI/Localization/CLocalizator.h"
#include "ResourceManager/CImage.h"

extern bool AskForFullscreen(const wchar_t * title, const wchar_t * message, int maxw, int maxh);

bool UserWantToPlayInFullscreen(unsigned int *width, unsigned int *height)
{
#ifdef __EDITOR__
    return false;
#endif
	sf::VideoMode videoMode;
	
	unsigned int maxw = *width;
	unsigned int maxh = *height;

	for ( size_t i = 0; i < sf::VideoMode::GetModesCount(); i++ )
	{
		videoMode = sf::VideoMode::GetMode( i );		
		if (videoMode.Width > maxw) { maxw = videoMode.Width; maxh = videoMode.Height; }
	}

	const std::wstring titleStr = gLocalizator.GetText("FULLSCREEN_TITLE");
	const std::wstring messageStr = gLocalizator.GetText("FULLSCREEN_MESSAGE");
	bool userAcceptedFullscreen = AskForFullscreen(titleStr.c_str(), messageStr.c_str(), maxw, maxh);
	if (userAcceptedFullscreen) {
		*width = maxw;
		*height = maxh;
	}
	return userAcceptedFullscreen;
}

template<> CGameOptions* CSingleton<CGameOptions>::msSingleton = 0;

bool CGameOptions::LoadOptions()
{
	unsigned int configVer = 0;
	unsigned int userConfigVer = 0;

	if ( FileUtils::FileExists( "data/config.xml" ) )
	{
		CXml xml( "data/config.xml", "root" );
		configVer = xml.GetInt( NULL, "version" );
	}

	bool is_first_game = false;
	
	if ( FileUtils::FileExists( FileUtils::GetUserDir() + "/config.xml" ) )
	{
		CXml xml( FileUtils::GetUserDir() + "/config.xml", "root" );
		userConfigVer = xml.GetInt( NULL, "version" );
	}
    else
    {
        // nie ma user/config, stworz plik first_game, zeby gracz wybral sobie sterowanie..
        FILE *first_game = fopen((FileUtils::GetUserDir() + "/first_game").c_str(), "w");
        if (first_game)
            fclose(first_game);
        else
            fprintf(stderr, "FATAL ERROR: couldn't create first_game file in userDir!\n");
		is_first_game = true;
    }

	if ( configVer + userConfigVer == 0 )
	{
		fprintf(stderr, "Cannot open config file\n");
		return false;
	}

	CXml xml( configVer > userConfigVer ? "data/config.xml" : FileUtils::GetUserDir() + "/config.xml", "root" );

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
            {
                if (file.rfind(".xml") != file.length() - 4) {
                    fprintf(stderr, "Localizator: found some non .xml file in locale, %s\n", file.c_str());
                } else {
    				gLocalizator.Load(file);
                }
            }
		}
	}
    // koniec ladowania locale

    // ladowanie achievementow
    if (FileUtils::FileExists(FileUtils::GetUserDir() + "/achievements.xml"))
        gAchievementManager.Load(FileUtils::GetUserDir() + "/achievements.xml");
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

	mFullscreen = xml.GetInt("video", "fs") != 0;
	
	if (is_first_game) {
		mWidth = 800; 
		mHeight = 600;
		mFullscreen = UserWantToPlayInFullscreen(&mWidth, &mHeight);
	}

	SetSmooth(xml.GetInt("smooth", "on", 1) != 0); // domyslnie wlaczamy smooth
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

#ifdef PLATFORM_LINUX
extern bool CanCreateWindowInFullScreenOnLinux();
bool CanCreateWindowInFullScreen() { return CanCreateWindowInFullScreenOnLinux(); }
#else
bool CanCreateWindowInFullScreen() { return true; }
#endif

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


CGameOptions::CGameOptions():
    mVersion(0),
    mWidth(0),
    mHeight(0),
    mBPP(0),
    mFullscreen(false),
    mVSync(true),
    m3DSound(true),
	mSmooth(true),
    mSoundVolume(100.f),
    mMusicVolume(100.f)
{
	mUserDir = FileUtils::GetUserDir();

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
    FILE *file = fopen((gGameOptions.GetUserDir() + "/config.xml").c_str(), "w");

	if(!file){
		fprintf(stderr,"Error opening %s/config.xml for writing, options not saved!\n", gGameOptions.GetUserDir().c_str());
        return;
	}

    // TODO: dopisac inne opcje ponizej:
    fprintf(file, "<root type=\"config\" version=\"%d\">\n", mVersion);
	fprintf(file, "\t<!-- VideoMode Config -->\n");
	fprintf(file, "\t<video width=\"%d\" height=\"%d\" fs=\"%d\" bpp=\"%d\" vsync=\"%d\" />\n", mWidth, mHeight, mFullscreen, mBPP, mVSync);
    fprintf(file, "\t<smooth on=\"%d\" />\n", (int) mSmooth);
	fprintf(file, "\t<audio sound=\"%f\" music=\"%f\" stereo=\"%d\" />\n", mSoundVolume, mMusicVolume, m3DSound);
	fprintf(file, "\t<locale lang=\"%s\" />\n\n", mLocaleLang.c_str());

    for ( unsigned p = 0; p < PLAYERS_CNT; ++p )
        for ( unsigned c = 0; c < mDefControlSchemes[p].size(); ++c )
            fprintf(file, "%s", mDefControlSchemes[p][c].Serialize().c_str());

	for ( unsigned p = 0; p < PLAYERS_CNT; p++ )
        fprintf(file, "\t<controls controls=\"%d\" player=\"%d\" />\n", System::Input::CBindManager::GetBindId(mControls[p], p), (int)p);

	fprintf(file, "</root>\n");
    fclose(file);
}

void CGameOptions::SetSmooth(bool s)
{
	mSmooth = s;
	System::Resource::CImage::smoothEnabled = s;
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
    // zmiana scheme'a? reset current-itema
    if (gPlayerManager.GetPlayerByNumber(player))
    {
        CPlayer* p = gPlayerManager.GetPlayerByNumber(player);
        p->InvalidateCurrentItem();
        if (p->GetController())
            p->GetController()->SetWalkTarget(false, sf::Vector2f(0.f, 0.f), true);
    }

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
    for (int i = (int)mDefControlSchemes[player].size() - 1; i >= 0; --i)
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
        for (int i = (int)mDefControlSchemes[player].size() - 1; i >= 0; --i)
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

    // zmiana bindingu? reset current-itema
    if (gPlayerManager.GetPlayerByNumber(player))
        gPlayerManager.GetPlayerByNumber(player)->InvalidateCurrentItem();
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
    gBindManagerByPlayer(player)->SetPointNClickMove(!!(flags & cfPointNClick));

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
        const GUI::FontSetting & fs = gGUI.GetFontSetting("FONT_MOUSE_CAST");
	    mc->SetKeyFont(fs.name, fs.size, fs.unit);
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
