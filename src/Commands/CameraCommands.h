#ifndef __CAMERACOMMANDS_H__
#define __CAMERACOMMANDS_H__

#include "CCommands.h"

extern CCommands::SCommandPair CameraCommands[];

void CommandTrailPhysical(size_t argc, const std::vector<std::wstring> &argv);
void CommandTrailStop(size_t argc, const std::vector<std::wstring> &argv);
void CommandTrailPlayers(size_t argc, const std::vector<std::wstring> &argv);
void CommandCameraSway(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetCameraPosition(size_t argc, const std::vector<std::wstring> &argv);
void CommandShakeCamera(size_t argc, const std::vector<std::wstring> &argv);

#endif
