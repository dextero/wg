#ifndef WORLD_GRAPH_H
#define WORLD_GRAPH_H

#include <string>
#include <map>
#include <vector>

struct CWorldGraphExit {
    std::string toMap;
    std::string onBorder;
    std::string toEntry;
    bool blocked;
};

struct CWorldGraphMap {
    std::string id;
    std::string scheme;
    int level;
    std::vector<CWorldGraphExit> exits;
    std::string boss;
    bool final;
};

struct CWorldGraph {
    std::string startingMap;
    std::string startingRegion;
    std::map<std::string, CWorldGraphMap> maps;
        
	CWorldGraph();
    void LoadFromFile(const std::string & filename);
};

#endif
