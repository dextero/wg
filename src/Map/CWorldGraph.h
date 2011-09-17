#ifndef WORLD_GRAPH_H
#define WORLD_GRAPH_H

#include <string>
#include <map>
#include <vector>

struct CWorldGraphExit {
    std::string toMap;
    std::string onBorder;
};

struct CWorldGraphMap {
    std::string id;
    std::string scheme;
    int level;
    std::vector<CWorldGraphExit> exits;
};

struct CWorldGraph {
    std::string startingMap;
    std::string startingRegion;
    std::map<std::string, CWorldGraphMap> maps;
        
	CWorldGraph();
    void LoadFromFile(const std::string & filename);
};

#endif