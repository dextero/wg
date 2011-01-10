#include "CameraCommands.h"
#include "CCommands.h"
#include "../Console/CConsole.h"
#include "../Utils/StringUtils.h"
#include "../Logic/CPhysicalManager.h"
#include "../Rendering/CCamera.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/CPlayer.h"

class CPhysical;

 // na koncu musi byc {0,0,0}, bo sie wszystko ***
CCommands::SCommandPair CameraCommands [] =
{
    {L"trail-physical"                  , "$MAN_TRAIL_PHYSICAL"         , CommandTrailPhysical },
	{L"trail-stop"                      , "$MAN_TRAIL_STOP"             , CommandTrailStop },
    {L"set-camera-position"             , "$MAN_SET_CAMERA_POSITION"    , CommandSetCameraPosition },
    {L"trail-players"                   , "$MAN_TRAIL_PLAYERS"          , CommandTrailPlayers },
    {L"camera-sway-to"                  , "$MAN_CAMERA_SWAY_TO"         , CommandCameraSway },
    {0,0,0}
};


void CommandTrailPhysical(size_t argc, const std::vector<std::wstring> &argv){
    if (( argc < 2) || ((argc >= 3) && (argv[2] != L"sway")))
    {
        gConsole.Printf( L"usage: %ls name [sway [t]]" , argv[0].c_str() );
        return;
    }

    if (argc <= 2)
        gCamera.TrailPhysical(gPhysicalManager.GetPhysicalById(argv[1]));
    else {
        float t = -1.0f;
        if (argc >= 4)
            swscanf( argv[3].c_str() , L"%f", &t );
        gCamera.TrailPhysical(gPhysicalManager.GetPhysicalById(argv[1]),t);
    }
}

void CommandTrailStop(size_t argc, const std::vector<std::wstring> &argv){
    gCamera.StopCamera();
}

void CommandSetCameraPosition( size_t argc, const std::vector<std::wstring> &argv )
{
    if ( argc < 3 )
    {
        gConsole.Printf( L"usage: %ls <x> <y>", argv[0].c_str() );
        return;
    }

    float x, y;
    x = y = 0.0f;
    swscanf( argv[1].c_str() , L"%f", &x );
    swscanf( argv[2].c_str() , L"%f", &y );

	gCamera.GoTo( x, y );
}

void CommandCameraSway(size_t argc, const std::vector<std::wstring> &argv ){
    if (argc < 3)
    {
        gConsole.Printf(L"usage: %ls <x> <y> [time]",argv[0].c_str());
        return;
    }
    float x,y,t = -1.0f;
    x = y = 0.0f;
    swscanf( argv[1].c_str() , L"%f", &x );
    swscanf( argv[2].c_str() , L"%f", &y );
    if (argc >= 4)
        swscanf( argv[3].c_str() , L"%f", &t );
    gCamera.GoTo(x,y,t);
}

void CommandTrailPlayers(size_t argc, const std::vector<std::wstring> &argv ){
    gCamera.TrailPlayers();
}

