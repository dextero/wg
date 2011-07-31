#include "MiscCommands.h"
#include "CCommands.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Console/CConsole.h"
#include "../Utils/StringUtils.h"
#include "../Rendering/CCamera.h"
#include "../Logic/CPhysicalManager.h"
#include "../Logic/CPhysical.h"
#include "../Logic/CEnemy.h"
#include "../Utils/CClock.h"
#include "../Utils/CRand.h"
#include "../Utils/FileUtils.h"
#include "../ResourceManager/CResourceManager.h"
#include "../GUI/CRoot.h"
#include "../Logic/CLogic.h"
#include "../VFS/vfs.h"
#include "../GUI/Localization/CLocalizator.h"

#include "../Logic/AI/CActorAI.h"
#include "../Logic/AI/CWandererScheme.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <fstream>
#include <boost/filesystem.hpp>

void CommandClearLogic(size_t argc, const std::vector<std::wstring> &argv)
{
    gLogic.Clear();
}

void CommandSetLogicState(size_t argc, const std::vector<std::wstring> &argv);
void CommandStartNewGame(size_t argc, const std::vector<std::wstring> &argv);
void CommandExitToMainMenu(size_t argc, const std::vector<std::wstring> &argv);
void CommandGuiCreateChooser(size_t argc, const std::vector<std::wstring> &argv);

// na koncu musi byc {0,0,0}, bo sie wszystko ***
CCommands::SCommandPair MiscCommands [] =
{
    {L"exec"                        , "$MAN_EXEC"                   , CommandExec                    },
    {L"print-screen"                , "$MAN_PRINT_SCREEN"           , CommandPrintScreen             },
    {L"capture-screen"              , "$MAN_CAPTURE_SCREEN"         , CommandCaptureScreen           },
	{L"clean"					    , "$MAN_CLEAN"                  , CommandCleanConsole            },
    {L"quit"                        , "$MAN_QUIT"                   , CommandQuit                    },
    {L"clock-info"                  , "$MAN_CLOCK_INFO"             , CommandGetClockInfo            },
	{L"echo"				        , "$MAN_ECHO"                   , CommandEcho		             },
    {L"add-effect-commands"         , "$MAN_ADD_EFFECT_COMMANDS"    , CommandShowParticles           },
	{L"load-resource"               , "$MAN_LOAD_RESOURCE"          , CommandLoadResource            },
	{L"play-music"					, "$MAN_PLAY_MUSIC"             , CommandPlayMusic				 },
	{L"stop-music"					, "$MAN_STOP_MUSIC"             , CommandStopMusic				 },
    {L"pause"                       , "$MAN_PAUSE"                  , CommandPause                   },
    {L"unpause"                     , "$MAN_UNPAUSE"                , CommandUnpause                 },
    {L"freeze"                      , "$MAN_FREEZE"                 , CommandFreeze                  },
    {L"unfreeze"                    , "$MAN_UNFREEZE"               , CommandUnfreeze                },
    {L"toggle-fx"                   , "$MAN_TOGGLE_FX"              , CommandPostFx                  },
    {L"set-video-mode"              , "$MAN_SET_VIDEO_MODE"         , CommandSetVideoMode            },
    {L"particles-info"              , "$MAN_PARTICLES_INFO"         , CommandParticlesInfo           },
    {L"play-sound"                  , "$MAN_PLAY_SOUND"             , CommandPlaySound               },
	{L"show-gui"					, "$MAN_SHOW_GUI"               , CommandShowGui				 },
	{L"set-logic-state"	     		, "$MAN_SET_LOGIC_STATE"        , CommandSetLogicState    		 },
	{L"clear-logic" 	     		, "$MAN_CLEAR_LOGIC"            , CommandClearLogic     		 },
    {L"load-playlist"               , "$MAN_LOAD_PLAYLIST"          , CommandLoadPlaylist            },
    {L"time"                        , "$MAN_TIME"                   , CommandTime                    },
    {L"start-new-game"              , "$MAN_START_NEW_GAME"         , CommandStartNewGame            },
    {L"exit-to-main-menu"           , "$MAN_EXIT_TO_MAIN_MENU"      , CommandExitToMainMenu          },
    {L"show-cursor"                 , "$MAN_SHOW_CURSOR"            , CommandShowCursor              },
    {L"show-loadingscreen"          , "$MAN_SHOW_LOADINGSCREEN"     , CommandShowLoadingScreen       },
    {L"list-vfs-contents"           , "$MAN_LIST_VFS_CONTENTS"      , CommandListVFSContents         },
	{L"force-load-all"				, "$MAN_FORCE_LOAD_ALL"         , CommandForceLoadAll			 },
    {L"vfs-extract"                 , "$MAN_VFS_EXTRACT"            , CommandVFSExtract              },
    {L"vfs-add"                     , "$MAN_VFS_ADD"                , CommandVFSAdd                  },
    {L"vfs-remove"                  , "$MAN_VFS_REMOVE"             , CommandVFSRemove               },
    {L"vfs-save"                    , "$MAN_VFS_SAVE"               , CommandVFSSave                 },
    {L"vfs-load"                    , "$MAN_VFS_LOAD"               , CommandVFSLoad                 },
    {L"vfs-contains"                , "$MAN_VFS_CONTAINS"           , CommandVFSContains             },
    {L"vfs-clear"                   , "$MAN_VFS_CLEAR"              , CommandVFSClear                },
    {L"gui-show-menu"               , "$MAN_GUI_SHOW_MENU"          , CommandGuiShowMenu             },
    {L"gui-create-chooser"          , "$MAN_GUI_CREATE_CHOOSER"     , CommandGuiCreateChooser        },
    {0,0,0}
};

std::wstring MergeArguments(const std::vector<std::wstring> &argv)
{
	if (argv.size() < 2) return L"";
	std::wstring result = argv[1];
	for (size_t i = 2; i < argv.size(); i++) {
		result += L" " + argv[i];
	}
	return result;
}

void CommandExec(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 2 )
    {
        gConsole.Printf( L"usage: %ls folder_name/file_name" , argv[0].c_str() );
        return;
    }

    std::string fullPath = StringUtils::ConvertToString( MergeArguments(argv) );
	FILE *f = fopen(fullPath.c_str(), "r");
    if ( !f )
    {
        std::string alternativePath = "data/console/" + fullPath + ".console";
		f = fopen(alternativePath.c_str(), "r");

        if ( !f )
        {
            if ( argc > 2 && argv[2] == L"ignore_warnings" )
                return;
            gConsole.Printf( L"File not found. (%s)", fullPath.c_str() );
            return;
        }
    }

    char line[1024];
    while ( !feof(f) )
    {
		fgets(line, 1024, f); 
        gCommands.ParseCommand( StringUtils::ReinterpretFromUTF8( line ) );
    }

    fclose(f);
}

void CommandPrintScreen(size_t argc, const std::vector<std::wstring> &argv)
{
    sf::Image screenshot = gGame.GetRenderWindow()->Capture();

    std::string filename;
    if ( argc >= 2 )
        filename = StringUtils::ConvertToString( argv[1] );
    else
        filename = gGameOptions.GetUserDir() + "/screenshot" + gClock.GenerateTimestamp() + ".jpg";
    if ( !screenshot.SaveToFile( filename ) )
    {
        gConsole.Printf( L"Unable to save screenshot: %s", filename.c_str() );
    }
}

void CommandCaptureScreen(size_t argc, const std::vector<std::wstring> &argv)
{
    sf::Image screenshot = gGame.GetRenderWindow()->Capture();
    gLogic.SetGameThumbnail(screenshot);
}

void CommandCleanConsole(size_t argc, const std::vector<std::wstring> &argv){
	gConsole.CleanScreen();
}

void CommandQuit(size_t argc, const std::vector<std::wstring> &argv)
{
    gGame.GetRenderWindow()->Close();
}

void CommandGetClockInfo(size_t argc, const std::vector<std::wstring> &argv)
{
    gConsole.Printf( L"Current FPS=%f Average FPS=%f", gClock.GetFPS(), gClock.GetAverageFPS() );
    gConsole.Printf( L"Frames count: %u Time: %f", gClock.GetFramesCount(), gClock.GetTotalTime() );
    gConsole.Printf( L"Float Random num(-1..1): %f num(0..1): %f", gRand.Rndnf(), gRand.Rndf() );
    gConsole.Printf( L"Random num(0..RAND_MAX): %d num(1..10): %d", gRand.Rnd(), gRand.Rnd(1,10) );
}

void CommandEcho(size_t argc, const std::vector<std::wstring> &argv)
{
    // dex: zmienilem troche, bo Printf krzaczylo sie na stringach z lokalizatora
    // ($*), tych ktore maja jakies %ls czy inne %d w sobie..
    std::wstring text = L"";
	for(size_t i=1; i<argc; i++)
        //gConsole.Printf(gLocalizator.Localize(argv[i]).c_str());
        text += gLocalizator.Localize(argv[i]) + L" ";
    gConsole.Print(text);
}
#include "EffectsCommands.h"
void CommandShowParticles(size_t argc, const std::vector<std::wstring> &argv)
{    
    //gConsole.Print(L"Rejestracja nowych komend :)");
    gCommands.RegisterCommands(EffectsCommands);

    //part::gParticleManager.LoadConfiguration("data\\particles\\particles.xml");
    
    //static bool show = false;

    //show = !show;
    //if (show)
    //{
    //    gConsole.Print(L"Puszczam dymek :)");
    //    part::gParticleManager.AddEffect(new part::effect::CSmoke);
    //}
    //else
    //{
    //    gConsole.Print(L"Juz nie bede dymic.. :P");
    //    part::gParticleManager.KillLast();
    //}
}
#include "../Rendering/Particles/CParticlePool.h"
void CommandParticlesInfo(size_t argc, const std::vector<std::wstring> &argv)
{
    gConsole.Printf(L"Ilość cząsteczek: obecnie - %u, maksymalnie - %u\nilosc wywolan create - %u, ilosc wywołan release- %u",
        part::gParticlePool.CurrentParticleCount(), part::gParticlePool.MaximumParticleCount(), part::gParticlePool.GetCreateCallNum(), part::gParticlePool.GetReleaseCallNum());
}
//------------------
void CommandLoadResource(size_t argc, const std::vector<std::wstring> &argv)
{
	bool print_usage = false;
	if(argc>2)
	{
		if((argc-3)%2 == 0)
		{
			std::map<std::string,std::string> args;
			std::string name;
			name.assign(argv[2].begin(), argv[2].end());
			args["name"] = name;
			for(unsigned int i=3; i<argc; i+=2)
			{
				std::string key;
				std::string value;

				key.assign(argv[i].begin(), argv[i].end());
				value.assign(argv[i+1].begin(), argv[i+1].end());

				args[key] = value;
			}

			std::string type;
			type.assign(argv[1].begin(), argv[1].end());

#undef LoadImage    // rav: jakis chamski lol sobie zajal ta nazwe
			if (type == "image")
				gResourceManager.LoadImage(args);
			else if (type == "map")
				gResourceManager.LoadMap(args);
			else if (type == "font")
				gResourceManager.LoadFont(args);
			else if (type == "sound")
				gResourceManager.LoadSound(args);
			else if (type == "music")
				gResourceManager.LoadMusic(args);
            else if (type == "atlas")
                /*ResourceHandle t = */gResourceManager.LoadAtlas(args);
            else
                gConsole.Printf(L"Unknown resource type: %s", StringUtils::ConvertToWString(type).c_str());
		}
		else
			print_usage	= true;
	}
	else
		print_usage	= true;

	if(print_usage)
		gConsole.Print(L"usage: load-resource <type> <name> <key1> <value1> <key2> <value2> ...");
}

#include "../Audio/CAudioManager.h"

void CommandPlayMusic(size_t argc, const std::vector<std::wstring> &argv)
{
	if(argc<2)
    {
        gConsole.Print(L"usage: music-play <name>");
		return;
    }
    std::string name = StringUtils::ConvertToString(argv[1]);
    System::Resource::CMusic* music = gResourceManager.GetMusic(name);
	if(music)
        gAudioManager.GetMusicPlayer().Play(music);
    else
        gConsole.Printf(L"failed to play music - %s not found (is this resource loaded?)", name.c_str());
}

void CommandStopMusic(size_t argc, const std::vector<std::wstring> &argv)
{
	if(argc<2)
    {
        gConsole.Print(L"usage: music-stop <name>");
		return;
    }
	std::string name = StringUtils::ConvertToString(argv[1]);
    System::Resource::CMusic* music = gResourceManager.GetMusic(name);
	if(music)
		music->GetMusicStream().Stop();
    else
        gConsole.Printf(L"failed to stop music - %s not found", name.c_str());
}

void CommandPause(size_t argc, const std::vector<std::wstring> &argv){
    gGame.SetPause(true);
}

void CommandUnpause(size_t argc, const std::vector<std::wstring> &argv){
    gGame.SetPause(false);
}

void CommandFreeze(size_t argc, const std::vector<std::wstring> &argv){
    gGame.SetFreezed(true);
}

void CommandUnfreeze(size_t argc, const std::vector<std::wstring> &argv){
    gGame.SetFreezed(false);
}


//rAum: testowo niech sobie polezy tutaj w takiej formie
#include "../Rendering/PostProcessing/CPostProcessing.h"
#include "../Rendering/PostProcessing/Effects/CLight.h"
#include "../Rendering/PostProcessing/Effects/CHole.h"
#include "../Rendering/PostProcessing/Effects/CMotionBlur.h"
#include "../Rendering/PostProcessing/Effects/CFader.h"
#include "../Rendering/PostProcessing/Effects/CColorize.h"
#include "../Rendering/PostProcessing/Effects/CNormalMapping.h"
void CommandPostFx(size_t argc, const std::vector<std::wstring> &argv)
{
    static bool w = false;   

    if (!w)
    {
        //gPostProcessing.AddEffect("motionblur",new pfx::effect::CMotionBlur());
        gPostProcessing.AddEffect("light",new pfx::effect::CLight());
        gPostProcessing.GetEffect("light")->Enable( false );
        gPostProcessing.AddEffect("fade",new pfx::effect::CFader());
        gPostProcessing.AddEffect("hole",new pfx::effect::CHole());
        gPostProcessing.GetEffect("hole")->Enable( false);
        gPostProcessing.AddEffect("colorize",new pfx::effect::CColorize());
        gPostProcessing.GetEffect("colorize")->Enable( false );
        gPostProcessing.AddEffect("nm",new pfx::effect::CNormalMapping());
        gPostProcessing.GetEffect("nm")->Enable( false );

        w = true;
    }    

    if (argc > 2)
    {
        if (argv[1] == L"experiment")
        {
            if (argv[2] == L"on")
                gPostProcessing.GetEffect("nm")->Enable( true );
            else if (argv[2] == L"off")
                gPostProcessing.GetEffect("nm")->Enable( false );
            else
                gConsole.Print(L"usage: toggle-fx experiment <on, off>");                
        }
        else if (argv[1] == L"light")
        {
            if (argv[2] == L"on")
                gPostProcessing.GetEffect("light")->Enable( true );
            else if (argv[2] == L"off")
                gPostProcessing.GetEffect("light")->Enable( false );
            else
                gConsole.Print(L"usage: toggle-fx light <on, off>");

        }
        else if (argv[1] == L"hole")
        {
            if (argv[2] == L"on")
                gPostProcessing.GetEffect("hole")->Enable( true );
            else if (argv[2] == L"off")
                gPostProcessing.GetEffect("hole")->Enable( false );
            else
                gConsole.Print(L"usage: toggle-fx hole <on, off>");
        }
        else if (argv[1] == L"mode")
        {
            if (argv[2] == L"rtt")
            {
                gPostProcessing.SetType(pfx::PFX_TYPE::TEXTURE);
                gConsole.Print(L"Launching Software Effects!");
            }
            else if (argv[2] == L"shd")
            {
                if (gPostProcessing.SetType(pfx::PFX_TYPE::SHADERS))
                    gConsole.Print(L"Shaders available, initializing!");
                else
                {
                    gConsole.Print(L"Shadery unavailable, falling back to software effects!");
                    gPostProcessing.SetType(pfx::PFX_TYPE::TEXTURE);
                }
            }
            else if (argv[2] == L"non")
            {
                gPostProcessing.SetType(pfx::PFX_TYPE::NONE);
                gConsole.Print(L"Disabled all effecs.");
            }
            else
                gConsole.Print(L"usage: toggle-fx mode <type: rtt, shd or non>");
        }
        else if (argv[1] == L"fade")
        {
            if (argv[2] == L"in")
                static_cast<pfx::effect::CFader *>(gPostProcessing.GetEffect("fade"))->SetFade(pfx::effect::PFX_FADE::FADE_IN);
            else if (argv[2] == L"out")
                static_cast<pfx::effect::CFader *>(gPostProcessing.GetEffect("fade"))->SetFade(pfx::effect::PFX_FADE::FADE_OUT);
            else
                gConsole.Print(L"usage: toggle-fx fade <in, out>");
        }
        else if (argv[1] == L"colorize")
        {
            if (argv[2] == L"on")
                gPostProcessing.GetEffect("colorize")->Enable(true);
            else if (argv[2] == L"off")
                gPostProcessing.GetEffect("colorize")->Enable(false);
            else
                gConsole.Print(L"usage: toggle-fx colorize <on, off>");
        }
        else
            gConsole.Print(L"usage: toggle-fx <mode, fade, light, hole, colorize> [options]");
    }
    else
        gConsole.Print(L"usage: toggle-fx <mode, fade, light, hole, colorize> [options]");
}
//-------------
#include <iostream>
#include "../CGameOptions.h"

void CommandSetVideoMode(size_t argc, const std::vector<std::wstring> &argv)
{
    if(argc < 5)
    {
        gConsole.Print(L"usage: set-video-mode <width> <height> <fullscreen> <bpp>");
        return;
    }
    unsigned int tmp;

    StringUtils::FromString(StringUtils::ConvertToString(argv[1]), tmp);
    gGameOptions.SetWidth(tmp);

    StringUtils::FromString(StringUtils::ConvertToString(argv[2]), tmp);
    gGameOptions.SetHeight(tmp);

    StringUtils::FromString(StringUtils::ConvertToString(argv[3]), tmp);
    gGameOptions.SetFullscreen(!!tmp);

    StringUtils::FromString(StringUtils::ConvertToString(argv[4]), tmp);
    gGameOptions.SetBitsPerPixel(tmp);


    sf::VideoMode mode(gGameOptions.GetWidth(), gGameOptions.GetHeight(), gGameOptions.GetBitsPerPixel());
    if(!mode.IsValid())
        return;
    
    gGameOptions.UpdateWindow();
    gGameOptions.SaveOptions();     // TODO: wywalic to stad i wrzucic do destruktora klasy CGameOptions 
                                    // tox,14 sierpnia, to ja tego todo powyzej napisalem?
}

#include "../Audio/CAudioManager.h"

void CommandPlaySound(size_t argc, const std::vector<std::wstring> &argv)
{
    if(argc < 2)
    {
        gConsole.Print(L"usage: play-sound <filename>");
        return;
    }

    std::string name = StringUtils::ConvertToString(argv[1]);
    System::Resource::CSound *sound = gResourceManager.GetSound(name);

    if(sound)
    {
        gAudioManager.GetSoundPlayer().Play(sound);
    } else
        gConsole.Printf(L"failed to play sound - %s not found", name.c_str());
}

void CommandShowGui(size_t argc, const std::vector<std::wstring> &argv)
{
	gGUI;
}

void CommandSetLogicState(size_t argc, const std::vector<std::wstring> &argv)
{
    gConsole.Printf(L"tox, 14 sierpnia: deprecated, prosze nie uzywac");
    if(argc < 2)
    {
        gConsole.Printf(L"usage: %ls <statename>", argv[0].c_str());
        return;
    }

    gLogic.SetState(argv[1]);
}

void CommandStartNewGame(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 2 )	gLogic.StartNewGame();
	else			gLogic.StartNewGame( argv[1] );
}

void CommandExitToMainMenu(size_t argc, const std::vector<std::wstring> &argv)
{
    gLogic.ExitToMainMenu();
}

void CommandLoadPlaylist(size_t argc, const std::vector<std::wstring> &argv)
{
    if(argc<2)
    {
        gConsole.Printf(L"usage: <filename>");
        return;
    }

    Audio::CPlayList playlist;
    if(playlist.Load(argv[1]))
    {
        gAudioManager.GetMusicPlayer().SetPlayList(playlist);
    }
}
#include "../CTimeManipulator.h"
void CommandTime(size_t argc, const std::vector<std::wstring> &argv)
{
    bool show_usage = false;
    if (argc == 2)
    {
        if (argv[1] == L"slower") {
            gTimeManipulator.SetMode( Time::BULLET_TIME );
            gConsole.Print( L"Bullet time ON!" );
        }
        else if (argv[1] == L"faster") {
            gTimeManipulator.SetMode ( Time::SHIFT_TIME );
            gConsole.Print( L"Shift time ON!" );
        }
        else if (argv[1] == L"reverse") {
            gTimeManipulator.SetMode ( Time::REWIND_TIME );
            gConsole.Print( L"Reverse time ON! What have you done, you fool!" );
        }
        else
            show_usage = true;
    }
    else
    {
        gTimeManipulator.SetMode ( Time::NORMAL_TIME );
        gConsole.Print ( L"Time is back to normal" );
        show_usage = true;
    }

    if (show_usage)
        gConsole.Print(L"usage: time [<slower, faster, reverse (do NOT use reverse - it's just for fun ;p)>] - no args returns time to normal");
}

void CommandShowCursor(size_t argc, const std::vector<std::wstring> &argv)
{
    bool show_usage(true);

    if (argc > 1)
    {
        if (argv[1] == L"true" || argv[1] == L"1" ) {
            gGUI.ShowCursor();
            show_usage = false;
        }
        else if (argv[1] == L"false" || argv[1] == L"0") {
            gGUI.ShowCursor(false);
            show_usage = false;
        }          
    }

    if (show_usage)
        gConsole.Print( L"usage: show-cursor true(1)/false(0)" );
}

#include "../GUI/CLoadingScreen.h"
void CommandShowLoadingScreen(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc > 1)
    {
        if ( argv[1] == L"true" || argv[1] == L"1" )
            GUI::CLoadingScreen::Show();
        else if ( argv[1] == L"false" || argv[1] == L"0" )
            GUI::CLoadingScreen::Hide();
        else
            gConsole.Print( L"usage: show-loadingscreen true(1)/false(0)" );
    }
    else
        gConsole.Print( L"usage: show-loadingscreen true(1)/false(0)" );
}

void CommandListVFSContents(size_t argc, const std::vector<std::wstring> &argv){
    const std::vector<std::string> &filenames = gVFS.GetFilenames();
    for (unsigned int i = 0; i < filenames.size(); i++)
        gConsole.Printf(L"%s",StringUtils::ConvertToWString(filenames[i]).c_str());
}

void CommandForceLoadAll(size_t argc, const std::vector<std::wstring> &argv){
	gResourceManager.ForceLoadAll();
}

// -----------------------
//          VFS
// -----------------------

void CommandVFSExtract(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc > 1)
    {
        gVFS.ExtractFiles(StringUtils::ConvertToString(argv[1]));
        gConsole.Printf(L"Files extracted successfully!");
    }
    else
        gConsole.Printf(L"usage: %s directory", argv[0].c_str());
}

void CommandVFSAdd(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc > 1)
    {
        std::vector<std::string> files;

        // a co tam, niech bedzie mozna dodawac pare naraz..
        for (size_t i = 1; i < argc; ++i)
        {
            if (boost::filesystem::is_directory(StringUtils::ConvertToString(argv[i])))
            {
                // ..nawet cale katalogi..
                std::vector<std::string> dirs;
                dirs.push_back(StringUtils::ConvertToString(argv[i]));
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
				            files.push_back(file);
		            }
	            }
            }
            else
                files.push_back(StringUtils::ConvertToString(argv[i]));
        }

        // hajda z tymi plikami!
        size_t added = 0;
        for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
        {
			//todo: do przerobienia na FILE* (jesli ma sie ladowac spoza workdir)
            std::ifstream file(it->c_str(), std::ios::in | std::ios::binary);
            if (!file.is_open())
            {
                gConsole.Printf(L"Couldn't open file: %S", it->c_str());
                return;
            }

            file.seekg(0, std::ios::end);
            size_t size = file.tellg();

            char* buf = new char[size];
            file.read(buf, size);
            file.close();

            if (gVFS.AddFile(it->c_str(), buf, size))
            {
                gConsole.Printf(L"+ %S", it->c_str());
                ++added;
            }
            else
                gConsole.Printf(L"Something strange happened, %s was not added to VFS.. oO", it->c_str());

            // AddFile kopiuje bufor, wiec kasujemy
            delete[] buf;
        }

        gConsole.Printf(L"Added %i files. Use vfs-save to save changes to disk.", added);
    }
    else
        gConsole.Printf(L"usage: %s file|directory [file|directory ..]", argv[0].c_str());
}
void CommandVFSRemove(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc > 1)
    {
        if (gVFS.RemoveFile(StringUtils::ConvertToString(argv[1]).c_str()))
            gConsole.Printf(L"File %s removed! Use vfs-save to save changes to disk.", argv[1].c_str());
        else
            gConsole.Printf(L"VFS didn't contain %s o_O", argv[1].c_str());
    }
    else
        gConsole.Printf(L"usage: %s file, 1 file at a time. you can use vfs-clear to remove everything", argv[0].c_str());
}

void CommandVFSSave(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc > 1)
    {
        gVFS.SaveToFile(StringUtils::ConvertToString(argv[1]).c_str());
        gConsole.Printf(L"VFS saved!");
    }
    else
        gConsole.Printf(L"usage: %s path", argv[0].c_str());
}

void CommandVFSLoad(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc > 1)
    {
        if (!FileUtils::FileExists(StringUtils::ConvertToString(argv[1]).c_str()))
        {
            gConsole.Printf(L"File %s doesn't exist.", argv[1].c_str());
            return;
        }
		//todo: do przerobienia na FILE* jesli ma sie ladowac spoza workdir
        std::ifstream file(StringUtils::ConvertToString(argv[1]).c_str(), std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            gConsole.Printf(L"Couldn't open file: %s", argv[1].c_str());
            return;
        }

        file.seekg(0, std::ios::end);
        if (file.tellg() < 4)
        {
            gConsole.Printf(L"Are you kidding me?");
            return;
        }
        file.seekg(std::ios::beg);

        char buf[4];
        file.read(buf, 4);
        buf[3] = '\0';
        if (strcmp(buf, "VFS") != 0)
        {
            gConsole.Printf(L"Bad file signature: %S - \"VFS\" expected", buf);
            return;
        }

        file.close();
        gVFS.OpenFileSystem(StringUtils::ConvertToString(argv[1]).c_str());

        gConsole.Printf(L"VFS loaded. Probably..");
    }
    else
        gConsole.Printf(L"Usage: %s file.vfs", argv[0].c_str());
}

void CommandVFSContains(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc > 1)
        gConsole.Printf(L"VFS %s %s", (gVFS.IsInVFS(StringUtils::ConvertToString(argv[1]).c_str()) ? L"contains" : L"does NOT contain"), argv[1].c_str());
    else
        gConsole.Printf(L"usage: %s path", argv[0].c_str());
}

void CommandVFSClear(size_t argc, const std::vector<std::wstring> &argv)
{
    gVFS.Clear();
    gConsole.Printf(L"VFS is now clean and shiny.");
}

// -----------------------
//          GUI
// -----------------------

void CommandGuiShowMenu(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2)
        gConsole.Printf(L"usage: %s menu-name", argv[0].c_str());
    else
        gLogic.GetMenuScreens()->Show(argv[1]);
}

#include "../GUI/CInGameOptionChooser.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/CPlayer.h"
#include "../Input/CPlayerController.h"
#include "../Logic/OptionChooser/CSimpleOptionHandler.h"

void CommandGuiCreateChooser(size_t argc, const std::vector<std::wstring> &argv)
{  
    CInGameOptionChooser* oc = NULL;
    CPlayer * p = gPlayerManager.GetPlayerByNumber(0);
    CPlayerController * pc = p != NULL ? p->GetController() : NULL;
    if (!pc) return;

    oc = pc->GetOptionChooser();
    oc->Hide();
    if (argc < 2) return;

    std::vector<std::string> options;
    for (size_t i = 1 ; i < argv.size() ; i++) {
        options.push_back(StringUtils::ConvertToString(argv[i]));
    }

    oc->SetOptions(options);
    oc->SetOptionHandler(new CSimpleOptionHandler());

    oc->Show();
}


