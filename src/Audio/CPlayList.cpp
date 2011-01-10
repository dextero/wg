#include "CPlayList.h"
#include "../Utils/StringUtils.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Console/CConsole.h"

#include <RapidXML/rapidxml_utils.hpp>

namespace Audio
{

void CPlayList::AddSong(System::Resource::CMusic *newSong)
{
    mSongs.push_back(newSong);
}

bool CPlayList::Load(const std::wstring &filename)
{
    rapidxml::file<char> fileIn(StringUtils::ConvertToString(filename).c_str());
    rapidxml::xml_document<char> doc;
    rapidxml::xml_node<char> *root=0;

    doc.parse< rapidxml::parse_declaration_node >( fileIn.data() );
    root = doc.first_node("root");

    mRandomizable = false;
    if(root->first_attribute("random")->value() == std::string("true"))
    {
        mRandomizable = true;
    }

    if(std::string(root->first_attribute("type")->value()) != std::string("playlist"))
        return false;

    System::Resource::CMusic *tmpMusicPtr=0;
    for(rapidxml::xml_node<char> *node=root->first_node("song"); node; node=node->next_sibling())
    {
        tmpMusicPtr = gResourceManager.GetMusic(node->first_attribute("name")->value());
        if(tmpMusicPtr)
            this->AddSong(tmpMusicPtr);
    }

    return mSongs.size() > 0;
}

}
