#include "ToxicUtils.h"

#ifdef __I_AM_TOXIC__

#include "../Console/CConsole.h"
#include "../Commands/CCommands.h"
#include "../Rendering/CCamera.h"
#include "../CGame.h"
#include "../Map/CMap.h"
#include "StringUtils.h"
#include "CRand.h"
#include "../Logic/CPhysicalManager.h"
#include "../Logic/CPhysical.h"
#include "../ResourceManager/CResourceManager.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Input.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <fstream>

namespace ToxicUtils {

sf::Vector2f GetPointedPosition()
{
    sf::Vector2f pos = gCamera.GetViewTopLeft();
    const sf::Input& input = gGame.GetRenderWindow()->GetInput();
    pos.x += input.GetMouseX();
    pos.y += input.GetMouseY();

    pos.x /= (float) Map::TILE_SIZE;
    pos.y /= (float) Map::TILE_SIZE;
//todo: przemnozyc przez przeskalowana rozdzielczosc okna gry

    return pos;
}

void Editor::MousePressed( const sf::Event::MouseButtonEvent &e )
{
    sf::Vector2f pos = ToxicUtils::GetPointedPosition();

    gConsole.Printf( L"pos x=%f, y=%f", pos.x, pos.y );

    if ( e.Button == sf::Mouse::Right )
    {
        CPhysical * player = gPhysicalManager.GetPhysicalById( L"player0" );
        if ( player )
            player->SetPosition( pos );
    }
}

void Editor::KeyPressed( const sf::Event::KeyEvent &e )
{
    if ( gConsole.GetVisible() )
        return;

    bool reload = false;
    bool spawnDoodah = false;
    bool spawnObj = false;

    std::string file;
    std::string code;
    float x = GetPointedPosition().x;
    float y = GetPointedPosition().y;
    std::string z = "background";
    float scale = 1.0f;
    float rot = gRand.Rndf( 0.0, 360.0 );

    if (e.Code == sf::Key::F )
    {        
        scale = gRand.Rndf( 0.9, 1.9 );
        
        file = "doodahs/boulders/rock" + StringUtils::ToString( gRand.Rnd( 1, 8 + 1 ) ) + ".png";
        if (e.Shift) {
            file = "doodahs/boulders/rocka" + StringUtils::ToString( gRand.Rnd( 1, 8 + 1 ) ) + ".png";
            scale = gRand.Rndf( 0.6, 1.11 );
        }
        if (e.Alt) {
            file = "doodahs/boulders/boulder" + StringUtils::ToString( gRand.Rnd( 1, 2 + 1 ) ) + ".png";
        }
        if (e.Control) {
            file = "doodahs/deadtree" + StringUtils::ToString( gRand.Rnd( 1, 3 + 1 ) ) + ".png";
        }
        if (e.Control && e.Shift) {
            file = "doodahs/desert/ruins" + StringUtils::ToString( gRand.Rnd( 1, 3 + 1 ) ) + ".png";
        }
        

        spawnDoodah = true;
    }
    else if (e.Code == sf::Key::T )
    {
        file = "doodahs/tree" + StringUtils::ToString( gRand.Rnd( 1, 2 + 1 ) ) + ".png";
        z = "foreground";
        scale = gRand.Rndf( 1.3, 1.7 );
        if (e.Shift)
        {
            file = "doodahs/tree-strange.png";
            z = "background";
            scale = gRand.Rndf( 1.0, 1.2 );
        }
        if (e.Alt)
        {
            int r = gRand.Rnd( 0, 2 );
            z = "background";
            file = "doodahs/tree_bark_one.png";
            scale = gRand.Rndf( 1.0, 1.25 );
            if ( r == 1 ) file = "doodahs/tree_bark_two.png";
        }
        spawnDoodah = true;
    }
    else if (e.Code == sf::Key::Y )
    {
        int r = gRand.Rnd( 0, 5 );
        file = "doodahs/tree_broken.png";
        scale = gRand.Rndf( 1.0, 1.5 );
        if ( r == 1 ) { file = "doodahs/trunk_large.png"; scale = gRand.Rndf( 0.7, 1.2 ); }
        if ( r == 2 ) { file = "doodahs/trunk_one.png"; }
        if ( r == 3 ) { file = "doodahs/trunk_two.png"; }
        if ( r == 4 ) { file = "doodahs/trunk_three.png"; }
        if (e.Shift)
            file = "doodahs/ancientblocks.png";
        spawnDoodah = true;
    }
    else if (e.Code == sf::Key::G )
    {
        int r = gRand.Rnd( 0, 5 );
        file = "doodahs/bonefire_one.png";
        scale = gRand.Rndf( 1.0, 1.25 );
        if ( r == 1 ) { file = "doodahs/bonefire_two.png"; }
        if ( r == 2 ) { file = "doodahs/bonefire_three.png"; }
        if ( r == 3 ) { file = "doodahs/bonefire_four.png"; }
        if ( r == 4 ) { file = "doodahs/bonefire_five.png"; }
        spawnDoodah = true;
    }
    else if (e.Code == sf::Key::S )
    {
        int r = gRand.Rnd( 0, 2 );
        file = "doodahs/skeleton.png";
        scale = gRand.Rndf( 1.0, 1.25 );
        if (e.Shift)
            file = "doodahs/beast_skull_small.png";
        if (e.Alt)
            file = "doodahs/beast_skeleton.png";
        if (e.Control)
        {
            file = "doodahs/beast_skull_one.png";
            if ( r == 1 ) file = "doodahs/beast_skull_two.png";
        }
        spawnDoodah = true;
    }
    else if (e.Code == sf::Key::A )
    {
        int r = gRand.Rnd( 0, 4 );
        file = "doodahs/ship_crash_wreck.png";
        scale = gRand.Rndf( 1.20, 1.60 );
        if ( r == 1 ) { file = "doodahs/ship_one.png"; }
        if ( r == 2 ) { file = "doodahs/ship_three.png"; }
        if ( r == 3 ) { file = "doodahs/ship_two.png"; }
        if ( e.Shift) file = "doodahs/bush_whole.png";
        spawnDoodah = true;
    }
    else if (e.Code == sf::Key::C )
    {
        int r = gRand.Rnd( 0, 4 );
        file = "doodahs/cote_broken_one.png";
        scale = gRand.Rndf( 1.50, 1.50 );
        if ( r == 1 ) { file = "doodahs/cote_broken_two.png"; }
        if ( r == 2 ) { file = "doodahs/shed_broken_two.png"; }
        if ( r == 3 ) { file = "doodahs/shed_crash_two.png"; }
        if ( e.Shift) file = "doodahs/cote_one.png";
        spawnDoodah = true;
    }
    else if (e.Code == sf::Key::V )
    {
        int r = gRand.Rnd( 0, 5 );
        file = "doodahs/bonefire_one.png";
        scale = gRand.Rndf( 1.0, 1.25 );
        if ( r == 1 ) { file = "doodahs/bonefire_two.png"; }
        if ( r == 2 ) { file = "doodahs/bonefire_three.png"; }
        if ( r == 3 ) { file = "doodahs/bonefire_four.png"; }
        if ( r == 4 ) { file = "doodahs/bonefire_five.png"; }
        spawnDoodah = true;
    }
    else if (e.Code >= sf::Key::Num0 &&
            e.Code <= sf::Key::Num9 )
    {
        code = "monster";
        if (e.Control)
            code = "monster-lair";
        if (e.Alt)
            code = "pacman";

        code += StringUtils::ToString( (int) e.Code - (int) sf::Key::Num0 );

        if (e.Alt && e.Code == sf::Key::Num5)
            code = "barrel";

        spawnObj = true;
    }
    else if (e.Code == sf::Key::P )
    {
        code = "collision";
        spawnObj = true;
        reload = true;
        rot = 0;
    }
    else if (e.Code == sf::Key::R )
    {
        reload = true;
    }
    else if (e.Code == sf::Key::U )
    {
        std::string buffer;
        std::string header = "<!-- -->";
        std::string prevBuf;
        std::ifstream ifs( "data/maps/toxic-workbench-inject.xml" );
        std::ofstream tmp( "data/maps/toxic-workbench-inject-tmp.xml" );

        tmp << header << '\n';

        bool first = true;

        while ( !ifs.eof() )
        {
            getline( ifs, buffer );
            if ( buffer == header ) continue;
            if ( !ifs.eof() && !first )
                tmp << prevBuf << "\n";
            prevBuf = buffer;
            first = false;
        }
        
        tmp.close();
        ifs.close();

        std::ifstream ifstmp( "data/maps/toxic-workbench-inject-tmp.xml" );
        std::ofstream ofs( "data/maps/toxic-workbench-inject.xml" );

        ofs << ifstmp.rdbuf();
        ofs.close();
        ifstmp.close();

        reload = true;
    }

    const sf::Input& input = gGame.GetRenderWindow()->GetInput();
    if ( input.IsKeyDown( sf::Key::Numpad8 ) ) rot = 0.0;
    if ( input.IsKeyDown( sf::Key::Numpad9 ) ) rot = 315.0;
    if ( input.IsKeyDown( sf::Key::Numpad6 ) ) rot = 270.0;
    if ( input.IsKeyDown( sf::Key::Numpad3 ) ) rot = 225.0;
    if ( input.IsKeyDown( sf::Key::Numpad2 ) ) rot = 180.0;
    if ( input.IsKeyDown( sf::Key::Numpad1 ) ) rot = 135.0;
    if ( input.IsKeyDown( sf::Key::Numpad4 ) ) rot = 90.0;
    if ( input.IsKeyDown( sf::Key::Numpad7 ) ) rot = 45.0;

    if ( spawnObj )
    {
        std::stringstream s;
        s << "<obj code=\"" << code << "\"\t x=\"" << x << "\"\t y=\"" << y << "\" rot=\"" << rot << "\" />\n";

        std::ofstream ofs( "data/maps/toxic-workbench-inject.xml", std::ios::app );
        ofs << s.str();
        ofs.close();

//        reload = true;
    }

    if ( spawnDoodah )
    {
        if ( file.empty() )
        {
            gConsole.Printf(L"warning: spawnDoodah: file is empty");
            return;
        }

        std::stringstream s;
        s << "<sprite file=\"data/maps/" << file << "\"\t x=\"" << x << "\"\t y=\"" << y << "\"\t z=\"" << z << 
                "\"\t scale=\"" << scale << "\"\t rot=\"" << rot << "\" />\n" ;

        std::ofstream ofs( "data/maps/toxic-workbench-inject.xml", std::ios::app );
        ofs << s.str();
        ofs.close();

        reload = true;
    }

    if ( reload )
    {
        std::ofstream merged( "data/maps/toxic-workbench-merged.xml" );

        std::ifstream    pre( "data/maps/toxic-workbench-pre.xml" );
        std::ifstream inject( "data/maps/toxic-workbench-inject.xml" );
        std::ifstream   post( "data/maps/toxic-workbench-post.xml" );

        merged << pre.rdbuf();
        merged << inject.rdbuf();
        merged << post.rdbuf();

        merged.close();

        pre.close();
        inject.close();
        post.close();

        gCommands.ParseCommand( L"unload-map" );
        gResourceManager.DropResource("data/maps/toxic-workbench-merged.xml");
        gCommands.ParseCommand( L"load-map data/maps/toxic-workbench-merged.xml" );

    }
   
}

}

#else

bool ToxicUtils::isGameInCrimsonMode = false;

#endif
