#include "CGame.h"
#include "CGameOptions.h"
#include "Logic/CLogic.h"
#include "Tests/CTests.h"
#include "Utils/ToxicUtils.h"
#include "Logic/Effects/EEffectType.h"
#include "Utils/CLocaleIntegrityChecker.h"
#include "ResourceManager/CResourceManager.h"
#include "ResourceManager/CImage.h"
#include "Utils/FileUtils.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>

int main( int argc, char* argv[] ){
	CSingletonCleaner cleaner;

	for (int i = 1; i < argc; i++){
		if (strcmp (argv[i], "-mod") == 0){
			if (i >= argc-1){
				fprintf (stderr, "no mod name specified; loading base game data...\n");
				continue;
			}

			std::string moddir (argv[i+1]);
			if (FileUtils::IsFileExists ((moddir + "/mod-info.xml").c_str()) ){
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
		} else if (!std::string(argv[i]).compare("-crimson")) {
            ToxicUtils::isGameInCrimsonMode = true;
            fprintf(stderr, "running in crimson mode...\n");
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
		}
    }

    gGame.Run();

    return 0;
}

#ifndef PLATFORM_MACOSX
#ifndef PLATFORM_LINUX

/*
uwaga -- to by by³o najlepiej przenieœæ do osobnego pliku (PlatformWin32.cpp ?), przy czym nie chcê wam popsuæ
plików z projektami. Swoj¹ drog¹ PLATFORM_WINDOWS te¿ by by³o ciekawe.
*/

#include <windows.h>

bool AskForFullscreen_Win32(const wchar_t * title, const wchar_t * message, int maxw, int maxh)
{
	wchar_t message2[4096];
	wsprintf(message2, message, maxw, maxh);
	return IDOK == MessageBoxW(0, message2, title, MB_OKCANCEL | MB_ICONQUESTION);
}

#endif
#endif
