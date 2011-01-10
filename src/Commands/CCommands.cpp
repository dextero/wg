#include <cstdio>

#include "CCommands.h"
#include "../Console/CConsole.h"

#include "MiscCommands.h"
#include "MapCommands.h"

#include "../Utils/StringUtils.h"
#include "../CGame.h"

#include "../GUI/Localization/CLocalizator.h"

#include <string>
#include <fstream>

// w jakiegos namespace'a to wrzucic? hm hm hm...

template<> CCommands* CSingleton<CCommands>::msSingleton = 0;

class CCommands::CCommandBlock{
public:
    CCommandBlock(): delay(0.0f),current(0){}

    void Perform(float dt){
        delay -= dt;
        if (delay < 0.0f) delay = 0.0f;
        if (delay == 0.0f){
            while (current < commands.size()){
                gCommands.ParseCommandFromBlock(commands[current],this,environment);
                current++;
                if (delay > 0.0f) // zostalo wykonane delay
                    break;
            }
        }
    }

    void Delay(size_t argc, const std::vector<std::wstring> &argv){
        if (argc != 2){
            gConsole.Printf(L"usage: delay time; usable only from a block");
            return;
        }

        float time = 0.0f;
        if (swscanf( argv[1].c_str(), L"%f", &time ) != 1){
            gConsole.Printf(L"usage: delay time; unable to parse time value");
            return;
        }
        delay += time;
    }

    bool Finished(){
        return (current == commands.size());
    }

    std::vector<std::wstring> commands;
    float delay;
    unsigned int current;
    std::map<std::wstring, std::wstring> environment;
};

CCommands::CCommands():mWorkBlock(NULL),mBlockDepth(0){
	fprintf(stderr,"CCommands::CCommands()\n");
	
	RegisterCommand(L"help",    "$MAN_HELP",    CCommands::CommandHelp);
    RegisterCommand(L"man",     "$MAN_MAN",     CCommands::CommandHelp);
    RegisterCommand(L"delay",   "$MAN_DELAY",   CCommands::CommandDelay);
	
	RegisterCommands(MiscCommands);
	RegisterCommands(MapCommands);

    gGame.AddFrameListener( this );
}

CCommands::~CCommands(){
	fprintf(stderr,"CCommands::~CCommands()\n");
}

void CCommands::RegisterCommand(const wchar_t *command, const char *manual, CommandFunctionPtr func){
	SCommandPair p;
	p.command = command;
    p.manualPage = manual;
	p.func = func;
	
	mCommands.push_back(p);
}

void CCommands::RegisterCommands(SCommandPair commands[]){
	for (SCommandPair *it = commands; it->command != 0; ++it)
		mCommands.push_back(*it);
}

void CCommands::ParseCommand( const std::wstring &input){
	std::map<std::wstring, std::wstring> emptyParams;
    ParseCommandFromBlock(input,NULL,emptyParams);
}

void CCommands::ParseCommand(const std::wstring &input, std::map<std::wstring, std::wstring> &environment){
	ParseCommandFromBlock(input,NULL,environment);
}

void CCommands::ParseCommandFromBlock( const std::wstring &input1, CCommands::CCommandBlock * block,
		std::map<std::wstring, std::wstring> &environment)
{
    std::wstring output;
    std::wstring input = StringUtils::TrimWhiteSpacesW( input1 );

    std::vector<std::wstring> params;
    //tokenizujemy wejscie (na piechote):
    bool isInWord = false;
    bool isInQuote = false;
    size_t lastPos = 0;
    size_t size = input.size();
    for ( size_t i = 0; i < size; i++ )
    {
        if ( ( isInWord && isInQuote && input[i] == L'"' ) ||
            ( isInWord && !isInQuote && input[i] == L' ' ) )
        {
            output = input.substr( lastPos, i - lastPos );
            params.push_back( output );

            isInWord = false;
            isInQuote = false;
        }
        else if ( !isInWord && input[i] != L' ' )
        {
            lastPos = i;
            isInWord = true;
            if ( input[i] == L'"' )
            {
                isInQuote = true;
                lastPos++;
            }
        }
    }
    if ( isInWord )
    {
        output = input.substr( lastPos, input.size() - lastPos );
        params.push_back( output );
    }

    if ((params.size() == 1) && (params[0] == L"[")){ // poczatek bloku
        if (mWorkBlock == NULL)
            mWorkBlock = new CCommandBlock();
        mBlockDepth++;
    } else if ((params.size() == 1) && (params[0] == L"]")){ // koniec bloku
        if (mBlockDepth <= 1){
            if (mWorkBlock != NULL) {
            	mWorkBlock->environment = environment;
                mCommandBlocks.push_back(mWorkBlock);
                mWorkBlock = NULL;
            } else {
                gConsole.Printf(L"ERROR: found unmatched ']'");
            }
        }
        mBlockDepth--;
    } else if (mWorkBlock != NULL) { // wewnatrz bloku
        mWorkBlock->commands.push_back(input);
    } else if ((block != NULL) && (mBlockDepth == 0)) { // wykonanie instrukcji bloku
        if ((params.size() > 0) && (params[0] == L"delay"))
            block->Delay(params.size(), params);
        else {
            if (environment.size() != 0){
            	InsertEnvironmentVariables(params, environment);
            }
            CCommands::DoCommand( params.size(), params);
        }
    } else { // normalne wykonanie
        if (environment.size() != 0){
        	InsertEnvironmentVariables(params, environment);
        }
        CCommands::DoCommand( params.size(), params );
    }
}

void CCommands::InsertEnvironmentVariables(std::vector<std::wstring> &params, std::map<std::wstring,std::wstring> &environment){
	for(unsigned int i = 0; i < params.size(); i++){
		if (environment.count(params[i]) != 0){
			params[i] = environment[params[i]];
		}
	}
}

void CCommands::CommandHelp(size_t argc, const std::vector<std::wstring> &argv)
{
    if (argc == 1)
    {
        gConsole.Printf(L"Printing commands:");
        std::wstring buffer(L""), tmp(L"");
        for( std::vector<SCommandPair>::iterator it = gCommands.mCommands.begin() ; it != gCommands.mCommands.end(); ++it ){
            tmp+=it->command;
            tmp+=L" ";
            if (tmp.length() > gConsole.GetWidth()){
                gConsole.Printf(L"%ls",buffer.c_str());
                buffer=it->command;
                buffer+=L" ";
                tmp=it->command;
                tmp+=L" ";
            } else{
                buffer+=it->command;
                buffer+=L" ";
            }
        }
        if (buffer.length() > 0)
            gConsole.Printf(L"%ls",buffer.c_str());
    }
    else
    {
        std::vector<std::wstring> args;
        args.push_back(argv[1]);
        args.push_back(L"?");
        gCommands.DoCommand(2, args);
    }
}

void  CCommands::CommandDelay( size_t argc, const std::vector<std::wstring> &argv ){
    // to tylko stub, zeby ktos go nie wpisal poza blokiem; 
    // prawdziwa obsluga jest w CCommandBlock::Delay()
    gConsole.Printf(L"usage: delay time; usable only from a block");
}

void CCommands::DoCommand( size_t argc, const std::vector<std::wstring> &argv )
{
    if ( 0 == argc )
        return;

    for( std::vector<SCommandPair>::iterator it = mCommands.begin(); it != mCommands.end() ; ++it )
    {
        if( argv[0] == it->command )
        {
            if (argc > 1 && (argv[1] == L"?" || argv[1] == L"--help"))
            {
                std::wstring man = gLocalizator.Localize(it->manualPage);
                gConsole.Printf(L"%ls", (man == L"???" ? L"This command has no mercy nor help page." : man.c_str()));
            }
            else
                it->func( argc, argv );
            return;
        }
    }

	if (argv[0].at(0) != '#')
		gConsole.Printf( L"Unrecognized command: %ls", argv[0].c_str() );
}

void CCommands::FrameStarted(float dt){
    for (unsigned int i = 0; i < mCommandBlocks.size(); i++){
        mCommandBlocks[i]->Perform(dt);
        if (mCommandBlocks[i]->Finished()){
            mCommandBlocks.erase(mCommandBlocks.begin() + i);
            i--;
        }
    }
}

