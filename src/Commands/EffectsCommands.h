#ifndef __EFFECTSCOMMANDS_H__
#define __EFFECTSCOMMANDS_H__

#include "CCommands.h"

extern CCommands::SCommandPair EffectsCommands[];

void CommandInitialize(size_t argc, const std::vector<std::wstring> &argv);
void CommandAddMagic(size_t argc, const std::vector<std::wstring> &argv);
void CommandDelMagic(size_t argc, const std::vector<std::wstring> &argv);
void CommandInitializeDrop(size_t argc, const std::vector<std::wstring> &argv);
void CommandAddPhysicalToPM(size_t argc, const std::vector<std::wstring> &argv);
void CommandMessage(size_t argc, const std::vector<std::wstring> &argv);

void CommandSetLighting(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetAmbient(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetNMAmbient(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetNMContrast(size_t argc, const std::vector<std::wstring> &argv);
void CommandCreateLight(size_t argc, const std::vector<std::wstring> &argv);
void CommandDestroyAllLights(size_t argc, const std::vector<std::wstring> &argv);

#endif
