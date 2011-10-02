#include "CEditorWnd.h"
#include "Utils/StringUtils.h"
#include <boost/filesystem/operations.hpp>

#include "Editor/CEditor.h"
#include "Editor/CTileResource.h"
#include "Editor/CDoodahResource.h"
#include "Editor/CPhysicalResource.h"
#include "Editor/CRegionResource.h"

#include "Logic/MapObjects/CRegion.h"
#include "Logic/Factory/CPhysicalTemplate.h"
#include "Logic/CPhysicalManager.h"
#include "Logic/CPhysical.h"

#include "Map/SceneManager/CQuadTreeSceneManager.h"
#include "Map/CMapManager.h"
#include "Map/CMap.h"
#include "Map/CTile.h"
#include "Map/CDoodahDescriptor.h"
#include "Map/CMapObjectDescriptor.h"

#include "ResourceManager/CResourceManager.h"
#include "ResourceManager/CImage.h"

#include "Rendering/Animations/CAnimationManager.h"
#include "Rendering/Animations/SAnimation.h"
#include "Rendering/ZIndexVals.h"

#include "CGameOptions.h"

#include <iostream>

// tak, zdaje sobie sprawe, co wciskam do funkcji, siedz cicho
#pragma warning(disable:4996)

// pomocnicze makra
#include "macros.h"

#ifdef PLATFORM_LINUX
#   include "wx/imaglist.h"
#endif

// ------------------------------------------------------------------------------

CEditorWnd::CEditorWnd( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) :
    CMainWindow( parent, id, title, pos, size, style ),
    mXmlSourceDlg(NULL),
    mHelpDlg(NULL),
    mMapSizeDlg(NULL),
    mRandomBrushPropertiesDlg(NULL),
    mRandomBrushMinScale(0.7f),
    mRandomBrushMaxScale(1.1f),
    mCurrentMap(L""),
    mCurrActionType(Actions::none),
    mGameProcessId(0)
{
    wxImageList* imgList = new wxImageList(TREE_ICON_SIZE);
    mTreeTiles->AssignImageList(imgList);
    mTreeDoodahs->SetImageList(imgList);
    mTreePhysicals->SetImageList(imgList);
    mTreeOther->SetImageList(imgList);
    
    // bo nie chce mi sie szukac, jak dodac tylko png
    wxInitAllImageHandlers();

    std::string defaultMod = PATH_TO_GAME + "data";
    if (!LoadMod(defaultMod))
        fprintf(stderr, "ERROR: Couldn't load default editor data (%s)!\n", defaultMod.c_str());

    // anty-perspektywa
    mSfml->UpdateGameWindow();
}

CEditorWnd::~CEditorWnd()
{
}

// ----------------------------------------------------------------------------------------------

// osobna funkcja, zeby katalogi wrzucic na wierzch
void CEditorWnd::LoadFromDirectory(std::vector<std::string>& directories, std::vector<std::string>& files)
{
	while (directories.size() > 0)
    {
		std::string dir = directories[directories.size()-1];
		directories.pop_back();
		boost::filesystem::directory_iterator di(dir),dir_end;

        // zeby katalogi byly na wierzchu, trzeba przejsc 2x
		// katalogi
        for (; di != dir_end; di++)
        {
			if (di->leaf() == ".svn")
				continue;
			std::string file = dir + "/" + di->leaf();
			if (boost::filesystem::is_directory(file))
            {
				directories.push_back(file);
                LoadFromDirectory(directories, files);
            }
		}
        // pliki
        di = boost::filesystem::directory_iterator(dir);
        for (; di != dir_end; di++)
        {
			if (di->leaf() == ".svn")
				continue;
			std::string file = dir + "/" + di->leaf();
			if (!boost::filesystem::is_directory(file))
				files.push_back(file);
		}
	}
}

// ladowanie zawartosci katalogu do drzewek
bool CEditorWnd::LoadMod(const std::string& modDir)
{
    // czyszczenie listy obrazkow, nie beda potrzebne
    wxImageList* imgList = mTreeTiles->GetImageList();
    if (!imgList)
    {
        fprintf(stderr, "ERROR: no image list assigned to tilesTree! loading mod aborted.\n");
        return false;
    }
    imgList->RemoveAll();

    // dodawanie elementow do drzewek; wczytywanie listy z folderow
    std::vector<std::string> foundFiles, foundDirectories;
	foundDirectories.push_back(modDir);
    LoadFromDirectory(foundDirectories, foundFiles);

	fprintf(stderr,"\n\nFound %d files!",(int)(foundFiles.size()));

    std::vector<std::wstring> tiles, doodahs, physicals;

    // ile znakow z poczatku sciezki usunac?
    size_t charsToRemove = modDir.size() + 1;

	for (size_t i = 0; i < foundFiles.size(); i++)
    {
		std::string f = foundFiles[i];
		if (f.find("/old/") == std::string::npos)
        {
            if (f.rfind(".xml")!=std::string::npos){
                if (f.find("/maps/")==std::string::npos)
                {
                    if ((f.find("/physicals/") != std::string::npos) ||
                        (f.find("/items/") != std::string::npos))
                        physicals.push_back(StringUtils::ConvertToWString(f.substr(charsToRemove)));
                }
            }
            else if (f.rfind(".png")!=std::string::npos)
            {
                if (f.find("/doodahs/") != std::string::npos)
                    doodahs.push_back(StringUtils::ConvertToWString(f.substr(charsToRemove)));
                else if (f.find("/tiles/") != std::string::npos)
                    tiles.push_back(StringUtils::ConvertToWString(f.substr(charsToRemove)));
                    
            }
		}
    }

    // domyslna ikona
    wxImage folder(PATH_TO_EDITORW + L"img/open.png");
    int folderImg = imgList->Add(folder.Scale(24, 24));
    
    // kafle
    mTreeTiles->DeleteAllItems();
    InitTreeTiles(tiles, folderImg);
    // doodahy
    mTreeDoodahs->DeleteAllItems();
    InitTreeDoodahs(doodahs, folderImg);
    // physicale
    mTreePhysicals->DeleteAllItems();
    InitTreePhysicals(physicals, folderImg);
    // reszta
    mTreeOther->DeleteAllItems();
    InitTreeOther(folderImg);

    // rozwijanie drzewek
    wxTreeCtrl* trees[] = { mTreeTiles, mTreeDoodahs, mTreePhysicals, mTreeOther };
    for (unsigned int i = 0; i < 4; ++i)
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId item = trees[i]->GetRootItem();
        while (item.IsOk())
        {
            // dopoki nie ma wiecej niz 1 dziecka
            wxTreeItemId tmpItem = item;
            while (trees[i]->GetChildrenCount(tmpItem, false) == 1) {
                tmpItem = trees[i]->GetFirstChild(tmpItem, cookie);
                if (tmpItem.IsOk())
                    trees[i]->Expand(tmpItem);
                else
                    break;
            }

            item = trees[i]->GetNextChild(item, cookie);
        }
    }

    // jeszcze ustawienie w grze
    // gGameOptions.SetModDir(modDir);

    return true;
}

// ----------------------------------------------------------------------------------------------

// akcje undo-redo
void CEditorWnd::HandleAction(SAction action)
{
    if (gMapManager.GetCurrent() == NULL)
        return;

    switch (action.type)
    {
    case Actions::changeTile:
        gMapManager.GetCurrent()->SetTile((int)action.x, (int)action.y, action.code);
        break;
    case Actions::deleteDoodah:
        // doodah BYL skasowany, trzeba przywrocic
        gMapManager.GetCurrent()->AddDoodah(action.code, action.x, action.y, action.z, action.rot, action.scale);
        break;
    case Actions::placeDoodah:
        // doodah BYL postawiony, trzeba skasowac
        {
            CSceneNode* node = gScene.GetDisplayableAt(sf::Vector2f(action.x, action.y));
            if (node != NULL)
            {
                gMapManager.GetCurrent()->EraseDoodah(node->GetDoodahDescriptorId());
                gScene.RemoveSceneNode(node, true);
            }
        }
        break;
    case Actions::deletePhysical:
        // physical BYL skasowany, trzeba przywrocic - name == codeNew..
        gMapManager.GetCurrent()->AddMapObject(action.code, action.x, action.y, action.rot, true, action.codeNew);
        break;
    case Actions::placePhysical:
        // physical BYL postawiony, trzeba skasowac
        {
            CSceneNode* node = gScene.GetPhysicalAt(sf::Vector2f(action.x, action.y));
            if (node != NULL)
            {
                gMapManager.GetCurrent()->EraseMapObject(node->GetMapObjectDescriptorId());
                gScene.RemoveSceneNode(node, true);
            }
            else
            {
                fprintf(stderr, "Undo ERROR: couldnt get scene node at: %f %f\n", action.x, action.y);
                node = gScene.GetPhysicalAt(sf::Vector2f(action.x, action.y));
            }
        }
        break;
    case Actions::deleteRegion:
        // region BYL skasowany, trzeba przywrocic - code == uniqueId physicala, codeNew == zserializowany region (?)
        {
            CRegionResource* rgn = dynamic_cast<CRegionResource*>(gEditor.mRegion);
            if (rgn == NULL)
                wxMessageBox(L"We've got some problem here :( gEditor.mRegion doesn't exist! (CEditorWnd::HandleAction(deleteRegion))");
            else
            {
                Map::CRegionDescriptor* desc = new Map::CRegionDescriptor(action.code, sf::Vector2f(action.x, action.y), action.scale);

                // kopiujemy zserializowany region do bufora char*, bo rapidxml
                // chce moc sobie go modyfikowac - nie moze byc const
                char* buf = new char[action.codeNew.size() + 1];
                buf[action.codeNew.size()] = '\0';
                action.codeNew.copy(buf, action.codeNew.size());

                // pusty obiekt CXml, wszystkie xml.Get* maja dzialac
                // przyjmujemy, ze codeNew jest poprawne (zawiera <region [..]>)
                CXml xml("", "");
                rapidxml::xml_document<> doc;
                doc.parse<0>(buf);

                // niech CRegionDescriptor sobie wszystko zaladuje..
                desc->Load(xml, doc.first_node("region"));

                // teraz mozna skasowac bufor, bo niepotrzebny
                delete[] buf;

                // teraz tylko popodmieniac uniqueId, zeby bylo "stare"..
                if (action.code.size() > 6 && action.code.substr(0, 6) == "region")
                    desc->uniqueId = StringUtils::Parse<int>(action.code.substr(6));

                // i mozna dodawac:
                gMapManager.GetCurrent()->AddRegion(desc, true);

                // ostatni physical == nasz region
                CPhysical* p = (*(--gPhysicalManager.GetPhysicals().end()));
                if (p->GetUniqueId() != L"")
                {
                    // id deskryptora w physicalu
                    CDynamicRegion* r = dynamic_cast<CDynamicRegion*>(p);
                    r->SetDescriptorId(desc->uniqueId);
                }
            }
        }
        break;
    case Actions::placeRegion:
        // region BYL postawiony, trzeba skasowac
        {
            CSceneNode* node = gScene.GetPhysicalAt(sf::Vector2f(action.x, action.y));
            if (node != NULL)
            {
                CPhysical* phys = node->GetPhysical();
                if (CDynamicRegion* rgn = dynamic_cast<CDynamicRegion*>(phys))
                {
                    rgn->MarkForDelete();
                    gMapManager.GetCurrent()->EraseRegion(rgn->GetDescriptorId());
                }
            }
        }
        break;
    default:
        break;
    }
}

void CEditorWnd::PrepareRandomBrush()
{
    // pedzel losujacy, tylko dla doodahow i physicali
    wxTreeCtrl* selectedTree = NULL;
    if (mTreeTabs->GetSelection() == 1)         selectedTree = mTreeDoodahs;
    else if (mTreeTabs->GetSelection() == 2)    selectedTree = mTreePhysicals;

    if (selectedTree)
    {
        // zaznaczone doodahy albo physicale, teraz pytanie: ile zaznaczen?
        wxArrayTreeItemIds itemsArray;
        selectedTree->GetSelections(itemsArray);

        // pozbywamy sie katalogow
        for (size_t item = 0; item < itemsArray.GetCount(); ++item)
            if (selectedTree->GetChildrenCount(itemsArray[item], false) > 0)
            {
                itemsArray.RemoveAt(item);
                --item;
            }

        if (itemsArray.GetCount() > 1)
        {
            // mamy wiecej niz 1 zaznaczenie, losujemy!
            wxTreeItemId selectedItem = itemsArray[rand() % itemsArray.GetCount()];
                
            // wyciagamy sciezke do wylosowanego elementu
            mSelectedItemStr = GetPathToItem(selectedTree, selectedItem);

            gEditor.SetSelected(StringUtils::ConvertToString(mSelectedItemStr));
            gEditor.mRotation = (float)(rand() % 360);

            // tekst wyswietlany na pasku statusu
            std::wstring message = L"Random brush: selected = " + mSelectedItemStr
                + L"; rotation = " + StringUtils::ToWString(gEditor.mRotation);

            // skala nie liczy sie w physicalach
            if (selectedTree != mTreePhysicals)
            {
                // jesli min == max, to division by zero :o
                if (mRandomBrushMinScale < mRandomBrushMaxScale)
                    gEditor.mScale = (float)(rand() % (int)((mRandomBrushMaxScale - mRandomBrushMinScale) * 100.f)) / 100.f + mRandomBrushMinScale;
                else
                    gEditor.mScale = mRandomBrushMinScale;
                message += L"; scale = " + StringUtils::ToWString(gEditor.mScale);
            }

            mStatusBar->SetStatusText(message);
        }
    }
}


void CEditorWnd::FocusCurrentTree()
{
    // focus na wybranym drzewku, zeby mozna bylo scrollowac bez klikania
    if (mTreeTabs)
    {
        switch (mTreeTabs->GetSelection())
        {
        case 0:
            mTreeTiles->SetFocus();
            break;
        case 1:
            mTreeDoodahs->SetFocus();
            break;
        case 2:
            mTreePhysicals->SetFocus();
            break;
        case 3:
            mTreeOther->SetFocus();
            break;
        default:
            break;
        }
    }
}

const wxString CEditorWnd::GetPathToItem(wxTreeCtrl* tree, wxTreeItemId item)
{
    // zwraca sciezke do itema w formacie root/parent/child/.../item
    wxString path = tree->GetItemText(item);
    for (wxTreeItemId id = tree->GetItemParent(item); id.IsOk(); id = tree->GetItemParent(id))
        path = tree->GetItemText(id) + L"/" + path;

    return path;
}

// --------------------------

void CEditorWnd::InitTreeTiles(std::vector<std::wstring>& tiles, int defaultImg)
{
    std::wstring name, rest;
    size_t slashAt = std::string::npos;
    wxTreeItemId parentNode;

    wxTreeItemId root = mTreeTiles->AddRoot(L"data");
    mTreeTiles->SetItemImage(root, defaultImg);

    fprintf(stderr, "\n\nLoading treeview thumbs:\n");
    for (std::vector<std::wstring>::iterator it = tiles.begin(); it != tiles.end(); ++it)
    {
        parentNode = root;

        rest = *it;

        while ((slashAt = rest.find(L"/")) != std::wstring::npos)
        {
            name = rest.substr(0, slashAt);

            wxTreeItemIdValue wtfIsThis;
            wxTreeItemId node = mTreeTiles->GetFirstChild(parentNode, wtfIsThis);
            for (; !node.IsOk() || (mTreeTiles->GetItemText(node) != name); node = mTreeTiles->GetNextSibling(node))
            {
                if (!node.IsOk())
                {
                    mTreeTiles->SetItemImage(node = mTreeTiles->AppendItem(parentNode, name), defaultImg);

                    break;
                }
            }

            parentNode = node;

            rest = rest.substr(slashAt + 1);
        }

        wxImage img(PATH_TO_GAMEW + L"data/" + *it);
        mTreeTiles->SetItemImage(mTreeTiles->AppendItem(parentNode, rest), mTreeTiles->GetImageList()->Add(img.Scale(TREE_ICON_SIZE)));

        fprintf(stderr, "Loading tiles.. (%u/%u)\r", (unsigned int)(it - tiles.begin() + 1), tiles.size());
    }
    mTreeTiles->Expand(root);
}

void CEditorWnd::InitTreeDoodahs(std::vector<std::wstring>& doodahs, int defaultImg)
{
    std::wstring name, rest;
    size_t slashAt = std::string::npos;
    wxTreeItemId parentNode;

    wxTreeItemId root = mTreeDoodahs->AddRoot(L"data");
    mTreeDoodahs->SetItemImage(root, defaultImg);

    fprintf(stderr, "\n");
    for (std::vector<std::wstring>::iterator it = doodahs.begin(); it != doodahs.end(); ++it)
    {
        parentNode = root;

        rest = *it;

        while ((slashAt = rest.find(L"/")) != std::wstring::npos)
        {
            name = rest.substr(0, slashAt);

            wxTreeItemIdValue wtfIsThis;
            wxTreeItemId node = mTreeDoodahs->GetFirstChild(parentNode, wtfIsThis);
            for (; !node.IsOk() || (mTreeDoodahs->GetItemText(node) != name); node = mTreeDoodahs->GetNextSibling(node))
            {
                if (!node.IsOk())
                {
                    mTreeDoodahs->SetItemImage(node = mTreeDoodahs->AppendItem(parentNode, name), defaultImg);

                    break;
                }
            }

            parentNode = node;

            rest = rest.substr(slashAt + 1);
        }

        wxImage img(PATH_TO_GAMEW + L"data/" + *it);
        mTreeDoodahs->SetItemImage(mTreeDoodahs->AppendItem(parentNode, rest), mTreeDoodahs->GetImageList()->Add(img.Scale(TREE_ICON_SIZE)));

        fprintf(stderr, "Loading doodahs.. (%u/%u)\r", (unsigned int)(it - doodahs.begin() + 1), doodahs.size());
    }
    mTreeDoodahs->Expand(root);
}

void CEditorWnd::InitTreePhysicals(std::vector<std::wstring>& physicals, int defaultImg)
{
    std::wstring name, rest;
    size_t slashAt = std::wstring::npos;
    wxTreeItemId parentNode;

    wxTreeItemId root = mTreePhysicals->AddRoot(L"data");
    mTreePhysicals->SetItemImage(root, defaultImg);

    fprintf(stderr, "\n");
    for (std::vector<std::wstring>::iterator it = physicals.begin(); it != physicals.end(); ++it)
    {
        parentNode = root;

        rest = *it;

        CXml xml(PATH_TO_GAME + "data/" + StringUtils::ConvertToString(*it), "root");
        std::string type = xml.GetString(xml.GetRootNode(), "type");
        if ((type != "item") &&
            (type != "enemy") &&
            (type != "lair") &&
            (type != "door") &&
            (type != "hook") &&
            (type != "obstacle") &&
            (type != "wall"))
            continue;

        while ((slashAt = rest.find(L"/")) != std::string::npos)
        {
            name = rest.substr(0, slashAt);

            wxTreeItemIdValue wtfIsThis;
            wxTreeItemId node = mTreePhysicals->GetFirstChild(parentNode, wtfIsThis);
            for (; !node.IsOk() || (mTreePhysicals->GetItemText(node) != name); node = mTreePhysicals->GetNextSibling(node))
            {
                if (!node.IsOk())
                {
                    mTreePhysicals->SetItemImage(node = mTreePhysicals->AppendItem(parentNode, name), defaultImg);

                    break;
                }
            }

            parentNode = node;

            rest = rest.substr(slashAt + 1);
        }

        std::string anim = "";

        xml_node<>* node;

        // enemy / lair / door / obstacle
        if (type == "enemy" ||
            type == "lair" ||
            type == "door" ||
            type == "obstacle")
        {
            std::string animNodeName = "anim";
            std::string animNodeAttr = "name";
            if (type == "obstacle")
            {
                // czemu obstacle maja inaczej? -_-
                animNodeName = "def-anim";
                animNodeAttr = "";
            }

            node = xml.GetChild(xml.GetRootNode(), animNodeName);
            if (!node)
            {
                node = xml.GetChild(xml.GetRootNode(), "animset");
                if (node)
                    node = xml.GetChild(node, animNodeName);
            }
            if (node)
                anim = xml.GetString(node, animNodeAttr);

            SAnimation* an = gAnimationManager.GetAnimation(anim);
            if (an != NULL)     // enemy / lair / door / obstacle
            {
                wxImage img(PATH_TO_GAMEW + StringUtils::ConvertToWString(an->frames[0].frame.imageName));
                sf::IntRect rect = an->frames[0].frame.rect;
                if (rect.GetWidth() != 0 && rect.GetHeight() != 0)
                    img = img.GetSubImage(wxRect(rect.Left, rect.Top, rect.GetWidth(), rect.GetHeight()));

                mTreePhysicals->AppendItem(parentNode, rest, mTreePhysicals->GetImageList()->Add(img.Scale(TREE_ICON_SIZE)));
            }
            else
                mTreePhysicals->AppendItem(parentNode, rest);
        }
        // item / wall
        else if (type == "item" ||
            type == "wall")
        {
            node = xml.GetChild(xml.GetRootNode(), "image");
            if (node)
                anim = PATH_TO_GAME + xml.GetString(node, "filename");

            if (anim != "")    // item / wall
            {
                wxImage img(StringUtils::ConvertToWString(anim));
                if (img.IsOk())
                    mTreePhysicals->AppendItem(parentNode, rest, mTreePhysicals->GetImageList()->Add(img.Scale(TREE_ICON_SIZE)));
                else
                {
                    wxMessageBox(L"Image not found: " + StringUtils::ConvertToWString(anim) + L"\nIn file: " + *it);
                    mTreePhysicals->AppendItem(parentNode, rest);
                }
            }
            else
                mTreePhysicals->AppendItem(parentNode, rest);
        }
        else if (type == "hook")
        {
            // potencjalne TODO: jakis obrazek?
            mTreePhysicals->AppendItem(parentNode, rest);
        }
        // wtf?
        else
        {
            wxMessageBox(wxString(L"Warning: something weird found:\n\ntype = ") + StringUtils::ConvertToWString(type) + L"\nfile = " + *it);
            mTreePhysicals->AppendItem(parentNode, rest);
        }

        fprintf(stderr, "Loading physicals.. (%u/%u)\r", (unsigned int)(it - physicals.begin() + 1), physicals.size());
    }
    mTreePhysicals->Expand(root);

}

void CEditorWnd::InitTreeOther(int defaultImg)
{
    wxImage del(PATH_TO_EDITORW + L"img/delete.png");
    int delImg = mTreeOther->GetImageList()->Add(del.Scale(TREE_ICON_SIZE));

    wxTreeItemId root = mTreeOther->AddRoot(L"Other");
    mTreeOther->SetItemImage(root, defaultImg);

    mTreeOther->SetItemImage(mTreeOther->AppendItem(root, L"Erase doodahs"), delImg);
    mTreeOther->SetItemImage(mTreeOther->AppendItem(root, L"Erase physicals"), delImg);
    mTreeOther->SetItemImage(mTreeOther->AppendItem(root, L"Erase region"), delImg);
    mTreeOther->SetItemImage(mTreeOther->AppendItem(root, L"Place region"), defaultImg);
    mTreeOther->SetItemImage(mTreeOther->AppendItem(root, L"Modify object"), defaultImg);

    mTreeOther->ExpandAll();
}


/**************
 *** EVENTY ***
 **************/

void CEditorWnd::OnClose(wxCloseEvent& event)
{
    // jesli nie ma zadnej mapy, to nie ma co nawet pytac
    if (gMapManager.GetCurrent() != NULL)
    {
        int answer = wxMessageBox(L"Do you want to save your map?", L"Question", wxCENTRE | wxYES_NO | wxCANCEL, this);
        switch (answer)
        {
        case wxCANCEL:  // anuluj
            if (event.CanVeto())
            {
                event.Veto();
                return;
            }
            break;
        case wxYES: // zapisz przed wyjsciem
            {
                wxCommandEvent foo;
                OnMenuFileSaveAs(foo);
            }
        default:    // wyjdz
            break;
        }
    }

    // czyszczenie:

    // zabijamy proces
    if (wxProcess::Exists(mGameProcessId))
    {
#ifdef PLATFORM_LINUX
        wxKillError rc = wxProcess::Kill(mGameProcessId);
#else   // na windowsie procesy konsolowe trzeba sigkillem, wiki.wxwidgets.org/WxProcess
        wxKillError rc = wxProcess::Kill(mGameProcessId, wxSIGKILL);
#endif
        fprintf(stderr, "Kill(): %d\n", (int)rc);
    }

    // usuwamy tymczasowy plik z mapa, jesli gdzies zostal
    std::remove(PATH_TO_TEMP_MAP.c_str());

    // i skasuj okno
    Destroy();
}

void CEditorWnd::OnSize(wxSizeEvent& event)
{
    Layout();
}

void CEditorWnd::OnSfmlWndSize(wxSizeEvent& event)
{
    mSfml->UpdateGameWindow();
}

// zmiana zakladki
void CEditorWnd::OnTreeTabsPageChanged(wxNotebookEvent& event)
{
    // bez tego sie zakladki nie zmieniaja :o
    CMainWindow::OnTreeTabsPageChanged(event);

    // odswiezenie zaznaczenia
    wxTreeItemId item;
    switch (event.GetSelection())
    {
    case 0:
        if ((item = mTreeTiles->GetSelection()).IsOk())
            mTreeTiles->SelectItem(item);
        break;
    case 1:
        {
            // wielokrotne zaznaczenie
            wxArrayTreeItemIds idArray;
            size_t items = mTreeDoodahs->GetSelections(idArray);
            for (size_t i = 0; i < items; ++i)
                if (idArray[i].IsOk())
                    mTreeDoodahs->SelectItem(idArray[i]);

            PrepareRandomBrush();
        }
        break;
    case 2:
        {
            // wielokrotne zaznaczenie
            wxArrayTreeItemIds idArray;
            size_t items = mTreePhysicals->GetSelections(idArray);
            for (size_t i = 0; i < items; ++i)
                if (idArray[i].IsOk())
                    mTreePhysicals->SelectItem(idArray[i]);

            PrepareRandomBrush();
        }
        break;
    case 3:
        if ((item = mTreeOther->GetSelection()).IsOk())
            mTreeOther->SelectItem(item);
        break;
    default:
        break;
    }
}

// zmiana wyboru w drzewkach
void CEditorWnd::OnTreeTilesSelChanged( wxTreeEvent& event )
{
    wxString path = GetPathToItem(mTreeTiles, event.GetItem());

    // nie chcemy tu folderow
    if (mTreeTiles->GetChildrenCount(event.GetItem(), false) > 0)
        return;

    // path = sciezka do elementu
    path = PATH_TO_GAMEW + path;

    mCurrActionType = Actions::changeTile;
    mSelectedItemStr = path.c_str();
    gEditor.SetMode(CEditor::PLACEMENT_MODE);
    gEditor.SetSelected(StringUtils::ConvertToString(path.c_str()));

    mStatusBar->SetStatusText(L"Selected tile: " + path);
}

void CEditorWnd::OnTreeDoodahsSelChanged( wxTreeEvent& event )
{
    wxArrayTreeItemIds idArray;
    size_t items = mTreeDoodahs->GetSelections(idArray);
    if (items == 1)
    {
        wxString path = GetPathToItem(mTreeDoodahs, idArray[0]);

        // path = sciezka do elementu
        // nie chcemy tu folderow
        if (mTreeDoodahs->GetChildrenCount(idArray[0], false) > 0)
            return;

        path = PATH_TO_GAMEW + path;
        mSelectedItemStr = path.c_str();
        gEditor.SetSelected(StringUtils::ConvertToString(path.c_str()));
        mStatusBar->SetStatusText(L"Selected doodah: " + path);
    }
    else
        // przeladowanie losowego pedzla, jesli wiecej niz 1 wybrany element
        PrepareRandomBrush();
    
    mCurrActionType = Actions::placeDoodah;
    gEditor.SetMode(CEditor::PLACEMENT_MODE);
}

void CEditorWnd::OnTreePhysicalsSelChanged( wxTreeEvent& event )
{
    wxArrayTreeItemIds idArray;
    size_t items = mTreePhysicals->GetSelections(idArray);
    if (items == 1)
    {
        wxString path = GetPathToItem(mTreePhysicals, idArray[0]);

        // nie chcemy tu folderow
        if (mTreePhysicals->GetChildrenCount(idArray[0], false) > 0)
            return;

        // path = sciezka do elementu
        path = PATH_TO_GAMEW + path;
        
        mSelectedItemStr = path.c_str();
        gEditor.SetSelected(StringUtils::ConvertToString(path.c_str()));

        mStatusBar->SetStatusText(L"Selected physical: " + path);
    }
    else
        // przeladowanie losowego pedzla, jesli wiecej niz 1 wybrany element
        PrepareRandomBrush();
    
    mCurrActionType = Actions::placePhysical;
    gEditor.SetMode(CEditor::PLACEMENT_MODE);
}

void CEditorWnd::OnTreeOtherSelChanged( wxTreeEvent& event )
{
    wxString path = GetPathToItem(mTreeOther, event.GetItem());

    // path = sciezka do elementu
    // nie chcemy tu folderow
    if (mTreeOther->GetChildrenCount(event.GetItem(), false) > 0)
        return;

    if (mTreeOther->GetItemText(event.GetItem()) == L"Erase doodahs")
    {
        mCurrActionType = Actions::deleteDoodah;
        gEditor.SetMode(CEditor::DOODAHS_ERASING_MODE);
        mStatusBar->SetStatusText(L"Mode: erasing doodahs");
        mSelectedItemStr = L"";
    }
    else if (mTreeOther->GetItemText(event.GetItem()) == L"Erase physicals")
    {
        mCurrActionType = Actions::deletePhysical;
        gEditor.SetMode(CEditor::PHYSICALS_ERASING_MODE);
        mSelectedItemStr = L"";
        mStatusBar->SetStatusText(L"Mode: erasing physicals");
    }
    else if (mTreeOther->GetItemText(event.GetItem()) == L"Erase region")
    {
        mCurrActionType = Actions::deleteRegion;
        gEditor.SetMode(CEditor::REGIONS_ERASING_MODE);
        gEditor.SetSelected(NULL);
        mSelectedItemStr = L"";
        mStatusBar->SetStatusText(L"Mode: erasing regions");
    }
    else if (mTreeOther->GetItemText(event.GetItem()) == L"Place region")
    {
        mCurrActionType = Actions::placeRegion;
        gEditor.SetMode(CEditor::PLACEMENT_MODE);
        gEditor.SetSelected("region");
        mSelectedItemStr = L"";
        mStatusBar->SetStatusText(L"Mode: placing regions");
    }
    else if (mTreeOther->GetItemText(event.GetItem()) == L"Modify object")
    {
        mCurrActionType = Actions::modifySomething;
        gEditor.SetMode(CEditor::MODIFYING_MODE);
        mStatusBar->SetStatusText(L"Mode: modifying objects");
    }
}

// ---------------------------

// nacisniecie klawisza, z focusem na edytorze
void CEditorWnd::OnSfmlWndKeyDown( wxKeyEvent& event )
{
    //TODO: w jakis cudowny sposob wyciagnac AKTUALNE dane z edytora
    // bo oczywiscie eventy wx'ow sa wczesniej od sfmla :F
    switch (event.GetKeyCode())
    {
    case 'Z':
        // !, bo tez byloby nieaktualne, tak to chociaz to jest w porzadku
        mStatusBar->SetStatusText(std::wstring(L"Z-index: ") + (!gEditor.mZIndexFlag ? L"FOREGROUND" : L"BACKGROUND"));
        break;
    case 'I':   // invisible-walle - widocznosc on/off
        mStatusBar->SetStatusText(std::wstring(L"Invisible-walls: ") + (!gEditor.mShowInvisibleWalls ? L"visible" : L"invisible"));
        break;
    case 'M':   // modyfikacja obiektow
        mTreeTabs->ChangeSelection(3);
        mSfml->SetFocus();
        mCurrActionType = Actions::modifySomething;
        mStatusBar->SetStatusText(L"Mode: modifying objects");
        break;
    case 'H': // ukrywanie physicali i doodahow
        mStatusBar->SetStatusText(std::wstring(L"Map objects: ") + (!gEditor.mMapObjectsVisible ? L"visible" : L"invisible"));
        break;
    case WXK_UP:
    case WXK_DOWN:
        mStatusBar->SetStatusText(L"Scale: " + StringUtils::ToWString(gEditor.mScale));
        break;
    case WXK_LEFT:
    case WXK_RIGHT:
        mStatusBar->SetStatusText(L"Rotation: " + StringUtils::ToWString(gEditor.mRotation));
        break;
    default:
        break;
    }
}


void CEditorWnd::OnSfmlWndEnterWindow( wxMouseEvent& event )
{
    if (IsActive())
        mSfml->SetFocus();
}


void CEditorWnd::OnSfmlWndLeaveWindow( wxMouseEvent& event )
{
    // durny workaround na gubienie eventa
    gEditor.mMouseLeftDown = false;
    gEditor.mMouseRightDown = false;

    // skoro juz zgubilismy leftUp, to przeladujmy losowy pedzel
    PrepareRandomBrush();
    // i setfocus na widocznym drzewku, zeby mozna bylo scrollowac bez klikania
    if (IsActive())
        FocusCurrentTree();
}


void CEditorWnd::OnSfmlWndLeftDown( wxMouseEvent& event )
{
    if (gMapManager.GetCurrent() == NULL)
        return;

    // trzeba zaznaczyc, ze to modyfikacja. musimy po usunieciu
    // zmienic mode na place*, bo bedzie kuku ;c
    bool wasModifyingSomething = false;
    if (mCurrActionType == Actions::modifySomething && gEditor.mSelectedToErase)
    {
        // nie wiemy, co chcemy usuwac, wiec trzeba to najpierw ustalic
        CSceneNode* node = gEditor.mSelectedToErase;
        if (node->GetPhysical())    // jakis physical?
        {
            if (dynamic_cast<CDynamicRegion*>(node->GetPhysical())) // region?
                mCurrActionType = Actions::deleteRegion;
            else    // inny physical
                mCurrActionType = Actions::deletePhysical;
        }
        else if (node->GetDisplayable())    // doodah
            mCurrActionType = Actions::deleteDoodah;
        
        wasModifyingSomething = true;

        // jeszcze zmiana drzewka, zeby nie bylo problemow...
        switch (mCurrActionType)
        {
        case Actions::deleteDoodah:     mTreeTabs->ChangeSelection(1);  break;
        case Actions::deletePhysical:   mTreeTabs->ChangeSelection(2);  break;
        case Actions::deleteRegion:     mTreeTabs->ChangeSelection(3);  break;
        default:    break; // jakis error po drodze
        }

        // jeszcze trzeba odswiezyc fokus na kontrolce po zmianie zakladki
        mSfml->SetFocus();
    }

    switch (mCurrActionType)
    {
    case Actions::changeTile:
        // przeniesione do CTileResource::Place, zeby undo dzialalo po kliku z przeciagnieciem
        break;
    case Actions::deleteDoodah:
        {
            if (gEditor.mSelectedToErase && gMapManager.GetCurrent())
            {
                int doodahDescId = gEditor.mSelectedToErase->GetDoodahDescriptorId();
                Map::CMap* map = gMapManager.GetCurrent();

                for (std::vector<Map::CDoodahDescriptor*>::iterator i = map->mDoodahDescriptors.begin(); i != map->mDoodahDescriptors.end(); ++i )
		            if ((*i)->uniqueId == doodahDescId)
                    {
                        Map::CDoodahDescriptor* doodah = *i;
                        gActionsList.Push(SAction::DeleteDoodah(doodah->x, doodah->y, doodah->zindex, doodah->rotation, doodah->scale, doodah->file));
			            break;
		            }
            }
        }
        break;
    case Actions::placeDoodah:
        {
            if (gMapManager.GetCurrent())
            {
                sf::Vector2f pos = gEditor.GetMousePosInWorld();
                gEditor.LockMousePos();
                gActionsList.Push(SAction::PlaceDoodah(pos.x, pos.y, gEditor.mZIndexFlag ? Z_MAPSPRITE_FG : Z_MAPSPRITE_BG, gEditor.mRotation, gEditor.mScale, StringUtils::ConvertToString(mSelectedItemStr)));
            }
        }
        break;
    case Actions::deletePhysical:
        {
            if (gEditor.mSelectedToErase && gMapManager.GetCurrent())
            {
                int physicalDescId = gEditor.mSelectedToErase->GetMapObjectDescriptorId();
                Map::CMap* map = gMapManager.GetCurrent();

                for (std::vector<Map::CMapObjectDescriptor*>::iterator i = map->mMapObjectDescriptors.begin(); i != map->mMapObjectDescriptors.end(); ++i )
		            if (((*i)->uniqueId == physicalDescId))
                    {
                        Map::CMapObjectDescriptor* physical = *i;
                        gActionsList.Push(SAction::DeletePhysical(physical->x, physical->y, physical->rot, physical->code, physical->name));
			            break;
		            }
            }
        }
        break;
    case Actions::placePhysical:
        {
            if (gMapManager.GetCurrent())
            {
                sf::Vector2f pos = gEditor.GetMousePosInWorld();
                gEditor.LockMousePos();
                std::string code = gMapManager.GetCurrent()->GetOrCreateMapObjectCode(gResourceManager.GetPhysicalTemplate(StringUtils::ConvertToString(mSelectedItemStr)));
                gActionsList.Push(SAction::PlacePhysical(pos.x, pos.y, gEditor.mRotation, code));
            }
        }
        break;
    case Actions::deleteRegion:
        {
            CSceneNode* node = gScene.GetPhysicalAt(gEditor.GetMousePosInWorld());
            if (node != NULL && gMapManager.GetCurrent())
            {
                CPhysical* phys = node->GetPhysical();
                if (CDynamicRegion* rgn = dynamic_cast<CDynamicRegion*>(phys))
                {
                    Map::CRegionDescriptor* desc = gMapManager.GetCurrent()->GetRegionDescriptorById(rgn->GetDescriptorId());
                    if (desc)
                    {
                        gEditor.LockMousePos();
                        gActionsList.Push(SAction::DeleteRegion(rgn->GetPosition().x, rgn->GetPosition().y, rgn->GetScale(), StringUtils::ConvertToString(rgn->GetUniqueId()), desc->Serialize()));
                    }
                    else
                        wxMessageBox(L"Bug! Region descriptor doesn't exist!");

                    // bez sprawdzania poprawnosci..
                    gEditor.mSelectedToErase = NULL;
                    gMapManager.GetCurrent()->EraseRegion(rgn->GetDescriptorId());
                    rgn->MarkForDelete();
                }
            }
        }
        break;
    case Actions::placeRegion:
        {
            if (gEditor.GetSelected() && gMapManager.GetCurrent())
            {
                sf::Vector2f pos = gEditor.GetMousePosInWorld();
                gEditor.GetSelected()->Place(pos, 0, gEditor.mScale, false);
                // Place() musi byc tu, zeby iterator sie nie #$%^%$%#
                Map::CRegionDescriptor* desc = *(--gMapManager.GetCurrent()->mRegionDescriptors.end());
                if (desc)
                    gActionsList.Push(SAction::PlaceRegion(pos.x, pos.y, gEditor.mScale, "region" + StringUtils::ToString(desc->uniqueId), ""));
            }
        }
        break;
    default:
        break;
    }

    // i teraz, jesli mamy modyfikowac, to bedziemy stawiac:
    if (wasModifyingSomething)
    {
        switch (mCurrActionType)
        {
        case Actions::deleteDoodah:     mCurrActionType = Actions::placeDoodah;     break;
        case Actions::deletePhysical:   mCurrActionType = Actions::placePhysical;   break;
        case Actions::deleteRegion:
            // jako, ze regiony sa prawie w calosci obslugiwane tu,
            // a nie w CEditor, trzeba przestawic tryb edytora
            mCurrActionType = Actions::placeRegion;
            gEditor.SetMode(CEditor::PLACEMENT_MODE);
            break;
        default:    break; // jakis error po drodze
        }
    }
}

void CEditorWnd::OnSfmlWndLeftUp(wxMouseEvent& event)
{
    // uaktualnienie pedzla losujacego
    // tutaj, zeby po kliknieciu bylo widac nastepnego physicala
    PrepareRandomBrush();
}

// ---------------------------

// menu - file
void CEditorWnd::OnMenuFileNew( wxCommandEvent& event )
{
    if (mMapSizeDlg == NULL)
        mMapSizeDlg = new CMapSizeDlg(this);

    // defaultowy rozmiar mapy
    mMapSizeDlg->mXNum->SetValue(32);
    mMapSizeDlg->mYNum->SetValue(32);

    if (mMapSizeDlg->ShowModal() == wxID_CANCEL)
        return;

    // czyszczenie mapy w pamieci, zeby na pewno przeladowac
    gMapManager.SetMap("", true);
    gResourceManager.DropResource(PATH_TO_GAME + PATH_TO_TEMP_MAP);

    gMapManager.SaveEmptyMap(PATH_TO_GAME + PATH_TO_TEMP_MAP, sf::Vector2i(mMapSizeDlg->mXNum->GetValue(), mMapSizeDlg->mYNum->GetValue()));
    gMapManager.SetMap(PATH_TO_GAME + PATH_TO_TEMP_MAP, true);
    std::remove((PATH_TO_GAME + PATH_TO_TEMP_MAP).c_str());

    mStatusBar->SetStatusText(L"New map created!");

    mCurrentMap = L"";
}

void CEditorWnd::OnMenuFileOpen( wxCommandEvent& event )
{
    wxFileDialog fileDialog(this, L"Select map file", wxEmptyString, wxEmptyString, L"XML files (*.xml)|*.xml", wxFD_OPEN);
    if (fileDialog.ShowModal() == wxID_CANCEL)
        return;

    std::string filePath = StringUtils::ConvertToString(fileDialog.GetPath().c_str());

    // wymus przeladowanie
    gMapManager.SetMap("", true);
    gResourceManager.DropResource(filePath);
    gResourceManager.LoadMap(filePath);

    if (gMapManager.SetMap(filePath, true))
    {
        mCurrentMap = fileDialog.GetPath().c_str();
        mStatusBar->SetStatusText(L"Loaded map: " + mCurrentMap);
    }

    else
    {
        mCurrentMap = L"";
        mStatusBar->SetStatusText(L"Failed loading map: " + fileDialog.GetPath());
    }
}

/*void CEditorWnd::OnMenuFileLoadMod(wxCommandEvent& event)
{
    wxDirDialog dirDialog(this, L"Select mod directory", wxEmptyString);
    if (dirDialog.ShowModal() == wxID_CANCEL)
        return;
}*/

void CEditorWnd::OnMenuFileSave( wxCommandEvent& event )
{
    if (mCurrentMap == L"")
    {
        OnMenuFileSaveAs(event);
        return;
    }

    if (gMapManager.SaveMap(StringUtils::ConvertToString(mCurrentMap)))
        mStatusBar->SetStatusText(L"Map saved: " + mCurrentMap);
    else
        mStatusBar->SetStatusText(L"Error saving map!");
}

void CEditorWnd::OnMenuFileSaveAs( wxCommandEvent& event )
{
    wxFileDialog fileDialog(this, L"Select map file", wxEmptyString, wxEmptyString, L"*.xml", wxFD_SAVE);
    if (fileDialog.ShowModal() == wxID_CANCEL)
        return;

    if (gMapManager.SaveMap(StringUtils::ConvertToString(fileDialog.GetPath().c_str())))
    {
        mCurrentMap = fileDialog.GetPath();
        mStatusBar->SetStatusText(L"Map saved: " + mCurrentMap);
    }
    else
        mStatusBar->SetStatusText(L"Error saving map!");
}

void CEditorWnd::OnMenuFileExit( wxCommandEvent& event )
{
    Close();
}


// menu - edit
void CEditorWnd::OnMenuEditUndo( wxCommandEvent& event )
{
    SAction action = gActionsList.Undo();
    HandleAction(action);
}

void CEditorWnd::OnMenuEditRedo( wxCommandEvent& event )
{
    SAction action = gActionsList.Redo();
    HandleAction(action);
}

void CEditorWnd::OnMenuEditResize(wxCommandEvent& event)
{
    if (gMapManager.GetCurrent())
    {
        wxMessageBox(L"Warning:\n\n"
            L"- resizing a map will erase your undo-redo list.\n"
            L"- the top-left corner of the map will remain unmodified, right/bottom "
            L"sides are cut off if the new size is less than current.\n"
            L"- all objects outside the bounds of a resized map will be irretrievably deleted.",
            L"Warning", wxICON_EXCLAMATION, this);

        if (mMapSizeDlg == NULL)
            mMapSizeDlg = new CMapSizeDlg(this);

        // uaktualnianie liczb
        Map::CMap* map = gMapManager.GetCurrent();
        mMapSizeDlg->mXNum->SetValue(map->GetSize().x);
        mMapSizeDlg->mYNum->SetValue(map->GetSize().y);

        // pobieranie inputu od uzytkownika
        if (mMapSizeDlg->ShowModal() == wxID_CANCEL)
            return;

        // zaznaczony w drzwku kafel staje sie domyslnym
        std::string defaultTile;
        if (mTreeTiles->GetSelection().IsOk())
            if (mTreeTiles->GetChildrenCount(mTreeTiles->GetSelection(), false) == 0) // bez folderow
            {
                wxString path = GetPathToItem(mTreeTiles, mTreeTiles->GetSelection());
                defaultTile = StringUtils::ConvertToString(path.c_str());
            }

        // zmiana rozmiaru
        if (!map->Resize(mMapSizeDlg->mXNum->GetValue(), mMapSizeDlg->mYNum->GetValue(), defaultTile))
            wxMessageBox(L"An error occured when trying to resize map. Check console for more info.", L"Error");
        else
        {
            // odswiezenie mapy - save-load
            map->Save(PATH_TO_TEMP_MAP);

            // bo byly jakies problemy z SetCurrentMapAsVisited
            gMapManager.SetMap("", true);

            // jesli sie da, usun poprzednia mape z pamieci, i wczytaj ta zedytowana
            gResourceManager.DropResource(PATH_TO_TEMP_MAP);
            // wymus wczytanie, zeby SetMap nie chcial jakiegos 0xfeeefeee czytac
            gResourceManager.LoadMap(PATH_TO_TEMP_MAP);

            gMapManager.SetMap(PATH_TO_TEMP_MAP, true);

            // czysczenie undo/redo
            gActionsList.Clear();
        }
    }
    else
        mStatusBar->SetStatusText(L"Open a map first!");
}

void CEditorWnd::OnMenuEditRandomBrushProperties(wxCommandEvent& event)
{
    if (mRandomBrushPropertiesDlg == NULL)
        mRandomBrushPropertiesDlg = new CRandomBrushPropertiesDlg(this);

    if (mRandomBrushPropertiesDlg->ShowModal() == wxID_CANCEL)
        return;

    // zmiana paranetrow pedzla
    mRandomBrushMinScale = (float)mRandomBrushPropertiesDlg->mMinScale->GetValue() / 100.f;
    mRandomBrushMaxScale = (float)mRandomBrushPropertiesDlg->mMaxScale->GetValue() / 100.f;

    // jesli ktos byl tak wredny, ze ustawil odwrotnie, to trzeba go przechytrzyc
    if (mRandomBrushMinScale > mRandomBrushMaxScale)
    {
        float tmp = mRandomBrushMinScale;
        mRandomBrushMinScale = mRandomBrushMaxScale;
        mRandomBrushMaxScale = tmp;
    }

    // odswiezenie pedzla
    PrepareRandomBrush();
}

void CEditorWnd::OnMenuEditRun( wxCommandEvent& event )
{
    // mamy w ogole jakas mape?
    if (gMapManager.GetCurrent())
    {
        // bez ./, bo gGameOptions wyrzuca bezwzgledna sciezke
        std::string mapPath = PATH_TO_TEMP_MAP;
        gMapManager.SaveMap(mapPath);

        // uruchamianie - lecimy po kolei po mozliwych nazwach
        size_t binaryNamesCount = 2;
        std::string binaryNames[] = 
#ifdef PLATFORM_LINUX   // linux/mac
        { "./warsztat_game", "./warlocks_gauntlet" };
#else // PLATFORM_WINDOWS
        { "./WarsztatGame.exe", "./WarlocksGauntlet.exe" };
#endif
        
        // argumenty
        std::string arguments = " --quick-load-map " + mapPath;

        if (wxProcess::Exists(mGameProcessId))
            wxMessageBox(L"Close an existing game window first!");
        else
        {
            // sprawdzamy wszystkie nazwy
            for (size_t i = 0; i < binaryNamesCount; ++i)
            {
                std::string command = binaryNames[i] + arguments;
                fprintf(stderr, ">> %s\n", command.c_str());

                // czy plik w ogole istnieje? bo wxExecute wywala brzydkie okno, jesli nie
                if (boost::filesystem::exists(binaryNames[i]))
                {
                    mGameProcessId = wxExecute(StringUtils::ConvertToWString(command).c_str(), wxEXEC_ASYNC);
                    if (mGameProcessId == 0)    // blad
                        fprintf(stderr, ">> executing file \"%s\" failed!\n", binaryNames[i].c_str());
                    else
                        break;  // udalo sie utworzyc, olewamy reszte
                }
                else
                    fprintf(stderr, ">> file not found: %s\n", binaryNames[i].c_str());
            }

            fprintf(stderr, ">> wxExecute(): created process' id = %d\n", mGameProcessId);

            // udalo sie?
            if (mGameProcessId <= 0)
                wxMessageBox(L"Something went wrong, check console for more info.", L"Error");
        }
    }
}

void CEditorWnd::OnMenuEditViewXMLSource( wxCommandEvent& event )
{
    if (mXmlSourceDlg == NULL)
        mXmlSourceDlg = new CXmlSourceDialog(this);

    if (!gMapManager.GetCurrent())
    {
        mStatusBar->SetStatusText(L"Open a map first -.-");
        return;
    }

    std::stringstream ss;
    gMapManager.GetCurrent()->Serialize(ss);

    mXmlSourceDlg->mText->SetText(StringUtils::ConvertToWString(ss.str()));
    mXmlSourceDlg->Show(true);
}


// menu - other
void CEditorWnd::OnMenuOtherHelp( wxCommandEvent& event )
{
    if (mHelpDlg == NULL)
        mHelpDlg = new CHelpDialog(this);

    mHelpDlg->Show(true);
}

void CEditorWnd::OnMenuOtherAbout( wxCommandEvent& event )
{
    wxMessageBox(L"Not yet implemented..", L":C");
}
