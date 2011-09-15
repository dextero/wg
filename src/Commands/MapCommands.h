#ifndef __MAPCOMMANDS_H__
#define __MAPCOMMANDS_H__

#include "CCommands.h"


extern CCommands::SCommandPair MapCommands[];

void CommandLoadMap(size_t argc, const std::vector<std::wstring> &argv);
void CommandPreloadMap(size_t argc, const std::vector<std::wstring> &argv);
void CommandUnloadMap(size_t argc, const std::vector<std::wstring> &argv);
void CommandPrintSceneStats(size_t argc, const std::vector<std::wstring> &argv);
void CommandCreateWall(size_t argc, const std::vector<std::wstring> &argv);
void CommandShowRegion(size_t argc, const std::vector<std::wstring> &argv);
void CommandEditorHelp(size_t argc, const std::vector<std::wstring> &argv);
void CommandSaveMap(size_t argc, const std::vector<std::wstring> &argv);
void CommandSaveEmptyMap(size_t argc, const std::vector<std::wstring> &argv);
void CommandDeleteRegion(size_t argc, const std::vector<std::wstring> &argv);
void CommandGenerateRandomMap(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetWorld(size_t argc, const std::vector<std::wstring> &argv);

#endif
