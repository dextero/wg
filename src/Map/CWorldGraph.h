#ifndef WORLD_GRAPH_H
#define WORLD_GRAPH_H

#include <string>
#include <map>
#include <vector>

#include <SFML/System/Vector2.hpp>

struct CWorldGraphExit {
    std::string toMap;
    std::string onBorder;
    std::string toEntry;
    bool blocked;

};

struct CWorldGraphMap {
    enum {
        MAP_LEVEL_CITY = 0
    };

    std::string id;
    std::string scheme;
    int level;
    std::vector<CWorldGraphExit> exits;
    std::string boss;
    bool final;
    sf::Vector2f mapPos;
};

struct CWorldGraph {
    std::string startingMap;
    std::string startingRegion;
    std::map<std::string, CWorldGraphMap> maps;

    struct WorldScheme {
        sf::Vector2f center;
        std::string name;

        WorldScheme(const sf::Vector2f& pos, const std::string& name): center(pos), name(name) {}
    };
    std::vector<WorldScheme> schemes;
        
	CWorldGraph();
    void LoadFromFile(const std::string & filename);
    void SaveToFile(const std::string & filename);

    void Generate(unsigned nodes);

    const std::string GetSchemeFromPos(const sf::Vector2f& pos);
};

#endif
