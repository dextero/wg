#include "CWorldGraph.h"
#include "../Utils/CXml.h"

CWorldGraph::CWorldGraph()	
{
}

void CWorldGraph::LoadFromFile(const std::string & filename) {
    CXml xml(filename, "root");
    xml_node<>* root = xml.GetRootNode();
    if (!root) {
        return;
    }
    if (xml.GetString(root, "type") != "worldGraph") {
        fprintf(stderr, "CWorldGraph::LoadFromFile: invalid XML type \"%s\", \"worldGraph\" expected", xml.GetString(root, "type").c_str());
        return;
    }
    startingMap = xml.GetString(root, "startingMap");
    startingRegion = xml.GetString(root, "startingRegion");

    maps.clear();
    for (rapidxml::xml_node<>* map = xml.GetChild(root, "map"); map; map = xml.GetSibl(map, "map")) {
        CWorldGraphMap graphMap;
        graphMap.id = xml.GetString(map, "id");
        graphMap.scheme = xml.GetString(map, "scheme");
        graphMap.level = xml.GetInt(map, "level");

        for (rapidxml::xml_node<>* exit = xml.GetChild(map, "exit"); exit; exit = xml.GetSibl(exit, "exit")) {
            CWorldGraphExit graphExit;
            graphExit.toMap = xml.GetString(exit, "toMap");
            graphExit.onBorder = xml.GetString(exit, "onBorder");
            graphExit.toEntry = xml.GetString(exit, "toEntry");
            graphMap.exits.push_back(graphExit);
        }
        
        maps[graphMap.id] = graphMap;
    }
}