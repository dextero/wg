#include "CSpriteRender.h"
#include "../../../Utils/CXml.h"
#include "../../../ResourceManager/CResourceManager.h"
#include "../../../ResourceManager/CImage.h"

template<> part::renderer::CSpriteRenderer * CSingleton<part::renderer::CSpriteRenderer>::msSingleton = 0;

bool part::renderer::CSpriteRenderer::LoadConfiguration(const std::string &file)
{
    CXml xml(file,"root");

    if (xml.GetString(0,"type") != "particle-renderer")
        return false;

    xml_node<> *node = xml.GetChild(0,"spriterenderer");

    std::string a = xml.GetString(node,"images");

    mTextureAtlas = gResourceManager.GetImage(a);
    mSprite.SetImage(*mTextureAtlas);

    int x,y,w,h;
    for (node = xml.GetChild(node,"img"); node; node = xml.GetSibl(node,"img"))
    {
        x = xml.GetInt(node,"x-offset");
        y = xml.GetInt(node,"y-offset");
        w = xml.GetInt(node,"w");
        h = xml.GetInt(node,"h");
        
        mTextureOffsets.push_back(sf::Rect<int>(x,y,x+w,y+h));
    }    

    return true;
}
