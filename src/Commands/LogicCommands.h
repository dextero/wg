 #ifndef __MISC_COMMANDS_H__
#define __MISC_COMMANDS_H__

#include <string>
#include "CCommands.h"

extern CCommands::SCommandPair LogicCommands[];

void CommandSpawnPhysical(size_t argc, const std::vector<std::wstring> &argv);
void CommandSpawnPhysicalRot(size_t argc, const std::vector<std::wstring> &argv);
void CommandSpawnRawPhysical(size_t argc, const std::vector<std::wstring> &argv);
void CommandSpawnWeapon(size_t argc, const std::vector<std::wstring> &argv);
void CommandSpawnDoor(size_t argc, const std::vector<std::wstring> &argv);
void CommandDestroyPhysical(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetPhysicalCategory(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetPhysicalImage(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetPhysicalAnimation(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetPhysicalPosition(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetPhysicalVelocity(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetEnemyAIScheme(size_t argc, const std::vector<std::wstring> &argv);
void CommandNewPlayer(size_t argc, const std::vector<std::wstring> &argv);
void CommandKillActor(size_t argc, const std::vector<std::wstring> &argv);
void CommandGetCurrAspect(size_t argc, const std::vector<std::wstring> &argv);
void CommandGetBaseAspect(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetBaseAspect(size_t argc, const std::vector<std::wstring> &argv);
void CommandSwitchControls(size_t argc, const std::vector<std::wstring> &argv);
void CommandAddXP(size_t argc, const std::vector<std::wstring> &argv);
void CommandGodMode(size_t argc, const std::vector<std::wstring> &argv);
void CommandDisplayEffectDebugData(size_t argc, const std::vector<std::wstring> &argv);
void CommandKillAll(size_t argc, const std::vector<std::wstring> &argv);
void CommandDisplayAbilities(size_t argc, const std::vector<std::wstring> &argv);
void CommandBuyAbility(size_t argc, const std::vector<std::wstring> &argv);
void CommandAddSkillPoints(size_t argc, const std::vector<std::wstring> &argv);
void CommandForceBuyAbility(size_t argc, const std::vector<std::wstring> &argv);
void CommandPlayCutscene(size_t argc, const std::vector<std::wstring> &argv);
void CommandDefineQuestFlag(size_t argc, const std::vector<std::wstring> &argv);
void CommandUndefQuestFlag(size_t argc, const std::vector<std::wstring> &argv);
void CommandGetPinnedAbis(size_t argc, const std::vector<std::wstring> &argv);
void CommandRandomizeDefaultAttack(size_t argc, const std::vector<std::wstring> &argv);
void CommandDoDefaultAttack(size_t argc, const std::vector<std::wstring> &argv);
void CommandShowFPS(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetActiveAbility(size_t argc, const std::vector<std::wstring> &argv);
void CommandStartDialog(size_t argc, const std::vector<std::wstring> &argv);
void CommandStartBossFight(size_t argc, const std::vector<std::wstring> &argv);
void CommandShowBoidDebug(size_t argc, const std::vector<std::wstring> &argv);
void CommandAddAchievement(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetSlotAbility(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetDifficultyFactor(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetScore(size_t argc, const std::vector<std::wstring> &argv);
void CommandShowScore(size_t argc, const std::vector<std::wstring> &argv);
void CommandAddScore(size_t argc, const std::vector<std::wstring> &argv);
//void CommandSetArcadeMode(size_t argc, const std::vector<std::wstring> &argv);
void CommandBestiaryAdd(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetBoss(size_t argc, const std::vector<std::wstring> &argv);

#endif//__MISC_COMMANDS_H__

