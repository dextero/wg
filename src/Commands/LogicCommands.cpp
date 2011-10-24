#include "LogicCommands.h"
#include "CCommands.h"
#include "../CGame.h"
#include "../CTimeManipulator.h"
#include "../Console/CConsole.h"
#include "../Utils/StringUtils.h"
#include "../Logic/CPhysicalManager.h"
#include "../Logic/CPhysical.h"
#include "../Logic/CPlayer.h"
#include "../Logic/CEnemy.h"
#include "../Logic/CLogic.h"
#include "../Logic/Effects/CAppliedEffect.h"
#include "../Logic/Effects/CExecutionContext.h"
#include "../Logic/Abilities/SAbilityInstance.h"
#include "../Logic/Abilities/CAbilityTree.h"
#include "../Logic/Abilities/CAbility.h"
#include "../Logic/Abilities/CPinnedAbilityBatch.h"
#include "../Logic/AI/CActorAI.h"
#include "../Logic/AI/CWandererScheme.h"
#include "../Logic/AI/CAISchemeManager.h"
#include "../Logic/Stats/EAspect.h"
#include "../Logic/Conditions/CCondition.h"

#include "../ResourceManager/CResourceManager.h"
#include "../Logic/Factory/CPhysicalTemplate.h"
#include "../Logic/Factory/CLootTemplate.h"
#include "../Logic/Factory/CDoorTemplate.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/Effects/CEffectManager.h"

#include "../Input/CInputHandler.h"
#include "../Input/CBindManager.h"
#include "../Input/CPlayerController.h"

#include "../Logic/CNPC.h"
#include "../Logic/CDialogGraph.h"
#include "../Logic/Quests/CQuestManager.h"
#include "../GUI/Dialogs/CDialogDisplayer.h"
#include "../GUI/Cutscene/CCutscenePlayer.h"

#include "../Logic/Boss/CBossManager.h"

#include "../Logic/Achievements/CAchievementManager.h"
#include "../Logic/CBestiary.h"

#include "../Logic/Loots/CLoot.h"

#include <SFML/Graphics/Image.hpp>

#include <sstream>
#include <fstream>

void CommandClearPhysicals(size_t argc, const std::vector<std::wstring> &argv);
void CommandSetAbility(size_t argc, const std::vector<std::wstring> &argv);
void CommandSpawnLoot(size_t argc, const std::vector<std::wstring> &argv);
void CommandAddGold(size_t argc, const std::vector<std::wstring> &argv);
void CommandOpenDoors(size_t argc, const std::vector<std::wstring> &argv);

// na koncu musi byc {0,0,0}, bo sie wszystko ***
CCommands::SCommandPair LogicCommands [] =
{
    {L"set-physical-category"               , "$MAN_SET_PHYSICAL_CATEGORY"      , CommandSetPhysicalCategory     },
    {L"set-physical-image"                  , "$MAN_SET_PHYSICAL_IMAGE"         , CommandSetPhysicalImage        },
    {L"set-physical-animation"              , "$MAN_SET_PHYSICAL_ANIMATION"     , CommandSetPhysicalAnimation    },
    {L"set-physical-position"               , "$MAN_SET_PHYSICAL_POSITION"      , CommandSetPhysicalPosition     },
    {L"set-physical-velocity"               , "$MAN_SET_PHYSICAL_VELOCITY"      , CommandSetPhysicalVelocity     },
    {L"spawn-loot"                          , "$MAN_SPAWN_LOOT"                 , CommandSpawnLoot               },
    {L"spawn-physical"                      , "$MAN_SPAWN_PHYSICAL"             , CommandSpawnPhysical           },
    {L"spawn-physical-rot"                  , "$MAN_SPAWN_PHYSICAL"             , CommandSpawnPhysicalRot        },
    {L"spawn-raw-physical"                  , "$MAN_SPAWN_RAW_PHYSICAL"         , CommandSpawnRawPhysical        },
    {L"spawn-weapon"                        , "$MAN_SPAWN_WEAPON"               , CommandSpawnWeapon             },
    {L"spawn-door"                          , "$MAN_SPAWN_DOOR"                 , CommandSpawnDoor               },
    {L"open-doors"                          , "$MAN_OPEN_DOORS"                 , CommandOpenDoors               },
    {L"destroy-physical"                    , "$MAN_DESTROY_PHYSICAL"           , CommandDestroyPhysical         },
    {L"set-ai-scheme"                       , "$MAN_SET_AI_SCHEME"              , CommandSetEnemyAIScheme        },
    {L"new-player"                          , "$MAN_NEW_PLAYER"                 , CommandNewPlayer               },
	{L"kill-actor"                          , "$MAN_KILL_ACTOR"                 , CommandKillActor               },
    {L"get-aspect"                          , "$MAN_GET_ASPECT"                 , CommandGetCurrAspect           },
    {L"get-base-aspect"                     , "$MAN_GET_BASE_ASPECT"            , CommandGetBaseAspect           },
    {L"set-aspect"                          , "$MAN_SET_ASPECT"                 , CommandSetBaseAspect           },
    {L"clear-physicals"                     , "$MAN_CLEAR_PHYSICALS"            , CommandClearPhysicals          },
    {L"switch-controls"                     , "$MAN_SWITCH_CONTROLS"            , CommandSwitchControls          },
    {L"add-xp"                              , "$MAN_ADD_XP"                     , CommandAddXP                   },
    {L"add-gold"                            , "$MAN_ADD_GOLD"                   , CommandAddGold                 },
    {L"god-mode"                            , "$MAN_GOD_MODE"                   , CommandGodMode                 },
    {L"display-effect-debug-data"           , "$MAN_DISPLAY_EFFECT_DEBUG_DATA"  , CommandDisplayEffectDebugData  },
    {L"kill-all"                            , "$MAN_KILL_ALL"                   , CommandKillAll                 },
    {L"display-abilities"                   , "$MAN_DISPLAY_ABILITIES"          , CommandDisplayAbilities        },
    {L"buy-ability"                         , "$MAN_BUY_ABILITY"                , CommandBuyAbility              },
    {L"set-ability"                         , "$MAN_SET_ABILITY"                , CommandSetAbility              },
    {L"add-skill-points"                    , "$MAN_ADD_SKILL_POINTS"           , CommandAddSkillPoints          },
	{L"force-buy-ability"					, "$MAN_FORCE_BUY_ABILITY"          , CommandForceBuyAbility		 },
	{L"play-cutscene"						, "$MAN_PLAY_CUTSCENE"              , CommandPlayCutscene			 },
	{L"define-quest-flag"					, "$MAN_DEFINE_QUEST_FLAG"          , CommandDefineQuestFlag		 },
	{L"undef-quest-flag"					, "$MAN_UNDEF_QUEST_FLAG"           , CommandUndefQuestFlag			 },
	{L"get-pinned-abis"						, "$MAN_GET_PINNED_ABIS"            , CommandGetPinnedAbis			 },
	{L"do-default-attack"					, "$MAN_DO_DEFAULT_ATTACK"          , CommandDoDefaultAttack		 },
	{L"randomize-default-attack"			, "$MAN_RANDOMIZE_DEFAULT_ATTACK"   , CommandRandomizeDefaultAttack  },
    {L"show-fps"                            , "$MAN_SHOW_FPS"                   , CommandShowFPS                 },
    {L"set-active-ability"                  , "$MAN_SET_ACTIVE_ABILITY"         , CommandSetActiveAbility        },
    {L"start-dialog"                        , "$MAN_START_DIALOG"               , CommandStartDialog             },
    {L"start-boss-fight"                    , "$MAN_START_BOSS_FIGHT"           , CommandStartBossFight          },
	{L"show-boid-debug"						, "$MAN_SHOW_BOID_DEBUG"			, CommandShowBoidDebug			 },
    {L"add-achievement"                     , "$MAN_ADD_ACHIEVEMENT"            , CommandAddAchievement          },
    {L"set-slot-ability"                    , "$MAN_SET_SLOT_ABILITY"           , CommandSetSlotAbility          },
    {L"set-difficulty-factor"               , "$MAN_SET_DIFFICULTY_FACTOR"      , CommandSetDifficultyFactor     },
    {L"set-score"                           , "$MAN_SET_SCORE"                  , CommandSetScore                },
    {L"show-score"                          , "$MAN_SHOW_SCORE"                 , CommandShowScore               },
    {L"add-score"                           , "$MAN_ADD_SCORE"                  , CommandAddScore                },
    //{L"set-arcade-mode"                     , "$MAN_SET_ARCADE_MODE"            , CommandSetArcadeMode           },
    {L"bestiary-add"                        , "$MAN_BESTIARY_ADD"               , CommandBestiaryAdd             },
    {L"set-boss"                            , "$MAN_SET_BOSS"                   , CommandSetBoss                 },
    {0,0,0}
};

void spawnPhysical(std::string &templName, std::wstring &id, float x, float y, int rot){
    CPhysicalTemplate* templ = gResourceManager.GetPhysicalTemplate(templName);

    if (templ == NULL){
        gConsole.Printf(L"Couldn't find template %ls.\n",templName.c_str());
        return;
    }

    CPhysical * physical = templ->Create( id );
	if ( physical ){
        physical->SetPosition( sf::Vector2f( x, y ) );
		physical->SetRotation( rot );
	}
}

#include "../Map/CRandomMapGenerator.h"

void CommandSpawnLoot(size_t argc, const std::vector<std::wstring> &argv) {
    float x = 0;
    float y = 0;
    int level = 0;
	if (argc == 4) {
        x = StringUtils::Parse<float>(StringUtils::ConvertToString(argv[2]));
        y = StringUtils::Parse<float>(StringUtils::ConvertToString(argv[3]));
	} else if (argc == 5) {
        x = StringUtils::Parse<float>(StringUtils::ConvertToString(argv[2]));
        y = StringUtils::Parse<float>(StringUtils::ConvertToString(argv[3]));
		level = StringUtils::Parse<int>(StringUtils::ConvertToString(argv[4]));
	} else {
        gConsole.Printf(L"usage: %ls filename x y [level]", argv[0].c_str());
        return;
	}
	std::string templName = StringUtils::ConvertToString(argv[1]);
    CLootTemplate* templ = dynamic_cast<CLootTemplate*>(gResourceManager.GetPhysicalTemplate(templName));

    if (templ == NULL) {
        gConsole.Printf(L"Couldn't find template %ls.\n",templName.c_str());
        return;
    }

    CLoot * loot = templ->Create();
	if (loot) {
        loot->SetPosition(sf::Vector2f(x, y));
        loot->SetLevel(level);
	}
}

void CommandSpawnRandomLoot(size_t argc, const std::vector<std::wstring> &argv){
	if (argc < 3) {
        gConsole.Printf( L"usage: %ls x y" , argv[0].c_str() );
		return;
	}
    float x = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[1] ) );
    float y = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[2] ) );
	sf::Vector2f pos(x, y);
    CLoot * loot = gRandomMapGenerator.GenerateNextLoot(0, pos);
    if (!loot) return;
	loot->SetPosition(pos);
}

void CommandSpawnPhysical(size_t argc, const std::vector<std::wstring> &argv){
    std::wstring id = L"";
    float x = 0;
    float y = 0;
	if ( argc == 3 ){
		id = argv[2];
	} else if ( argc == 4 ) {
        x = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[2] ) );
        y = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[3] ) );
    } else if ( argc == 5 ){
        id = argv[2];
        x = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[3] ) );
        y = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[4] ) );
    } else {
        gConsole.Printf( L"usage: %ls scheme [id]" , argv[0].c_str() );
        gConsole.Printf( L"usage: %ls scheme [x y]" , argv[0].c_str() );
        return;
    }
	std::string templName = StringUtils::ConvertToString( argv[1]);
	spawnPhysical(templName, id, x, y, 0);
}

void CommandSpawnPhysicalRot(size_t argc, const std::vector<std::wstring> &argv){
    std::wstring id = L"";
    float x = 0;
    float y = 0;
	int rot = 0;
	if ( argc == 5 ){
        x = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[2] ) );
        y = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[3] ) );
		rot = StringUtils::Parse<int>( StringUtils::ConvertToString( argv[4] ) );
	} else if ( argc == 6 ){
        id = argv[2];
        x = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[3] ) );
        y = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[4] ) );
		rot = StringUtils::Parse<int>( StringUtils::ConvertToString( argv[5] ) );
	} else {
        gConsole.Printf( L"usage: %ls templateName [id] x y rot" , argv[0].c_str() );
        return;
	}
	std::string templName = StringUtils::ConvertToString( argv[1]);
	spawnPhysical(templName, id, x, y, rot);
}

void CommandSpawnRawPhysical(size_t argc, const std::vector<std::wstring> &argv)
{
    std::wstring id = L"";
    if ( argc >= 2 )
        id =  argv[1];

    gPhysicalManager.CreatePhysical( id );
}

void CommandSpawnWeapon(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 4)
    {
        gConsole.Printf(L"usage: %ls weapon.xml x y", argv[0].c_str());
        return;
    }

    std::string lootTplFile = "data/loots/weapon.xml";
    CPhysicalTemplate* lootTpl = gResourceManager.GetPhysicalTemplate(lootTplFile);
    if (!lootTpl)
    {
        gConsole.Printf(L"Error: couldn't load physical template %s", lootTplFile.c_str());
        return;
    }

    CLoot* loot = (CLoot*)lootTpl->Create();
    if (!loot)
    {
        gConsole.Printf(L"Error: couldn't create loot");
        return;
    }

    loot->SetPosition(sf::Vector2f(StringUtils::Parse<float>(argv[2]), StringUtils::Parse<float>(argv[3])), true);
    loot->SetAbility(StringUtils::ConvertToString(argv[1]));
}

void CommandSpawnDoor(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 4)
    {
        gConsole.Printf(L"usage: %ls door.xml x y [opened|true|false(default)]", argv[0].c_str());
        return;
    }

    CDoorTemplate* doorTpl = dynamic_cast<CDoorTemplate*>(gResourceManager.GetPhysicalTemplate(StringUtils::ConvertToString(argv[1])));
    if (!doorTpl)
    {
        gConsole.Printf(L"invalid door xml file");
        return;
    }

    CDoor* door = doorTpl->Create();
    door->SetPosition(sf::Vector2f(StringUtils::Parse<float>(argv[2]), StringUtils::Parse<float>(argv[3])), true);
    bool open = argc > 4 && (argv[4] == L"opened" || argv[4] == L"true");
    door->SetState(open ? CDoor::dsOpened : CDoor::dsClosed);

}

void CommandOpenDoors(size_t argc, const std::vector<std::wstring> &argv)
{
    const std::vector<CPhysical *> &phys = gPhysicalManager.GetPhysicals();
    for (size_t i = 0; i < phys.size(); i++) {
        if (phys[i]->GetCategory() & PHYSICAL_DOOR) {
            CDoor * door = dynamic_cast<CDoor*>(phys[i]);
            if (door) {
                door->SetState(CDoor::dsOpened);
            }
        }
    }
}

void CommandDestroyPhysical(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 2 )
    {
        gConsole.Printf( L"usage: %ls [physicalId]", argv[0].c_str() );
        return;
    }

    gPhysicalManager.DestroyPhysical( argv[1] );
}

void CommandClearPhysicals(size_t argc, const std::vector<std::wstring> &argv)
{
    gPhysicalManager.Clear();
}

void CommandSetPhysicalCategory(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 3 ) return;

    CPhysical* physical = gPhysicalManager.GetPhysicalById( argv[1] );

    if ( !physical ) return;

    physical->SetCategory( argv[2] );
}

void CommandSetPhysicalImage(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 3 ) return;

    CPhysical* physical = gPhysicalManager.GetPhysicalById( argv[1] );

    if ( !physical ) return;

    physical->SetImage( StringUtils::ConvertToString( argv[2] ) );
}

void CommandSetPhysicalAnimation(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 3 ) return;

    CPhysical* physical = gPhysicalManager.GetPhysicalById( argv[1] );

    if ( !physical ) return;

    physical->SetAnimation( StringUtils::ConvertToString( argv[2] ) );
}

void CommandSetPhysicalPosition(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 4 ) return;

    CPhysical* physical = gPhysicalManager.GetPhysicalById( argv[1] );

    if ( !physical ) return;

    float x, y;
    x = y = 0.0f;
    swscanf( argv[2].c_str() , L"%f", &x );
    swscanf( argv[3].c_str() , L"%f", &y );

    physical->SetPosition( sf::Vector2f( x, y ) );
}

void CommandSetPhysicalVelocity(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 4 ) return;

    CPhysical* physical = gPhysicalManager.GetPhysicalById( argv[1] );

    if ( !physical ) return;

    float x, y;
    x = y = 0.0f;
    swscanf( argv[2].c_str() , L"%f", &x );
    swscanf( argv[3].c_str() , L"%f", &y );

    physical->SetVelocity( sf::Vector2f( x, y ) );
}

void CommandSetEnemyAIScheme(size_t argc, const std::vector<std::wstring> &argv) {
    if (argc < 3) {
        gConsole.Printf( L"usage: %ls id scheme" , argv[0].c_str() );
        return;
    }

    CEnemy* enemy= dynamic_cast<CEnemy*>(gPhysicalManager.GetPhysicalById( argv[1] ));

    if ( !enemy){
        gConsole.Printf(L"enemy %ls not found",argv[1].c_str());
        return;
    }

    CAIScheme *scheme = gAISchemeManager.GetScheme(argv[2]);

    if (!scheme){
        gConsole.Printf(L"scheme %ls not found",argv[2].c_str());
        return;
    }

    enemy->GetAI()->SetScheme(scheme);
}

void CommandNewPlayer(size_t argc, const std::vector<std::wstring> &argv){

	unsigned playerNumber;
	if ( argc < 2 )	playerNumber = 0;
	else			playerNumber = StringUtils::Parse<unsigned>(argv[1]);

	gPlayerManager.LoadPlayer( playerNumber );
}

void CommandKillActor(size_t argc, const std::vector<std::wstring> &argv){
	if (argc < 2){
        gConsole.Printf( L"usage: %ls id" , argv[0].c_str() );
        return;
    }

    CActor* actor= dynamic_cast<CActor*>(gPhysicalManager.GetPhysicalById( argv[1]));

    if ( !actor){
        gConsole.Printf(L"actor %ls not found",argv[1].c_str());
        return;
    }

    actor->Kill();
}

void CommandGetBaseAspect(size_t argc, const std::vector<std::wstring> &argv){
	if (argc < 3){
        gConsole.Printf( L"usage: %ls id aspect" , argv[0].c_str());
        return;
    }

    CActor* actor= dynamic_cast<CActor*>(gPhysicalManager.GetPhysicalById( argv[1] ));

    if ( !actor){
        gConsole.Printf(L"actor %ls not found",argv[1].c_str());
        return;
    }

    EAspect aspect = parseAspect(argv[2]);
    if (aspect == aNoAspect){
        gConsole.Printf(L"unrecognised aspect: %ls",argv[2].c_str());
        gConsole.Printf(L"Available aspects: %ls",availableAspects().c_str());
        return;
    }

    gConsole.Printf(L"%ls base value: %lf",argv[2].c_str(),actor->GetStats()->GetBaseAspect(aspect));
}

void CommandGetCurrAspect(size_t argc, const std::vector<std::wstring> &argv){
	if (argc < 3){
        gConsole.Printf( L"usage: %ls id aspect" , argv[0].c_str());
        return;
    }

    CActor* actor= dynamic_cast<CActor*>(gPhysicalManager.GetPhysicalById( argv[1] ));

    if ( !actor){
        gConsole.Printf(L"actor %ls not found",argv[1].c_str());
        return;
    }

    EAspect aspect = parseAspect(argv[2]);
    if (aspect == aNoAspect){
        gConsole.Printf(L"unrecognised aspect: %ls",argv[2].c_str());
        gConsole.Printf(L"Available aspects: %ls",availableAspects().c_str());
        return;
    }

    gConsole.Printf(L"%ls current value: %lf",argv[2].c_str(),actor->GetStats()->GetCurrAspect(aspect));
}

void CommandSetBaseAspect(size_t argc, const std::vector<std::wstring> &argv){
	if (argc < 4){
        gConsole.Printf( L"usage: %ls id aspect value" , argv[0].c_str());
        return;
    }

    CActor* actor= dynamic_cast<CActor*>(gPhysicalManager.GetPhysicalById( argv[1] ));

    if ( !actor){
        gConsole.Printf(L"actor %ls not found",argv[1].c_str());
        return;
    }

    EAspect aspect = parseAspect(argv[2]);
    if (aspect == aNoAspect){
        gConsole.Printf(L"unrecognised aspect: %ls",argv[2].c_str());
        gConsole.Printf(L"Available aspects: %ls",availableAspects().c_str());
        return;
    }

    float value = 0.0f;
    if (swscanf(argv[3].c_str(),L"%f",&value) < 1){
        gConsole.Printf(L"failed to parse value: %ls",argv[3].c_str());
        return;
    }

    actor->GetStats()->SetBaseAspect(aspect,value);
}

void CommandSwitchControls(size_t argc, const std::vector<std::wstring> &argv){
    gBindManager0->NextBindManager();
}

void CommandAddXP(size_t argc, const std::vector<std::wstring> &argv){
	if (argc < 2){
        gConsole.Printf( L"usage: %ls value [ignore-skill-points] [silent]" , argv[0].c_str());
        return;
    }

    float value = StringUtils::Parse<float>(argv[1]);

    bool ignoreSkillPoints = false, muteSound = false;
    for (size_t i = 1; i < argc; ++i)
    {
        if (argv[i] == L"ignore-skill-points")
            ignoreSkillPoints = true;
        else if (argv[i] == L"silent")
            muteSound = true;
    }

    gPlayerManager.XPGained(value, ignoreSkillPoints, muteSound);
}

void CommandAddGold(size_t argc, const std::vector<std::wstring> &argv){
	if (argc < 3){
        gConsole.Printf( L"usage: %ls playerId value" , argv[0].c_str());
        return;
    }

    int amount = StringUtils::Parse<float>(argv[2]);
    CPlayer* player= dynamic_cast<CPlayer*>(gPhysicalManager.GetPhysicalById(argv[1]));
    if (!player) {
        gConsole.Printf(L"player %ls not found", argv[1].c_str());
        return;
    }
    player->SetGold(player->GetGold() + amount);
    // todo: play a sound?
}

void CommandGodMode(size_t argc, const std::vector<std::wstring> &argv){
    if (argc < 2){
//        gConsole.Printf( L"usage:... nah, aint tellin' ;)");
//        ja ci kurna dam aint tellin - i potem musze zagladac do zrodel <nonono>
        gConsole.Printf( L"usage: god-mode [on|off]");

    }

    if (argc > 1 && argv[1] == L"off"){
        gPlayerManager.SetGodMode(false);
        gConsole.Printf(L"immortality lost");
    }else {
        gPlayerManager.SetGodMode(true);
        gConsole.Printf(L"immortality acquired");
        if (gConsole.GetVisible())
            gCommands.ParseCommand(L"add-achievement $ACH_CHEATER");
    }
}


void CommandDisplayEffectDebugData(size_t argc, const std::vector<std::wstring> &argv){
    gConsole.Printf(L"Total CPhysical instances: %d",gPhysicalManager.GetPhysicals().size());
    gConsole.Printf(L"Total CEffectSource instances: %d",CEffectSource::GetInstanceCount());
    gConsole.Printf(L"Total CAppliedEffect instances: %d",CAppliedEffect::GetInstanceCount());
    gConsole.Printf(L"Total CExecutionContext instances: %d",CExecutionContext::GetInstanceCount());
}

void CommandKillAll(size_t argc, const std::vector<std::wstring> &argv){
    if (argc < 2){
        gConsole.Printf( L"usage: %ls filter",argv[0].c_str());
        gConsole.Printf( L"example: %ls enemies",argv[0].c_str());
        return;
    }
    int filter = ParsePhysicalFilter(StringUtils::ConvertToString(argv[1]));
    const std::vector<CPhysical *> &phys = gPhysicalManager.GetPhysicals();
    for (unsigned int i = 0; i < phys.size(); i++){
        if (phys[i]->GetCategory() & filter){
            phys[i]->Kill();
        }
    }
}

void CommandDisplayAbilities(size_t argc, const std::vector<std::wstring> &argv){
    if (argc < 2){
        gConsole.Printf( L"usage: %ls id" , argv[0].c_str());
        return;
    }

    CPlayer* player= dynamic_cast<CPlayer*>(gPhysicalManager.GetPhysicalById( argv[1] ));

    if ( !player){
        gConsole.Printf(L"player %ls not found",argv[1].c_str());
        return;
    }

    std::wstringstream out;

    out << "HP: " << player->GetStats()->GetHP() << "/" << player->GetStats()->GetCurrAspect(aMaxHP) << "\n";
    out << "mana: " << player->GetStats()->GetMana() << "/" << player->GetStats()->GetCurrAspect(aMaxMana) << "\n";
	out << "speed: " << player->GetStats()->GetCurrAspect(aSpeed) << ":" << player->GetStats()->GetBaseAspect(aSpeed) << "\n";
    out << "aPhysResist: " << player->GetStats()->GetCurrAspect(aPhysResist) << ":" << player->GetStats()->GetBaseAspect(aPhysResist) << "\n";
    out << "aFireResist: " << player->GetStats()->GetCurrAspect(aFireResist) << ":" << player->GetStats()->GetBaseAspect(aFireResist) << "\n";
    out << "aIceResist: " << player->GetStats()->GetCurrAspect(aIceResist) << ":" << player->GetStats()->GetBaseAspect(aIceResist) << "\n";
    out << "aLightningResist: " << player->GetStats()->GetCurrAspect(aLightningResist) << ":" << player->GetStats()->GetBaseAspect(aLightningResist) << "\n";
    out << "aPoisonResist: " << player->GetStats()->GetCurrAspect(aPoisonResist) << ":" << player->GetStats()->GetBaseAspect(aPoisonResist) << "\n";
    std::vector<CAbilityTree*> *trees = player->GetBoundAbilityTrees();
    std::vector<SAbilityInstance> *abilities = player->GetAbilityPerformer().GetAbilities();
    for (unsigned int i = 0; i < trees->size(); i++){
        CAbilityTree *at = trees->at(i);
        out << at->GetName() << L": ";
        for (unsigned int j = 0; j < at->GetAbilityNodes().size(); j++){
            const SAbilityNode *an = &(at->GetAbilityNodes()[j]);
            bool found = false;
            if (j != 0)
                out << L"; ";
            for (unsigned int abi = 0; abi < abilities->size(); abi++)
                if (abilities->at(abi).abiNode == an){
                    out << abilities->at(abi).ability->name << L"[" << StringUtils::ConvertToWString(an->code) << L"]: "
                        << abilities->at(abi).level;
                    found = true;
                    break;
                }
            if (!found && an != NULL)
                out << an->ability->name << L"[" << StringUtils::ConvertToWString(an->code) << L"]: 0";
        }
        gConsole.Printf(out.str().c_str());
        out.clear();
    }
}


#include "../GUI/Messages/CMessageSystem.h"
#include "../Logic/Items/CItem.h"

void CommandSetAbility(size_t argc, const std::vector<std::wstring> &argv){
    if (argc < 4){
        gConsole.Printf( L"usage: %ls id ability.xml trigger [level = 1]" , argv[0].c_str());
        return;
    }

    CPlayer* player= dynamic_cast<CPlayer*>(gPhysicalManager.GetPhysicalById( argv[1] ));

    if (!player){
        gConsole.Printf(L"player %ls not found",argv[1].c_str());
        return;
    }

    std::string abilityName = StringUtils::ConvertToString(argv[2]).c_str();

    size_t index = StringUtils::Parse<int>(StringUtils::ConvertToString(argv[3]));
    if (index > 3) index = 3;

    int level = 1;
    if (argc > 4)
        level = StringUtils::Parse<int>(StringUtils::ConvertToString(argv[4]));
    if (level < 1)
        level = 1;

//    CItem * item = player->GetItem(index);
//    if (item != NULL) {
//        item->SetAbility(abilityName);
//        return;
//    }

    CItem * item = new CItem();
    item->SetAbility(abilityName);
    item->mLevel = level;
    player->AddItem(item, index);


//    CAbility * ability = gResourceManager.GetAbility( abilityName );
//    if ( !ability ) {
//        gConsole.Printf(L"ability %s not found", abilityName.c_str());
//        return;
//    }
//
//    ability->trigger = StringUtils::ConvertToString(argv[3]).c_str();
//    SAbilityInstance ai(ability,1);
//    int animCode = 1;
//    player->GetAbilityPerformer().AddOrSwapAbilityWithTrigger(ai, ability->trigger, animCode);
//    gMessageSystem.AddMessage(StringUtils::ConvertToWString(ability->trigger + " " + abilityName).c_str());

//    player->GetAbilityPerformer().AddAbility(ai, animCode);


}

void CommandBuyAbility(size_t argc, const std::vector<std::wstring> &argv){
    if (argc < 4){
        gConsole.Printf( L"usage: %ls id tree-code abi-code" , argv[0].c_str());
        return;
    }

    CPlayer* player= dynamic_cast<CPlayer*>(gPhysicalManager.GetPhysicalById( argv[1] ));

    if ( !player){
        gConsole.Printf(L"player %ls not found",argv[1].c_str());
        return;
    }

    int treeId = player->FindAbilityTree(StringUtils::ConvertToString(argv[2]));
    if (treeId < 0){
        gConsole.Printf(L"ability tree %ls not found in player %ls\n",argv[2].c_str(),argv[1].c_str());
        return;
    }
    int abiId = player->FindAbilityInTree(treeId,StringUtils::ConvertToString(argv[3]));
    if (abiId < 0){
        gConsole.Printf(L"ability %ls not found in player's %ls tree %ls\n",argv[3].c_str(),argv[1].c_str(),argv[2].c_str());
        return;
    }
    EAbilityBuyingResult abr = player->TryBuyAbilityLevel(treeId,abiId);
    switch(abr){
        case abrLevelReqNotMet:
            gConsole.Printf(L"player level not high enough");
            break;
        case abrNoPoints:
            gConsole.Printf(L"not enough skill points available");
            break;
        case abrOK:
            gConsole.Printf(L"ability bought!");
            break;
        case abrTooLowPreqLevel:
            gConsole.Printf(L"too low prequisitive level");
            break;
        default:
            break;
    }
}

void CommandAddSkillPoints(size_t argc, const std::vector<std::wstring> &argv){

    if (argc < 3){
        gConsole.Printf( L"usage: %ls id amount" , argv[0].c_str());
        return;
    }

    CPlayer* player= dynamic_cast<CPlayer*>(gPhysicalManager.GetPhysicalById( argv[1] ));

    if ( !player){
        gConsole.Printf(L"player %ls not found",argv[1].c_str());
        return;
    }

    int out;
    if (StringUtils::FromString<int>(StringUtils::ConvertToString(argv[2]),out)==false){
        gConsole.Printf(L"failed to parse %ls as integer",argv[2].c_str());
        return;
    }
    player->AddSkillPoints(out);
}

void CommandForceBuyAbility(size_t argc, const std::vector<std::wstring> &argv){
    if (argc < 4){
        gConsole.Printf( L"usage: %ls id tree-code abi-code [count]" , argv[0].c_str());
        return;
    }

    CPlayer* player= dynamic_cast<CPlayer*>(gPhysicalManager.GetPhysicalById( argv[1] ));

    if ( !player){
        gConsole.Printf(L"player %ls not found",argv[1].c_str());
        return;
    }

    int treeId = player->FindAbilityTree(StringUtils::ConvertToString(argv[2]));
    if (treeId < 0){
        gConsole.Printf(L"ability tree %ls not found in player %ls\n",argv[2].c_str(),argv[1].c_str());
        return;
    }
    int abiId = player->FindAbilityInTree(treeId,StringUtils::ConvertToString(argv[3]));
    if (abiId < 0){
        gConsole.Printf(L"ability %ls not found in player's %ls tree %ls\n",argv[3].c_str(),argv[1].c_str(),argv[2].c_str());
        return;
    }

	int cnt = 1;
	if (argc > 4)
		if (!StringUtils::FromString(StringUtils::ConvertToString(argv[4]),cnt))
			cnt = 1;
	for (int i = 0; i < cnt; i++)
		player->AdvanceAbilityLevel(treeId,abiId);
}

void CommandPlayCutscene(size_t argc, const std::vector<std::wstring> &argv)
{
	if ( argc < 2 )
	{
        gConsole.Printf( L"usage: %ls cutscene-path" , argv[0].c_str() );
        return;
	}

	gCutscenePlayer.PlayCutscene( StringUtils::ConvertToString( argv[1] ) );
}

void CommandDefineQuestFlag(size_t argc, const std::vector<std::wstring> &argv)
{
	if ( argc < 2 )
	{
		gConsole.Printf( L"usage: define-quest-flag flag" );
		return;
	}

	gQuestManager.DefineFlag( argv[1] );
}

void CommandUndefQuestFlag(size_t argc, const std::vector<std::wstring> &argv)
{
	if ( argc < 2 )
	{
		gConsole.Printf( L"usage: undef-quest-flag flag" );
		return;
	}

	gQuestManager.UndefFlag( argv[1] );
}

void CommandGetPinnedAbis(size_t argc, const std::vector<std::wstring> &argv)
{
	CPlayer *pl = gPlayerManager.GetPlayerByNumber(0);
	if (pl == NULL) return;

	CPinnedAbilityBatch *pab = pl->GetPinnedAbilityBatch();
	if (pab == NULL) return;

	const std::vector<CPinnedAbilityBatch::SExportedAbility> & pinned = pab->GetPinnedAbilities();
	for (unsigned int i = 0; i < pinned.size(); i++){
		gConsole.Printf((pinned[i].abi->name + L" - %d").c_str(),pinned[i].level);
	}
}

void CommandRandomizeDefaultAttack(size_t argc, const std::vector<std::wstring> &argv){
	if ( argc < 2 )
	{
		gConsole.Printf( L"usage: do-default-attack physical" );
		return;
	}
	CPhysical *phys = gPhysicalManager.GetPhysicalById(argv[1]);
	if ( phys == NULL )
	{
		gConsole.Printf( L"physical %s not found", argv[1].c_str() );
		return;
	}

	CActor *actor = dynamic_cast<CActor*>(phys);
	if (actor == NULL)
	{
		gConsole.Printf( L"physical %s is not an actor", argv[1].c_str() );
		return;
	}

	actor->GetAbilityPerformer().RandomizeDefaultAttack();
}

void CommandDoDefaultAttack(size_t argc, const std::vector<std::wstring> &argv){
	if ( argc < 2 )
	{
		gConsole.Printf( L"usage: do-default-attack physical" );
		return;
	}
	CPhysical *phys = gPhysicalManager.GetPhysicalById(argv[1]);
	if ( phys == NULL )
	{
		gConsole.Printf( L"physical %s not found", argv[1].c_str() );
		return;
	}

	CActor *actor = dynamic_cast<CActor*>(phys);
	if (actor == NULL)
	{
		gConsole.Printf( L"physical %s is not an actor", argv[1].c_str() );
		return;
	}

	actor->GetAbilityPerformer().DoDefaultAttack();
}

void CommandShowFPS(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argv.size() < 2)
        gGame.SetShowingFps(!gGame.GetShowingFps());
    else
        gGame.SetShowingFps(argv[1] == L"on" ? true : false);

    gConsole.Printf(L"Showing FPS: %s", (gGame.GetShowingFps() ? L"on" : L"off"));
}

void CommandSetActiveAbility(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argv.size() < 4)
        gConsole.Printf(L"Usage: set-active-ability player-number tree ability-id");
    else
    {
        CPlayer* player = gPlayerManager.GetPlayerByNumber(StringUtils::Parse<int>(StringUtils::ConvertToString(argv[1])));
        if (!player) {
            gConsole.Printf(L"Invalid player number");
            return;
        }
        int treeId = player->FindAbilityTree(StringUtils::ConvertToString(argv[2]));
        if (treeId == -1) {
            gConsole.Printf(L"Invalid tree name");
            return;
        }
        int abiId = player->FindAbilityInTree(treeId, StringUtils::ConvertToString(argv[3]));
        if (abiId == -1) {
            gConsole.Printf(L"Invalid ability id");
            return;
        }
        player->EnableTriggerInAbility(treeId, abiId);
    }
}

void CommandStartDialog(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2) {
        gConsole.Printf(L"Usage: start-dialog npc-id");
        return;
    }

    CNPC* npc = dynamic_cast<CNPC*>(gPhysicalManager.GetPhysicalById(argv[1]));

    if (!npc) {
        gConsole.Printf(L"%ls doesn't exist, or it is not a NPC", argv[1].c_str());
        return;
    }

    gDialogDisplayer.DisplayDialog(npc->GetDialogGraph());
}

void CommandStartBossFight( size_t argc, const std::vector<std::wstring> &argv )
{
    if (argc < 2) {
        gConsole.Printf(L"Usage: start-boss-fight enemy-id");
        return;
    }

    CEnemy * enemy = dynamic_cast<CEnemy *>(gPhysicalManager.GetPhysicalById(argv[1]));

    if (!enemy) {
        gConsole.Printf(L"%ls doesn't exist, or it is not an enemy", argv[1].c_str());
        return;
    }

    gBossManager.StartBossFight(enemy);
}

void CommandShowBoidDebug(size_t argc, const std::vector<std::wstring> &argv){
	if (argc < 2) {
        gConsole.Printf(L"Usage: show-boid-debug on|off");
        return;
    }

	std::wstring val = argv[1];
	if (val == L"on"){
		gAISchemeManager.SetBoidDebug(true);
	} else if (val == L"off"){
		gAISchemeManager.SetBoidDebug(false);
	} else {
		gConsole.Printf(L"Usage: show-boid-debug on|off");
	}
}

void CommandAddAchievement(size_t argc, const std::vector<std::wstring> &argv)
{
    gAchievementManager.AddAchievement(argc, argv);
}


void CommandSetSlotAbility(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 5)
        gConsole.Printf(L"usage: %s player-number slot-number tree ability", argv[0].c_str());

    unsigned int playerNumber = StringUtils::Parse<unsigned int>(argv[1]);
    CPlayer* player = gPlayerManager.GetPlayerByNumber(playerNumber);
    if (player == NULL)
    {
        gConsole.Printf(L"Invalid player number!");
        return;
    }

    unsigned int slot = StringUtils::Parse<unsigned int>(argv[2]);
    if (slot >= ABI_SLOTS_COUNT)
    {
        gConsole.Printf(L"Invalid slot number!");
        return;
    }

    int tree = player->FindAbilityTree(StringUtils::ConvertToString(argv[3]));
    if (tree == -1)
    {
        gConsole.Printf(L"Invalid tree name!");
        return;
    }

    int abi = player->FindAbilityInTree(tree, StringUtils::ConvertToString(argv[4]));
    if (abi == -1)
    {
        gConsole.Printf(L"Invalid ability code!");
        return;
    }

    // ..gdyby nie sprawdzanie poprawnosci parametrow, to wystarczylaby jedna linia :(
    gLogic.GetGameScreens()->SetSlotAbility(playerNumber, slot, player->GetBoundAbilityTrees()->at(tree)->GetAbilityNodes()[abi].ability);
}

void CommandSetDifficultyFactor(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2)
    {
        gConsole.Printf(L"usage: %s factor", argv[0].c_str());
        return;
    }

    gLogic.SetDifficultyFactor(StringUtils::Parse<float>(argv[1]));
}

void CommandSetScore(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2)
    {
        gConsole.Printf(L"usage: %s score", argv[0].c_str());
        return;
    }

    if (gConsole.GetVisible())
    {
        gConsole.Printf(L"What are you doing?\n");
        // ]:->
        gLogic.SetScore(0);
    }

    gLogic.SetScore(StringUtils::Parse<unsigned>(argv[1]));
}

void CommandShowScore(size_t argc, const std::vector<std::wstring> &argv)
{
    gConsole.Printf(L"%u", gLogic.GetScore());
}

void CommandAddScore(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2)
    {
        gConsole.Printf(L"usage: %s points", argv[0].c_str());
        return;
    }

    if (gConsole.GetVisible())
    {
        gConsole.Printf(L"Me no english, no touch.\n");
        // ]:->
        gLogic.SetScore(0);
    }

    gLogic.ScoreGained(StringUtils::Parse<unsigned>(argv[1]));
}

/*void CommandSetArcadeMode(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2)
        gConsole.Printf(L"usage: %s [on|off]", argv[0].c_str());

    if (argc > 1 && argv[1] == L"off")
        gLogic.SetArcadeMode(false);
    else
        gLogic.SetArcadeMode(true);

    gTimeManipulator.SetMode(gLogic.IsInArcadeMode() ? Time::SHIFT_TIME : Time::NORMAL_TIME);
    gConsole.Printf(L"Arcade mode %ls", (gLogic.IsInArcadeMode() ? L"ON" : L"OFF"));
}*/

void CommandBestiaryAdd(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2)
    {
        gConsole.Printf(L"usage: %s template-path", argv[0].c_str());
        return;
    }

    gBestiary.EnableMonster(StringUtils::ConvertToString(argv[1]));
}

void CommandSetBoss(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 4)
    {
        gConsole.Printf(L"usage: %s physical-id ai-scheme trigger-radius [playlist]", argv[0].c_str());
        return;
    }

    if (!gPhysicalManager.GetPhysicalById(argv[1]))
    {
        gConsole.Printf(L"invalid physical id: %s", argv[1].c_str());
        return;
    }

    if (!gAISchemeManager.GetScheme(argv[2]))
    {
        gConsole.Printf(L"invalid ai scheme: %s", argv[2].c_str());
        return;
    }

    float r = StringUtils::Parse<float>(argv[3]);
    if (r <= 0.f)
    {
        gConsole.Printf(L"invalid trigger radius: %f", argv[3].c_str());
        return;
    }

    gBossManager.Trace(argv[1], StringUtils::ConvertToString(argv[2]), r, (argc > 4 ? StringUtils::ConvertToString(argv[4]) : ""));
    gConsole.Printf(L"ok!");
}
