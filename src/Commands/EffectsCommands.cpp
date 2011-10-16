#include "EffectsCommands.h"
#include "CCommands.h"
#include "../Console/CConsole.h"
#include "../Utils/StringUtils.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/SLight.h"
#include "../Rendering/Effects/CGraphicalEffects.h"
#include "../Logic/Effects/CEffectSource.h"
#include "../Logic/Effects/CAppliedEffect.h"
#include "../Logic/CPhysicalManager.h"
#include "../GUI/Localization/CLocalizator.h"
#include "../Map/CMap.h"

#include <SFML/Graphics/Color.hpp>

void CommandShowGraphicalEffect(size_t argc, const std::vector<std::wstring> &argv);

 // na koncu musi byc {0,0,0}, bo sie wszystko ***
CCommands::SCommandPair EffectsCommands [] =
{
    {L"effects-init"                  , "$MAN_EFFECTS_INIT"             , CommandInitialize },
    {L"show-graphical-effect"         , "$MAN_SHOW_GRAPHICAL_EFFECT"    , CommandShowGraphicalEffect },
    {L"add-to-pm"                     , "$MAN_ADD_TO_PM"                , CommandAddPhysicalToPM },
    {L"msg"                           , "$MAN_MSG"                      , CommandMessage },
	{L"set-lighting"				  , "$MAN_SET_LIGHTING"				, CommandSetLighting },
	{L"set-ambient"					  , "$MAN_SET_AMBIENT"				, CommandSetAmbient },
	{L"set-nm-ambient"				  , "$MAN_SET_NM_AMBIENT"			, CommandSetNMAmbient },
	{L"set-nm-contrast"				  , "$MAN_SET_NM_CONTRAST"			, CommandSetNMContrast },
	{L"create-light"				  , "$MAN_CREATE_LIGHT"				, CommandCreateLight },
	{L"destroy-all-lights"			  , "$MAN_DESTROY_ALL_LIGHTS"		, CommandDestroyAllLights },
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

void CommandSetLighting(size_t argc, const std::vector<std::wstring> &argv)
{
	if ( argc < 2 )
    {
        gConsole.Printf( L"usage: %ls <true|false>", argv[0].c_str() );
        return;
    }

	gDrawableManager.SetLighting(argv[1] == L"true");
}

void CommandSetAmbient(size_t argc, const std::vector<std::wstring> &argv)
{
	if ( argc < 4 )
    {
        gConsole.Printf( L"usage: %ls <R> <G> <B>", argv[0].c_str() );
        return;
    }

	gDrawableManager.SetAmbient(sf::Color(
		StringUtils::Parse<unsigned>(argv[1]),
		StringUtils::Parse<unsigned>(argv[2]),
		StringUtils::Parse<unsigned>(argv[3])
	));
}

void CommandSetNMAmbient(size_t argc, const std::vector<std::wstring> &argv)
{
	if ( argc < 4 )
    {
        gConsole.Printf( L"usage: %ls <R> <G> <B>", argv[0].c_str() );
        return;
    }

	gDrawableManager.SetNormalMappingAmbient(sf::Color(
		StringUtils::Parse<unsigned>(argv[1]),
		StringUtils::Parse<unsigned>(argv[2]),
		StringUtils::Parse<unsigned>(argv[3])
	));
}

void CommandSetNMContrast(size_t argc, const std::vector<std::wstring> &argv)
{
	if ( argc < 2 )
    {
		gConsole.Printf( L"usage: %ls <contrast(default:1.0)>", argv[0].c_str() );
        return;
    }

	gDrawableManager.SetNormalMappingContrast(StringUtils::Parse<float>(argv[1]));
}

void CommandCreateLight(size_t argc, const std::vector<std::wstring> &argv)
{
	if ( argc < 8 )
    {
		gConsole.Printf( L"usage: %ls <X> <Y> <Z> <radius> <R> <G> <B>", argv[0].c_str() );
        return;
    }

	SLight* light = gDrawableManager.CreateLight();
	light->mPosition = sf::Vector3f(
		StringUtils::Parse<float>(argv[1]) * Map::TILE_SIZE,
		StringUtils::Parse<float>(argv[2]) * Map::TILE_SIZE,
		StringUtils::Parse<float>(argv[3]) * Map::TILE_SIZE
	);
	light->mRadius = StringUtils::Parse<float>(argv[4]) * Map::TILE_SIZE;
	light->mColor = sf::Color(
		StringUtils::Parse<unsigned>(argv[5]),
		StringUtils::Parse<unsigned>(argv[6]),
		StringUtils::Parse<unsigned>(argv[7])
	);
}

void CommandDestroyAllLights(size_t argc, const std::vector<std::wstring> &argv)
{
	gDrawableManager.DestroyAllLights();
}
