#ifndef __COMMANDS_H__
#define __COMMANDS_H__

/* Klasa implementujaca parsowanie komend i wywolywanie odpowiednich funkcji.
 */

#include <string>
#include <vector>
#include <map>

#include "../Utils/CSingleton.h"
#include "../IFrameListener.h"

#define gCommands CCommands::GetSingleton()

class CCommands : public CSingleton<CCommands>, public IFrameListener {
public:
	CCommands();
	virtual ~CCommands();
	
	typedef void (*CommandFunctionPtr)(size_t argc, const std::vector<std::wstring> &argv);
	struct SCommandPair
	{
	    const wchar_t* command;
        const char* manualPage;
	    CommandFunctionPtr func;
	};
	
	void RegisterCommand(const wchar_t *command, const char *manual, CommandFunctionPtr func);
	void RegisterCommands(SCommandPair commands[]);
	
	void ParseCommand(const std::wstring &input);
	void ParseCommand(const std::wstring &input, std::map<std::wstring, std::wstring> &environment);
	
	void DoCommand(size_t argc, const std::vector< std::wstring> &argv);

    const std::vector<SCommandPair>& GetCommands() const { return mCommands; };

    void FrameStarted( float secondsPassed );
    
    virtual bool FramesDuringPause(EPauseVariant pv) { return true; };
private:	
	// PIMPL it!

	std::vector< SCommandPair > mCommands;
	
	void InsertEnvironmentVariables(std::vector<std::wstring> &params, std::map<std::wstring,std::wstring> &environment);

	static void CommandHelp(size_t argc, const std::vector<std::wstring> &argv);
    static void CommandDelay(size_t argc, const std::vector<std::wstring> &argv);
    
    class CCommandBlock; // definicja w .cpp

    void ParseCommandFromBlock(const std::wstring &input,CCommandBlock *block,
    		std::map<std::wstring, std::wstring> &environment);

    std::vector<CCommandBlock *> mCommandBlocks;
    CCommandBlock *mWorkBlock; // blok aktualnie konstruowany
    int mBlockDepth; // zagniezdzenie blokow
};

#endif//__COMMANDS_H__
