#ifndef __MACROS_H__
#define __MACROS_H__

// pomocnicze makra

#define TREE_ICON_SIZE  24, 24

#define GAME_FOLDER     ""
#define EDITOR_FOLDER   "editor/"
#define EDITOR_HELP_FOLDER  "editor/help/"

// std::string
#define PATH_TO_GAME    std::string(GAME_FOLDER)
// std::wstring
#define PATH_TO_GAMEW   std::wstring(_T(GAME_FOLDER))
#define PATH_TO_EDITORW std::wstring(_T(EDITOR_FOLDER))

#define PATH_TO_EDITOR_HELP     std::string(EDITOR_HELP_FOLDER)
#define PATH_TO_EDITOR_HELPW    std::wstring(std::wstring(_T(GAME_FOLDER)) + _T(EDITOR_HELP_FOLDER))

// sciezka WZGLEDNA do pliku tymczasowej mapy
#define PATH_TO_TEMP_MAP        (gGameOptions.GetUserDir() + std::string("/editor_temp_map"))


#endif // __MACROS_H__