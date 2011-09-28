#include "CXmlSourceDialog.h"
#include "CEditorWnd.h"
#include "CActionsList.h"

#include "Map/CMap.h"
#include "Map/CMapManager.h"

#include "ResourceManager/CResourceManager.h"

#include "Utils/StringUtils.h"

#include "CGameOptions.h"

#include "macros.h"

#include <fstream>
#include <boost/filesystem.hpp>


CXmlSourceDialog::CXmlSourceDialog(wxWindow* parent, wxWindowID id):
    CXmlSourceDlg(parent)
{
}


void CXmlSourceDialog::OnToolbarApplyClicked(wxCommandEvent& event)
{
    // wez sciezke do edytowanej mapy
    std::string mapName = StringUtils::ConvertToString(dynamic_cast<CEditorWnd*>(GetParent())->GetCurrentMap());
    if (mapName == "")
        mapName = PATH_TO_TEMP_MAP;

    // zapisz tam zedytowana wersje
    std::ofstream tmpFile((mapName + ".tmp").c_str(), std::ios::trunc | std::ios::out);
    tmpFile << StringUtils::ConvertToString(mText->GetText().c_str());
    tmpFile.close();

    // dwuetapowy zapis
    if (boost::filesystem::exists(mapName + ".tmp"))
    {
        if (boost::filesystem::exists(mapName))
            boost::filesystem::remove(mapName);
        boost::filesystem::rename(mapName + ".tmp", mapName);
    }
    else
        fprintf(stderr, "ERROR: couldn't save file: %s\n", mapName.c_str());

    // sprobuj ja wczytac
    std::map<std::string, std::string> m;
    m.insert(std::pair<std::string, std::string>("name", mapName));

    // tymczasowa mapa, do testu wczytania
    Map::CMap* map = new Map::CMap();
    if (map->Load(m))
    {
        // bo byly jakies problemy z SetCurrentMapAsVisited
        gMapManager.SetMap("", true);

        // jesli sie da, usun poprzednia mape z pamieci, i wczytaj ta zedytowana
        gResourceManager.DropResource(mapName);
        // wymus wczytanie, zeby SetMap nie chcial jakiegos 0xfeeefeee czytac
        gResourceManager.LoadMap(mapName);

        gMapManager.SetMap(mapName, true);
        this->Hide();
    }
    else
    {
        // jesli nie, zapisz poprzednia wersje (ciagle w pamieci) do pliku
        if (mapName != PATH_TO_TEMP_MAP)
            gResourceManager.GetMap(mapName)->Save(mapName);

        // i wyswietl ladny error
        wxMessageBox(L"Error somewhere in code! :C");
    }

    // zwolnienie tymczasowej mapy (zawiera delete this)
    map->Drop();
    
    // jesli to mapa tymczasowa, to nie musi byc w pliku
    if (mapName == PATH_TO_TEMP_MAP)
        std::remove(mapName.c_str());

    // czyszczenie kolejki undo-redo, bo sie #$%^ po reloadzie
    gActionsList.Clear();
}

void CXmlSourceDialog::OnToolbarCancelClicked(wxCommandEvent& event)
{
    this->Hide();
}
