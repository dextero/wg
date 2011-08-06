#include <cstdio>
#include <cstdarg>

#include "CConsole.h"
#include "../CGame.h"
#include "../CGameOptions.h"
#include "../Commands/CCommands.h"

#include "../Utils/StringUtils.h"
#include "../Rendering/ZIndexVals.h"
#include "../Rendering/CHudSprite.h"
#include "../Rendering/CHudStaticText.h"
#include "../Rendering/CDrawableManager.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/String.hpp>

#include "../GUI/Localization/CLocalizator.h"
#include "../GUI/Localization/GeneralKeys.h"

#include <fstream>

#define CONSOLE_WIDTH 70
#define CONSOLE_HEIGHT 15
#define CONSOLE_FONT_SIZE 18

#ifndef SCM_REVISION
#define SCM_REVISION dev-snapshot
#endif

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)
 
static const std::string revision(STR(SCM_REVISION));
const unsigned int CConsole::msCommandHistoryCount = 10;
static unsigned currentFontSize = 0;

template<> CConsole* CSingleton<CConsole>::msSingleton = 0;

CConsole::CConsole() :
    mVisible( false )
{   
    fprintf( stderr, "CConsole::CConsole()\n" );

    gCommands;
	currentFontSize = unsigned( (float) CONSOLE_FONT_SIZE * std::min( (float) gGameOptions.GetWidth() / 800.0f, (float) gGameOptions.GetHeight() / 600.0f ) );
	mWidth = CONSOLE_WIDTH;
	mHeight = CONSOLE_HEIGHT;
    
    mHudSprite = gDrawableManager.CreateHudSprite( Z_CONSOLE );
    if ( mHudSprite )
    {
        sf::Image* backgroundImage = gResourceManager.GetImage("data/GUI/console.png");
        if ( !backgroundImage )
        {
            fprintf( stderr, "error: CConsole::CConsole(): Unable to load backgroundImage, continuing anyway\n" );
        }
        else
            mHudSprite->GetSFSprite()->SetImage( *backgroundImage );
        mHudSprite->GetSFSprite()->Resize( (float)gGame.GetRenderWindow()->GetWidth(), (float)gGame.GetRenderWindow()->GetHeight() * 0.5f );
    }

    std::map<std::string, std::string> args;
    args["name"] = gLocalizator.GetFont(GUI::FONT_CONSOLE);
    args["charsize"] = StringUtils::ToString(currentFontSize);
    gResourceManager.LoadFont(args);
    sf::Font* font = gResourceManager.GetFont(gLocalizator.GetFont(GUI::FONT_CONSOLE));
    if (!font) {
        fprintf(stderr, "error: CConsole::CConsole(): Unable to load font, continuing anyway\n");
    }

    mHudStaticText = gDrawableManager.CreateHudStaticText(Z_CONSOLE_TEXT);
    if (mHudStaticText) {
        sf::Font* font = gResourceManager.GetFont(gLocalizator.GetFont(GUI::FONT_CONSOLE));
        if (font) {
            mHudStaticText->GetSFString()->SetFont(*font);
		}
        mHudStaticText->GetSFString()->SetSize((float)currentFontSize);
        mHudStaticText->GetSFString()->SetPosition(0, 0);
        mHudStaticText->GetSFString()->SetColor(sf::Color::Black);
    }

    mVersionStaticText = gDrawableManager.CreateHudStaticText(Z_CONSOLE_TEXT);
    if (mVersionStaticText) {
        if (font) {
            mVersionStaticText->GetSFString()->SetFont( *font );
		}
        mVersionStaticText->GetSFString()->SetSize((float)currentFontSize);
        mVersionStaticText->GetSFString()->SetText(StringUtils::ConvertToWString(revision));
        mVersionStaticText->GetSFString()->SetColor(sf::Color(128, 128, 128));
    }

	gGame.AddFrameListener( this );
    gGame.AddKeyListener( this );

	Print( gLocalizator.GetText("CONSOLE_WELCOME"));

    float halfWindowHeight = (float)gGame.GetRenderWindow()->GetHeight() * 0.5f;
    mHudSprite->GetSFSprite()->SetPosition( 0.0, -halfWindowHeight );

    std::string lastCommandsFile = gGameOptions.GetUserDir() + "/last_commands.console";
    FILE *infile = fopen(lastCommandsFile.c_str(), "r");
    if (!infile) {
        fprintf(stderr, "CConsole: unable to load m_LastCommands from file `%s'\n", lastCommandsFile.c_str());
    }
    else
    {
        char buf[4096];
        while (!feof(infile))
        {
          	std::string line(fgets(buf, 4096, infile) != NULL ? buf : "");
            if ( line.empty() )
                continue;
            AddCommandToHistory( StringUtils::ConvertToWString( line ) );
        }
	fclose(infile);
    }
}

CConsole::~CConsole()
{
    fprintf( stderr, "CConsole::~CConsole()\n" );
    Cleanup();
}

void CConsole::Cleanup()
{
    fprintf( stderr, "CConsole::Cleanup()\n" );
    if ( mHudStaticText )
        gDrawableManager.DestroyDrawable( mHudStaticText );
    mHudStaticText = NULL;
    if ( mHudSprite )
        gDrawableManager.DestroyDrawable( mHudSprite );
    mHudSprite = NULL;

    if ( mCommandHistory.size() > 0 )
    {
		std::string lastCommandsFile = gGameOptions.GetUserDir() + "/last_commands.console";
        FILE *of = fopen(lastCommandsFile.c_str(), "w");
        if (!of) {
            fprintf(stderr, "Console: unable to save m_LastCommands to file `%s'", lastCommandsFile.c_str());
        }
        else
        {
            for ( CommandsList::iterator it = mCommandHistory.begin() ; it != mCommandHistory.end() ; it++ )
            {
                fputs((StringUtils::ConvertToString( *it ) + "\n").c_str(), of);
            }
        }
        fclose(of);
    }
}

void CConsole::FrameStarted( float secondsPassed )
{
	unsigned newFontSize = unsigned( (float) CONSOLE_FONT_SIZE * std::min( (float) gGameOptions.GetWidth() / 800.0f, (float) gGameOptions.GetHeight() / 600.0f ) );
    if (newFontSize != currentFontSize) {
        currentFontSize = newFontSize;
        mHudStaticText->GetSFString()->SetSize((float)currentFontSize);
        mVersionStaticText->GetSFString()->SetSize((float)currentFontSize);
    }
    float fontHeight = (float)gGameOptions.GetHeight() / (float)mHeight;
    float fontWidth = (float)gGameOptions.GetWidth() / (float)mWidth;

    float currY = mHudSprite->GetSFSprite()->GetPosition().y;
	float spriteHeight = mHudSprite->GetSFSprite()->GetSize().y;
    if ( !mVisible )
    {
        currY -= secondsPassed * spriteHeight * 3;
        if ( currY < -spriteHeight )
        {
            currY = -spriteHeight;
            gGame.SetConsoleOpened(false);
        }
    }
    else
    {
        currY += secondsPassed * spriteHeight * 3;
        if ( currY > 0 ) {
            currY = 0;
            gGame.SetConsoleOpened(true);
        }
    }

    mHudSprite->GetSFSprite()->SetPosition( 0.0, currY );

    std::wstring text;

    std::list< std::wstring >::iterator it;
    size_t listSize = mLines.size();
    int linesToSkip = (int) (listSize - mHeight);

    it = mLines.begin();
    for ( int i = 0; i < linesToSkip; i++ )
    {
        it++;
    }

    while ( it != mLines.end() )
    {
        text += (*it) + L"\n";
        it++;
    }

    int linesToAdd = mHeight - (int)listSize;
    while ( linesToAdd > 0 )
    {
        text += L"\n";
        linesToAdd--;
    }

    text += L"]";

    if ( mPrompt.length() > mWidth - 2 )
        text += mPrompt.substr( mPrompt.length() - mWidth + 2 );
    else
        text += mPrompt;

    static float timeToNextCursorBlink = 0;
    timeToNextCursorBlink += secondsPassed;
    int truncated = (int)(timeToNextCursorBlink * 2);
    if (truncated % 2)
        text += L"_";

    mHudStaticText->GetSFString()->SetText( text );

	mHudSprite->GetSFSprite()->Resize( (float)gGameOptions.GetWidth(), (float) gGameOptions.GetHeight() * 0.5f );
    mHudStaticText->GetSFString()->SetPosition( mHudSprite->GetSFSprite()->GetPosition() );
    float halfWindowHeight = (float)gGame.GetRenderWindow()->GetHeight() * 0.5f;
    mVersionStaticText->GetSFString()->SetPosition(gGameOptions.GetWidth() - (revision.size() * fontWidth), (currentFontSize * mHeight) + currY);
}

bool CConsole::GetVisible()
{
    return mVisible;
}

void CConsole::SetVisible(bool visible){
    mVisible = visible;
}

void CConsole::KeyPressed( const sf::Event::KeyEvent &e )
{
	switch(e.Code){	
        case sf::Key::F11 :
            mVisible = !mVisible;
            break;
		case sf::Key::F5:
			gCommands.ParseCommand(L"exec F5");
			break;
		case sf::Key::F6:
			gCommands.ParseCommand(L"exec F6");
			break;
		case sf::Key::F7:
			gCommands.ParseCommand(L"exec F7");
			break;
		case sf::Key::F8:
			gCommands.ParseCommand(L"exec F8");
			break;
		default:
			break;
	}

    if ( !mVisible )
        return;

    switch ( e.Code )
    {
        case sf::Key::Back :
        if ( !mPrompt.empty() )
            mPrompt = mPrompt.erase( mPrompt.length() - 1 );
        break;
        case sf::Key::Return :
            Print( L"]" + mPrompt );
            gCommands.ParseCommand( mPrompt );
            
            AddCommandToHistory( mPrompt );
            
            mPrompt.clear();
            break;
        case sf::Key::Up:
            PrintBackHistory();
            break;
        case sf::Key::Down:
            PrintForwardHistory();
            break;
        case sf::Key::Tab:
            ShowAvailableCommands();
            break;
        default:
            std::wstring keyText;            
            if ( StringUtils::ValidChars.find( e.Code ) != std::wstring::npos )
            {
                if (e.Shift)
                    switch (e.Code)
                    {
                    case sf::Key::Num1:     keyText = L"!"; break;
                    case sf::Key::Num2:     keyText = L"@"; break;
                    case sf::Key::Num3:     keyText = L"#"; break;
                    case sf::Key::Num4:     keyText = L"$"; break;
                    case sf::Key::Num5:     keyText = L"%"; break;
                    case sf::Key::Num6:     keyText = L"^"; break;
                    case sf::Key::Num7:     keyText = L"&"; break;
                    case sf::Key::Num8:     keyText = L"*"; break;
                    case sf::Key::Num9:     keyText = L"("; break;
                    case sf::Key::Num0:     keyText = L")"; break;
                    default:                keyText = e.Code - 32;
                    }
                else
                    keyText = e.Code;
            }
            else
            {
                if (e.Shift)
                    switch (e.Code)
                    {
                    case sf::Key::Dash:     keyText = L"_"; break;
                    case sf::Key::Slash:    keyText = L"?"; break;
                    default:                                break;
                    }
                else
                    switch(e.Code)
                    {
                    case sf::Key::LBracket :  keyText = L"[";  break;
                    case sf::Key::RBracket :  keyText = L"]";  break;
                    case sf::Key::SemiColon : keyText = L";";  break;
                    case sf::Key::Comma :     keyText = L",";  break;
                    case sf::Key::Period :    keyText = L".";  break;
                    case sf::Key::Quote :     keyText = L"'";  break;
                    case sf::Key::Slash :     keyText = L"/";  break;
                    case sf::Key::BackSlash : keyText = L"\\"; break;
                    case sf::Key::Tilde :     keyText = L"~";  break;
                    case sf::Key::Equal :     keyText = L"=";  break;
                    case sf::Key::Dash :      keyText = L"-";  break;
                    case sf::Key::Add :       keyText = L"+";  break;
                    case sf::Key::Subtract :  keyText = L"-";  break;
                    case sf::Key::Multiply :  keyText = L"*";  break;
                    case sf::Key::Divide :    keyText = L"/";  break;
                    case sf::Key::Space :     keyText = L" ";  break;
                    default : /*do nothing*/; break;
                    }
    	    }

            if ( StringUtils::ValidChars.find( keyText ) != std::string::npos )
            {
                mPrompt += keyText;
            }
            break;
    }
}

void CConsole::KeyReleased( const sf::Event::KeyEvent &e )
{
    ;
}

void CConsole::Print( const std::wstring& text )
{
    std::wstring currentLine;

    fprintf( stderr, "%ls\n", text.c_str() );

    size_t start = 0;
    size_t index = text.find( L"\n" );
    while ( index != std::wstring::npos )
    {
        while ( index - start > mWidth )
        {
            currentLine = text.substr( start, mWidth );
            mLines.push_back( currentLine );
            start += mWidth;
        }
        currentLine = text.substr( start, index - start );
        mLines.push_back( currentLine );

        start = index + 1;
        index = text.find( L"\n", start );
    }
    index = text.length();
    while ( index - start > mWidth )
    {
        currentLine = text.substr( start, mWidth );
        mLines.push_back( currentLine );
        start += mWidth;
    }
    currentLine = text.substr( start );
    mLines.push_back( currentLine );
}

void CConsole::Printf( const wchar_t* text, ... )
{
    static wchar_t s_Buffer[32768];
    if( wcslen(text)!=0 )
    {
        va_list args;
        va_start(args, text);
        vswprintf( s_Buffer, 32767, text, args );
        va_end(args);
        Print( s_Buffer );
    }
}

unsigned int CConsole::GetWidth(){
    return mWidth;    
}

unsigned int CConsole::GetHeight(){
    return mHeight;
}

void CConsole::CleanScreen(){
    mLines.clear();
}

void CConsole::AddCommandToHistory( const std::wstring &command ){

    if (!mCommandHistory.empty() && mCommandHistory.back() == command)
        return;

    if(mCommandHistory.size() >= msCommandHistoryCount)
        mCommandHistory.pop_front();

    mCommandHistory.push_back( command );
    mCurrentHistoryCommand = --mCommandHistory.end();
}

void CConsole::PrintBackHistory(){
    if(mCommandHistory.size() == 0)
        return;

    mPrompt = (*mCurrentHistoryCommand);
    if(mCurrentHistoryCommand != mCommandHistory.begin())
        mCurrentHistoryCommand--;
}

void CConsole::PrintForwardHistory(){
    if(mCommandHistory.size() == 0)
        return;
    if(mCurrentHistoryCommand != --mCommandHistory.end())
        mCurrentHistoryCommand++;

    mPrompt = (*mCurrentHistoryCommand);
}


void CConsole::ShowAvailableCommands()
{
    // ciach
    mPrompt = StringUtils::TrimWhiteSpacesW(mPrompt);
    if (mPrompt.size() == 0)
        return;

    // sprawdz, czy ktos nie szuka pomocy; jesli tak - omin polecenie i zajmij sie tematem
    std::wstring man;
    if (mPrompt.size() > 3 && mPrompt.substr(0, 4) == L"man ")
    {
        man = mPrompt.substr(0, 4);
        mPrompt = mPrompt.substr(4);
    }
    else if (mPrompt.size() > 4 && mPrompt.substr(0, 5) == L"help ")
    {
        man = mPrompt.substr(0, 5);
        mPrompt = mPrompt.substr(5);
    }

    std::wstring available, match, cmd;
    int num = 0;

    for (std::vector<CCommands::SCommandPair>::const_iterator i = gCommands.GetCommands().begin(); i != gCommands.GetCommands().end(); ++i)
        if ((cmd = i->command).size() >= mPrompt.size() && cmd.substr(0, mPrompt.size()) == mPrompt)
        {
            // znajdz powtarzajacy sie poczatek polecenia
            if (!num)
                match = i->command;
            for (size_t j = 0; j < (match.size() > cmd.size() ? cmd.size() : match.size()); ++j)
                if (match[j] != cmd[j])
                {
                    match = match.substr(0, j);
                    break;
                }

            // dopisz polecenie do listy
            available += std::wstring(i->command) + L" ";
            // ile polecen znalazlo?
            ++num;
        }

    // jesli nic nie pasuje, uciekaj
    if (!num)
    {
        if (man.size())
            mPrompt = man + mPrompt;
        return;
    }

    // jesli ktos szuka pomocy, dopisz to, co bylo na poczatku
    if (man.size())
        mPrompt = man;
    if (!man.size())
        mPrompt = L"";

    // jesli pasuje tylko jedno polecenie, to dawaj w prompta
    if (num == 1)
        mPrompt += available;
    else
    {
        // jesli nie - wpisz do prompta to, co sie powtarza i wypisz liste
        mPrompt += match;
        Print(available);
    }
}