#ifndef __MISC_COMMANDS_H__
#define __MISC_COMMANDS_H__

#include <string>
#include "CCommands.h"

// Polecenia zwiazane z animacjami i obiektami CDisplayable

extern CCommands::SCommandPair AnimCommands[];

void CommandSpawnDisplayable(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayableImage(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayablePosition(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayableScale(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayableCenter(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayableRect(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayableAnimation(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayableAnimationPlay(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayableAnimationStop(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayableAnimationLoop(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDisplayableAnimationRewindTo(size_t argc, const std::vector<std::wstring> &argv);

#endif//__MISC_COMMANDS_H__

