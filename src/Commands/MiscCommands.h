#ifndef __MISC_COMMANDS_H__
#define __MISC_COMMANDS_H__

#include <string>
#include "CCommands.h"

// Zbior polecen roznych - miscellaneous commands :)

/* przy dodawaniu nowych polecen do konsoli piszemy ponizej deklaracje funkcji
 * a do MiscCommands dopisujemy pare (tekst polecenia, nazwa funkcji).
 * (tak jak pozostale funkcje sa zrobione).
 *  
 * mozna tworzyc i rejestrowac wlasne zbiory polecen: 
 * wystarczy wywolac gCommands.RegisterCommands(SCommandPair commands[])
 * (przekazujac tablice taka jak MiscCommands - z {0,0} na koncu)
 */

extern CCommands::SCommandPair MiscCommands[];

void CommandExec(size_t argc, const std::vector<std::wstring> &argv);
void CommandPrintScreen(size_t argc, const std::vector<std::wstring> &argv);
void CommandCaptureScreen(size_t argc, const std::vector<std::wstring> &argv);
void CommandQuit(size_t argc, const std::vector<std::wstring> &argv);
void CommandGetClockInfo(size_t argc, const std::vector<std::wstring> &argv);
void CommandEcho(size_t argc, const std::vector<std::wstring> &argv);
void CommandShowParticles(size_t argc, const std::vector<std::wstring> &argv);
void CommandLoadResource(size_t argc, const std::vector<std::wstring> &argv);
void CommandPlayMusic(size_t argc, const std::vector<std::wstring> &argv);
void CommandStopMusic(size_t argc, const std::vector<std::wstring> &argv);
void CommandCleanConsole(size_t argc, const std::vector<std::wstring> &argv);
void CommandPause(size_t argc, const std::vector<std::wstring> &argv);
void CommandUnpause(size_t argc, const std::vector<std::wstring> &argv);
void CommandFreeze(size_t argc, const std::vector<std::wstring> &argv);
void CommandUnfreeze(size_t argc, const std::vector<std::wstring> &argv);
void CommandPostFx(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetVideoMode(size_t argc, const std::vector<std::wstring> &argv);
void CommandParticlesInfo(size_t argc, const std::vector<std::wstring> &argv);
void CommandPlaySound(size_t argc, const std::vector<std::wstring> &argv);
void CommandShowGui(size_t argc, const std::vector<std::wstring> &argv);
void CommandLoadPlaylist(size_t argc, const std::vector<std::wstring> &argv);
void CommandTime(size_t argc, const std::vector<std::wstring> &argv);
void CommandShowCursor(size_t argc, const std::vector<std::wstring> &argv);
void CommandShowLoadingScreen(size_t argc, const std::vector<std::wstring> &argv);
void CommandListVFSContents(size_t argc, const std::vector<std::wstring> &argv);
void CommandForceLoadAll(size_t argc, const std::vector<std::wstring> &argv);

// VFS
void CommandVFSExtract(size_t argc, const std::vector<std::wstring> &argv);
void CommandVFSAdd(size_t argc, const std::vector<std::wstring> &argv);
void CommandVFSRemove(size_t argc, const std::vector<std::wstring> &argv);
void CommandVFSSave(size_t argc, const std::vector<std::wstring> &argv);
void CommandVFSLoad(size_t argc, const std::vector<std::wstring> &argv);
void CommandVFSContains(size_t argc, const std::vector<std::wstring> &argv);
void CommandVFSClear(size_t argc, const std::vector<std::wstring> &argv);

// GUI
void CommandGuiShowMenu(size_t argc, const std::vector<std::wstring> &argv);

#endif//__MISC_COMMANDS_H__

