#include "CGame.h"
#include "CGameOptions.h"
#include "Logic/CLogic.h"
#include "Tests/CTests.h"
#include "Logic/Effects/EEffectType.h"
#include "Utils/CLocaleIntegrityChecker.h"
#include "ResourceManager/CResourceManager.h"
#include "ResourceManager/CImage.h"
#include "Utils/FileUtils.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>

#ifndef PLATFORM_LINUX
#   include <windows.h>
#endif /* PLATFORM_LINUX */

void ShowMessageBox(const wchar_t * title, const wchar_t * message);

int main( int argc, char* argv[] ){
	CSingletonCleaner cleaner;

	bool attemptHideConsole = true;

	for (int i = 1; i < argc; i++){
		if (strcmp (argv[i], "-mod") == 0){
			if (i >= argc-1){
				fprintf (stderr, "no mod name specified; loading base game data...\n");
				continue;
			}

			std::string moddir (argv[i+1]);
			if (FileUtils::FileExists ((moddir + "/mod-info.xml").c_str()) ){
				CGameOptions::SetModDir (moddir + '/');
				fprintf (stderr, "loading mod %s...\n", argv[i+1]);
			}
			else {
				fprintf (stderr, "mod %s not found or %s/mod-info.xml is missing; loading base game data...\n", argv[i+1], argv[i+1]);
			}
			i++;
		} else if (strcmp(argv[i],"--tests")==0){
			CTests::RunAll();
		} else if (strcmp(argv[i],"--tests-only")==0){
			CTests::RunAll();
			getchar();
			return 0;
        } else if (strcmp(argv[i],"--editor")==0) {
            fprintf(stderr, "running in editor mode...\n");
            gGameOptions;   // MUSI byc zaladowane przed gLogic
            gLogic.SetMenuScreensFlag(MENUSCREEN_SHOW_EDITOR);
        } else if (strcmp(argv[i],"--effect-syntax")==0) {
            fprintf(stderr, "displaying logical effect syntax data...\n");
			EffectTypes::PrintOutSyntax();
			getchar();
			return 0;
        } else if (strcmp(argv[i],"--locale-check")==0) {
            std::vector<std::string> langs;
            langs.push_back("pl");  // wszystkie testy odnosza sie do pierwszego jezyka, nie ma ze kazdy z kazdym
            langs.push_back("en");
            bool areExtras = CLocaleIntegrityChecker().Check(langs);
            return (areExtras ? 1 : 0);
		} else if (strcmp(argv[i],"--resource-check")==0){
			CGame::dontLoadWindowHack = true;
			System::Resource::CImage::dontLoadDataHack = true;
			gResourceManager.ForceLoadAll(false);
			int errors = gResourceManager.GetErrorsOccured();
			fprintf(stderr,"\nSUMMARY: Total %d errors occured\n", errors);
            return (errors > 0 ? 1 : 0);
		} else if (strcmp(argv[i], "--quick-load-map") == 0){
			if (i + 1 >= argc){
				fprintf(stderr,"USAGE: --quick-load-map <map>");
				return 1;
			} else {
				gGame.SetMapToLoadAtInit(new std::string(argv[i+1]));
				i++;
			}
		} else if (strcmp(argv[i], "--show-console") == 0){
			attemptHideConsole = false;
		}
    }

	if (attemptHideConsole){
		std::string userDir = FileUtils::GetUserDir();
		
		if (!FileUtils::FileExists(userDir))
		{
			// i tutaj inicjalizacja danych uzytkownika - kopiowanie 'first_game', 'config.xml' itepe itede
			// mozna tez pomyslec o tym, aby przy uruchamianu WarlocksGauntlet.exe --clean-user-dir 
			// skasowac katalog uzytkownika i na nowo go tworzyc

			FileUtils::CreateDir(userDir);
		}
		if (std::freopen((userDir + "/stdout.log").c_str(), "w", stdout) &&
			std::freopen((userDir + "/stderr.log").c_str(), "w", stderr)) {

	#ifndef PLATFORM_LINUX
			::FreeConsole();
	#endif /* PLATFORM_LINUX */

			gGame.Run();
		} else {
			fprintf(stderr, "Fatal error: couldn't reopen stdout and/or stderr! Aborting...\n");

			// TODO: multi-platform msgbox - wersje pod linuksa i macosa
			std::wstring msg = L"Error: couldn't reopen stdout and/or stderr. Ensure that you have write permissions for directory: ";
			msg += StringUtils::ConvertToWString(userDir);
			msg += L" and files stdout.log and stderr.log inside it.";
			ShowMessageBox(L"Fatal error", msg.c_str());
		}
	} else {
		gGame.Run();
	}

    return 0;
}
