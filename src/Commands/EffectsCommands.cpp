#include "EffectsCommands.h"
#include "CCommands.h"
#include "../Console/CConsole.h"
#include "../Utils/StringUtils.h"
#include "../Rendering/Effects/CGraphicalEffects.h"
#include "../Logic/Effects/CEffectSource.h"
#include "../Logic/Effects/CAppliedEffect.h"
#include "../Logic/CPhysicalManager.h"
#include "../GUI/Localization/CLocalizator.h"

void CommandShowGraphicalEffect(size_t argc, const std::vector<std::wstring> &argv);
void CommandClearLootManager(size_t argc, const std::vector<std::wstring> &argv);
void CommandInitLootManager(size_t argc, const std::vector<std::wstring> &argv);

 // na koncu musi byc {0,0,0}, bo sie wszystko ***
CCommands::SCommandPair EffectsCommands [] =
{
    {L"effects-init"                  , "$MAN_EFFECTS_INIT"             , CommandInitialize },
    {L"show-graphical-effect"         , "$MAN_SHOW_GRAPHICAL_EFFECT"    , CommandShowGraphicalEffect },
    {L"load-loots"                    , "$MAN_LOAD_LOOTS"               , CommandInitLootManager },
    {L"clear-loots"                   , "$MAN_CLEAR_LOOTS"              , CommandClearLootManager },
    {L"add-to-pm"                     , "$MAN_ADD_TO_PM"                , CommandAddPhysicalToPM },
    {L"msg"                           , "$MAN_MSG"                      , CommandMessage },
    {0,0,0}
};

#include "../GUI/Messages/CMessageSystem.h"

void CommandMessage(size_t argc, const std::vector<std::wstring> &argv)
{
    
    std::wstring msg;
	if (argc >= 2)
        msg = gLocalizator.Localize(argv[1]);
    for (size_t i=2; i < argc; ++i) {
       msg += L" ";
       msg += gLocalizator.Localize(argv[i]);
    }
    gMessageSystem.AddMessage( msg.c_str() );
}

void CommandInitialize(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2)
    {
        gConsole.Print(L"usage: effects-init <effects-config.xml>");
        return;
    }
    else
    {
        if (!gGraphicalEffects.Initialize(StringUtils::ConvertToString(argv[1])))
            gConsole.Print(L"gGraphicalEffects: initialization failed :( Check log file");
        /* else
            gConsole.Print(L"initialization done :) Enjoy fireworks :)");*/
    }
}

void CommandShowGraphicalEffect(size_t argc, const std::vector<std::wstring> &argv)
{
    if ( argc < 2 )
    {
        gConsole.Printf( L"usage: %ls effect-name [<params>]", argv[0].c_str() );
        gConsole.Printf( L"for example:", argv[0].c_str() );
        gConsole.Printf( L"usage: %ls effect-name physical-from.id", argv[0].c_str() );
        gConsole.Printf( L"usage: %ls effect-name x y", argv[0].c_str() );
        gConsole.Printf( L"usage: %ls effect-name x y physical-from.id physical-to.id", argv[0].c_str() );
    }
    else
    {
        std::string effectName = StringUtils::ConvertToString( argv[1] );
        sf::Vector2f pos(0, 0);
        CPhysical * from = NULL;
        CPhysical * to = NULL;

        if ( argc == 3 )
        {
            from = gPhysicalManager.GetPhysicalById ( argv[2] );
        }
        else
        {
            pos.x = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[2] ) );
            pos.y = StringUtils::Parse<float>( StringUtils::ConvertToString( argv[3] ) );
            if ( argc > 4 )
                from = gPhysicalManager.GetPhysicalById ( argv[4] );
            if ( argc > 5 )
                to = gPhysicalManager.GetPhysicalById ( argv[5] );
        }

        SEffectParamSet eps = gGraphicalEffects.Prepare( effectName );
        gGraphicalEffects.ShowEffect( eps, pos, from, to );

    }
}

#include "../Rendering/Particles/CParticleManager.h"
#include "../Rendering/Particles/Affectors/CPointMassAffector.h"
void CommandAddPhysicalToPM(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2)
        gConsole.Print(L"add-to-pm <point mass name> <physical>");
    else do
    {
        part::affector::CPointMass * a = dynamic_cast<part::affector::CPointMass *> (part::gParticleManager.GetAffector( StringUtils::ConvertToString(argv[1]) ));
        
        if (a==NULL) {
            gConsole.Print(L"couldn't find point masss by that name");
            break;
        }

        CPhysical *p = gPhysicalManager.GetPhysicalById(argv[2]);
        if (p==NULL) {
            gConsole.Print(L"coldn't find physical");
            break;
        }

        a->AddPhysical(p);
        gConsole.Print(L"Operation successfully");
    } while(false);        
}

#include "../Logic/Loots/CLootManager.h"
#include "../Logic/Loots/CLoot.h"
void CommandInitLootManager(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc < 2) {
        gConsole.Print(L"Usage: load-loots <filename.xml>");
        return;
    }
    else
    {
        std::string filename = StringUtils::ConvertToString(argv[1]);
        if (!gLootManager.LoadLoots(filename)) {
            gConsole.Printf(L"Failed to load loots configuration from %s file", argv[1].c_str());
            return;
        }
    }
}

void CommandClearLootManager(size_t argc, const std::vector<std::wstring> &argv)
{
    gLootManager.Clear();
}
