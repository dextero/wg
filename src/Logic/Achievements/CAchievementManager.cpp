#include "CAchievementManager.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../VFS/vfs.h"
#include "../../Console/CConsole.h"
#include "../../Commands/CCommands.h"
#include "../../CGameOptions.h"

template<> CAchievementManager* CSingleton<CAchievementManager>::msSingleton = 0;

void CAchievementManager::Load(const std::string& file)
{
    CXml xml(file, "root");

    if (xml.GetString(xml.GetRootNode(), "type") != "achievements")
    {
        fprintf(stderr, "ERROR loading achievements: invalid XML type: %s (\"achievements\" expected)\n", xml.GetString(xml.GetRootNode(), "type").c_str());
        return;
    }

    for (rapidxml::xml_node<>* node = xml.GetChild(xml.GetRootNode(), "achievement"); node; node = node->next_sibling("achievement"))
    {
        SAchievement a;
        a.name = xml.GetString(xml.GetChild(node, "name"), "value");
        a.desc = xml.GetString(xml.GetChild(node, "desc"), "value");
        a.image = xml.GetString(xml.GetChild(node, "image"), "value");
        a.completed = !!xml.GetInt(xml.GetChild(node, "completed"), "value");

        mData.push_back(a);
    }

    mLoadedFilePath = file;
}

void CAchievementManager::Save(const std::string& file) const
{
    std::ofstream f(file.c_str(), std::ios_base::trunc);
    if (f.is_open())
    {
        f << "<root type=\"achievements\">\n";
        for (std::vector<SAchievement>::const_iterator i = mData.begin(); i != mData.end(); ++i)
            f << "\t<achievement>\n"
                << "\t\t<name value=\"" << i->name.c_str() << "\" />\n"
                << "\t\t<desc value=\"" << i->desc.c_str() << "\" />\n"
                << "\t\t<image value=\"" << i->image.c_str() << "\" />\n"
                << "\t\t<completed value=\"" << (int)i->completed << "\" />\n"
              << "\t</achievement>\n";
        f << "</root>";

        f.close();
    }
    else
	{
		//src/Logic/Achievements/CAchievementManager.cpp:51: warning: cannot pass objects of non-POD type 'const struct std::basic_string<char, std::char_traits<char>, std::allocator<char> >' through '...'; call will abort at runtime
		//tu akurat siê bug czai³ :)
        fprintf(stderr, "ERROR saving achievements data: couldn't open file for writing: %s\n", file.c_str());
	}
}


void CAchievementManager::Clear()
{
    for (std::vector<SAchievement>::iterator i = mData.begin(); i != mData.end(); ++i)
        i->completed = false;
}

void CAchievementManager::AddAchievement(size_t argc, const std::vector<std::wstring>& argv)
{
    // falszywa nadzieja..
    if (argc < 2)
    {
        gConsole.Printf(L"usage: %ls achievement-name", argv[0].c_str());
        return;
    }

    // zabezpieczenie anty-cheaterowe
    /*if (gConsole.GetVisible())
    {
        // die, cheaters! mwhahaha ]:->
        Clear();
        Save(mLoadedFilePath);
        // TODO: czyszczenie flag QuestManagera, jak kasowac, to kasowac.
        //gConsole.Printf(L"You have failed, please die. Oh, and you just accidentally lost all completed achievements. Have fun :-)");
    }
    else*/
    {
        std::string name = StringUtils::ConvertToString(argv[1]);
        for (std::vector<SAchievement>::iterator i = mData.begin(); i != mData.end(); ++i)
            if (i->name == name)
            {
                // cœœœ, ukrywamy przed graczem..
                /*if (!i->completed)
                {
                    gConsole.Printf(L"Achievement \"%ls\" unlocked!", argv[1].c_str());
                    gCommands.ParseCommand(std::wstring(L"msg $ACH_COMPLETED ") + argv[1]);
                }*/
                i->completed = true;
                Save(gGameOptions.GetUserDir() + "/achievements.xml");

                return;
            }

        gConsole.Printf(L"Error: achievement \"%ls\" not found!", argv[1].c_str());
    }
}
