#include "CHelpDialog.h"

#include "Utils/StringUtils.h"

// PATH_TO_GAME
#include "macros.h"

#include <vector>
#include <string>
#include <boost/filesystem/operations.hpp>


CHelpDialog::CHelpDialog(wxWindow* parent, wxWindowID id):
    CHelpWindow(parent, id)
{
    // dodawanie elementow do drzewka; wczytywanie listy z folderow
    std::vector<std::string> foundDirectories;
    std::vector<std::wstring> foundFiles;

    if (!boost::filesystem::exists(PATH_TO_EDITOR_HELP))
    {
        wxMessageBox(L"Cannot find help contents!");
        return;
    }

	foundDirectories.push_back(PATH_TO_EDITOR_HELP);
	while (foundDirectories.size() > 0)
    {
		std::string dir = foundDirectories[foundDirectories.size()-1];
		foundDirectories.pop_back();
		boost::filesystem::directory_iterator di(dir),dir_end;
		for (; di != dir_end; di++)
        {
			if (di->leaf() == ".svn")
				continue;
			std::string file = dir + "/" + di->leaf();
			if (boost::filesystem::is_directory(file))
				foundDirectories.push_back(file);
			else if (file.substr(file.size() - 5) == ".html")
                foundFiles.push_back(StringUtils::ConvertToWString(file.substr(PATH_TO_EDITOR_HELP.size() + 1, file.size() - PATH_TO_EDITOR_HELP.size() - 6)));
		}
	}
    
    wxTreeItemId root = mHelpTree->AddRoot(L"Help contents");
    wxTreeItemId parentNode;
    std::wstring rest, name;
    size_t slashAt = std::wstring::npos;

    for (std::vector<std::wstring>::iterator it = foundFiles.begin(); it != foundFiles.end(); ++it)
    {
        parentNode = root;

        rest = *it;

        while ((slashAt = rest.find(L"/")) != std::wstring::npos)
        {
            name = rest.substr(0, slashAt);

            wxTreeItemIdValue wtfIsThis;
            wxTreeItemId node = mHelpTree->GetFirstChild(parentNode, wtfIsThis);
            for (; !node.IsOk() || (mHelpTree->GetItemText(node) != name); node = mHelpTree->GetNextSibling(node))
            {
                if (!node.IsOk())
                {
                    node = mHelpTree->AppendItem(parentNode, name);
                    break;
                }
            }

            parentNode = node;
            rest = rest.substr(slashAt + 1);
        }

        mHelpTree->AppendItem(parentNode, rest);
        fprintf(stderr, "Loading help.. (%u/%u)\r", (unsigned int)(it - foundFiles.begin() + 1), foundFiles.size());
    }

    mHelpTree->Expand(root);
}


void CHelpDialog::OnCloseWindow(wxCloseEvent& event)
{
    this->Hide();
}

void CHelpDialog::OnTreeSelChanged(wxTreeEvent& event)
{
    if (event.GetItem() == mHelpTree->GetRootItem())
        return;

    wxString path = mHelpTree->GetItemText(event.GetItem());
    for (wxTreeItemId id = mHelpTree->GetItemParent(event.GetItem()); id.IsOk() && id != mHelpTree->GetRootItem(); id = mHelpTree->GetItemParent(id))
        path = mHelpTree->GetItemText(id) + L"/" + path;

    // path = sciezka do elementu
    path = PATH_TO_EDITOR_HELPW + path + L".html";

    mHelpHtmlContent->LoadFile(path);
}

void CHelpDialog::OnHtmlLinkClicked(wxHtmlLinkEvent& event)
{
    wxString link = event.GetLinkInfo().GetHref();
    wxString file = mHelpHtmlContent->GetOpenedPage();
    file = file.substr(file.find(_T(EDITOR_HELP_FOLDER)));

    size_t lastSlash = file.rfind('/') + 1;
    link = file.substr(0, lastSlash) + link;

    for (size_t i = link.rfind(L"../", wxString::npos); i != wxString::npos; i = link.rfind(L"../", i))
    {
        size_t j = link.rfind(L'/', i);
        if (j != std::string::npos && link.substr(j, 3) != L"../")
        {
            link.erase(j, i + 2);
            i = wxString::npos;
        }
    }

    if (link.size() > 5 && link.substr(link.size() - 5) == L".html")
        link = link.substr(0, link.size() - 5);

    wxTreeItemId id = mHelpTree->GetRootItem();
    while (link.size())
    {
        wxString txt = link;
        size_t slashAt = link.find(L'/');
        if (slashAt != std::string::npos)
            txt = link.substr(0, slashAt);

        wxTreeItemIdValue wtf;
        for (wxTreeItemId i = mHelpTree->GetFirstChild(id, wtf); i.IsOk(); i = mHelpTree->GetNextChild(id, wtf))
            if (mHelpTree->GetItemText(i) == txt)
            {
                id = i;
                break;
            }

        if (slashAt != wxString::npos)
            link = link.substr(slashAt + 1);
        else
            link = L"";
    }

    mHelpTree->SelectItem(id);
}