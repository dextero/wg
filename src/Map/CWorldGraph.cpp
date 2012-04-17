#include "CWorldGraph.h"
#include "../Utils/CXml.h"
#include "../Utils/Maths.h"
#include "../Utils/StringUtils.h"

#include <SFML/Graphics/Image.hpp>

#include <algorithm>
#include <queue>
#include <string.h>


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
        graphMap.boss = xml.GetString(map, "boss");
        graphMap.final = xml.GetInt(map, "final") != 0;
        graphMap.mapPos = sf::Vector2f(xml.GetFloat(map, "posx"), xml.GetFloat(map, "posy"));

        for (rapidxml::xml_node<>* exit = xml.GetChild(map, "exit"); exit; exit = xml.GetSibl(exit, "exit")) {
            CWorldGraphExit graphExit;
            graphExit.toMap = xml.GetString(exit, "toMapvoid Generate(unsigned nodes);");
            graphExit.onBorder = xml.GetString(exit, "onBorder");
            graphExit.toEntry = xml.GetString(exit, "toEntry");
            graphExit.blocked = !xml.GetString(exit, "blocked").empty();
            graphMap.exits.push_back(graphExit);
        }
        
        maps[graphMap.id] = graphMap;
    }
}

void CWorldGraph::SaveToFile( const std::string & filename )
{
    std::ofstream file(filename.c_str(), std::ios::out);
    if (!file.is_open())
    {
        fprintf(stderr, "error: couldn't open file %s for writing (CWorldGraph::SaveToFile)\n", filename.c_str());
        return;
    }

    file << "<root type=\"worldGraph\">\n"
         << "    <startingMap>" << startingMap.c_str() << "</startingMap>\n"
         << "    <startingRegion>" << startingRegion.c_str() << "</startingRegion>\n";

    for (std::map<std::string, CWorldGraphMap>::iterator it = maps.begin(); it != maps.end(); ++it)
    {
        file << "    <map id=\"" << it->second.id.c_str()
                   << "\" scheme=\"" << it->second.scheme.c_str()
                   << "\" level=\"" << it->second.level
                   << "\" posx=\"" << it->second.mapPos.x
                   << "\" posy=\"" << it->second.mapPos.y << "\" ";

        if (it->second.boss.size() > 0)
            file << "boss=\"" << it->second.boss.c_str() << "\" ";
        if (it->second.final)
            file << "final=\"1\" ";

        file << "\">\n";

        for (std::vector<CWorldGraphExit>::iterator exit = it->second.exits.begin(); exit != it->second.exits.end(); ++exit)
        {
            file << "        <exit toMap=\"" << exit->toMap.c_str() << "\" ";
            if (exit->toEntry.size() > 0)
                file << "toEntry=\"" << exit->toEntry.c_str() << "\" ";
            if (exit->blocked)
                file << "blocked=\"true\" ";
            file << "onBorder=\"" << exit->onBorder.c_str() << "/>\n";
        }

        file << "    </map>\n";
    }
    file << "</root>";
}


// struktury - generowanie swiata
class WorldGraphGenerator
{
public:
    struct Node {
        sf::Vector2f pos;
        std::vector<size_t> edges;  // indeksy w wektorze nodes
        unsigned borders;           // zajete sciany
        std::string scheme;
        bool isTown;

        enum Border {
            BorderWest = 1 << 0,
            BorderNorth = 1 << 1,
            BorderEast = 1 << 2,
            BorderSouth = 1 << 3
        };

        Node(sf::Vector2f pos): pos(pos), borders(0), isTown(false) {}

        bool BorderBlocked(unsigned index) { return (borders & (1 << index)) != 0; }
        void BlockBorder(unsigned index) { borders |= (1 << index); }
    };

    struct Edge {
        float weight;
        size_t from, to;

        Edge(float weight, size_t from, size_t to): weight(weight), from(from), to(to) {}
    };

    struct Scheme {
        sf::Vector2f center;
        std::string name;

        Scheme(const sf::Vector2f& pos, const std::string& name): center(pos), name(name) {}
    };

    // porownywarka do Edge
    struct EdgeWeightLess
    {
        bool operator ()(const Edge& first, const Edge& second)
        {
            return first.weight < second.weight;
        }
    };
    
   
    // funkcje pomocnicze - generowanie swiata
    static unsigned GetBorderIndex(const sf::Vector2f& from, const sf::Vector2f& dest)
    {
        float angle = Maths::AngleBetween(Maths::VectorRight(), dest - from);
        //fprintf(stderr, "vec = (%f %f) angle = %f, border = %s\n", dest.x - from.x, dest.y - from.y, angle, borders[(int)((angle + 225.f) / 90.f) % 4].c_str());
        angle += 225.f;

        return (unsigned)(angle / 90.f) % 4;
    }

    // nazwa granicy, na ktorej ma byc polaczenie, zeby bylo po ludzku
    // a nie tak, ze wyjscie na wschodzie prowadzi do mapy na zachodzie
    static const std::string& GetBorderName(const sf::Vector2f& from, const sf::Vector2f& dest)
    {
        static std::string borders[] = {
            "west", "north", "east", "south"
        };

        return borders[GetBorderIndex(from, dest)];
    }


private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<Scheme> schemes;
    unsigned char collisionMap[200][200];

    // szerokosc 'ramki', w ktorej nie bedzie wezlow, w %
    float borderSize;

    unsigned GetBorderIndex(unsigned from, unsigned to)
    {
        return GetBorderIndex(nodes[from].pos, nodes[to].pos);
    }

    const std::string& GetBorderName(unsigned from, unsigned to)
    {
        return GetBorderName(nodes[from].pos, nodes[to].pos);
    }

    // dobiera scheme taki, do ktorego "centrum" jest najblizej
    const std::string& GetBestScheme(const sf::Vector2f& point)
    {
        assert(schemes.size() && "no schemes specified");

        float min = Maths::LengthSQ(schemes[0].center - point);
        size_t minIdx = 0;

        for (size_t scheme = 1; scheme < schemes.size(); ++scheme)
        {
            float curr = Maths::LengthSQ(schemes[scheme].center - point);
            if (curr < min)
            {
                min = curr;
                minIdx = scheme;
            }
        }

        return schemes[minIdx].name;
    }

    void CollisionMarkNode(const sf::Vector2f& node)
    {
        for (int i = -1; i < 2; ++i)
            for (int j = -1; j < 2; ++j)
                collisionMap[(int)node.x * 2 + i][(int)node.y * 2 + j] = 1;
    }

    void CollisionMarkEdge(const sf::Vector2f& first, const sf::Vector2f& second)
    {
        sf::Vector2f at = first * 2.f;
        sf::Vector2f step = (second - first) * 2.f;
        float len = Maths::Length(step);
        len *= 2;
        step /= len;
        unsigned steps = (unsigned)len;

        for (unsigned i = 0; i < steps; ++i)
        {
            collisionMap[(unsigned)at.x][(unsigned)at.y] = 1;
            at += step;
        }
    }

    bool CollisionCheck(const sf::Vector2f& first, const sf::Vector2f& second)
    {
        sf::Vector2f at = first * 2.f;
        sf::Vector2f step = (second - first) * 2.f;
        float len = Maths::Length(step) * 2.f;
        step /= len;
        at += step * 4.f;
        unsigned steps = (unsigned)len - 8;

        for (unsigned i = 0; i < steps; ++i)
        {
            if (collisionMap[(unsigned)at.x][(unsigned)at.y])
                return true;

            at += step;
        }

        return false;
    }

    bool GenerateNodes(unsigned numNodes, float borderSize)
    {
        nodes.clear();

        // pomocnicza zmienna, przydatna przy losowaniu wspolrzednych
        unsigned moduloFactor = (101 - (unsigned)borderSize * 2);

        // minimalna odleglosc miedzy wezlami, w %
        float minDist = (200.f - borderSize * 2.f) / (float)numNodes;
        float minDistSq = minDist * minDist;

        // po ilu nieudanych probach wylosowania wspolrzednych przerywamy i przestajemy dodawac kolejne wezly
        unsigned maxRandFails = 1000;

        nodes.reserve(numNodes);

        // generowanie pozycji wezlow
        for (size_t i = 0; i < numNodes; ++i)
        {
            sf::Vector2f pos;
            bool tooClose;
            unsigned guard = 0;

            do {
                tooClose = false;

                pos = sf::Vector2f(borderSize + (float)(rand() % moduloFactor), borderSize + (float)(rand() % moduloFactor));

                // sprawdzanie odleglosci od innych wezlow
                for (size_t node = 0; node < nodes.size(); ++node)
                    if (Maths::LengthSQ(nodes[node].pos - pos) < minDistSq)
                    {
                        tooClose = true;
                        break;
                    }

                if (++guard > maxRandFails)
                    // argh, nie mamy szczescia
                    goto too_many_rand_fails;

            } while (tooClose);

            nodes.push_back(pos);
            CollisionMarkNode(pos);
        }

        return true;
        
        // jesli po 1000 probach nie udalo sie wylosowac nowego punktu, ktory jest wystarczajaco daleko od reszty, ladujemy tu
    too_many_rand_fails:
        return false;
    }

    bool GenerateMinimalSpanningTreePrim()
    {
        // algorytm Prima (minimalne drzewo rozpinajace)
        
        std::vector<Edge> edges;
        
        // odleglosci, przeliczane wczesniej
        std::vector<std::vector<float> > distances;
        distances.resize(nodes.size());
        for (size_t i = 0; i < distances.size(); ++i)
            distances[i].resize(nodes.size());

        for (size_t i = 0; i < nodes.size(); ++i)
            for (size_t j = i + 1; j < nodes.size(); ++j)
                distances[j][i] = distances[i][j] = -Maths::LengthSQ(nodes[i].pos - nodes[j].pos);

        // wlasciwe polaczenia
        std::vector<size_t> mst;
        mst.reserve(nodes.size());
        mst.push_back(0);

        std::priority_queue<Edge, std::vector<Edge>, EdgeWeightLess> reachable;
        for (size_t i = 1; i < nodes.size(); ++i)
            reachable.push(Edge(distances[0][i], 0, i));

        for (size_t node = 1; node < nodes.size(); ++node)
        {
            // znajdz element o najmniejszej wadze, ktory nie nalezy do mst
            while ((std::find(mst.begin(), mst.end(), reachable.top().to) != mst.end()) ||
                   // i taki, ze da sie poprowadzic krawedz tak jak trzeba - na 1 scianie 1 przejscie
                   (nodes[reachable.top().from].BorderBlocked(
                        GetBorderIndex(reachable.top().from, reachable.top().to))) ||
                   (nodes[reachable.top().to].BorderBlocked(
                        GetBorderIndex(reachable.top().to, reachable.top().from))))
                reachable.pop();

            if (reachable.empty())
            {
                assert("world generator: couldn't find valid node with free border");
                return false;
            }

            Edge edge = reachable.top();
            // dodaj go do mst
            mst.push_back(edge.to);
            reachable.pop();

            // dodaj krawedz
            Node& from = nodes[edge.from], & to = nodes[edge.to];
            from.edges.push_back(edge.to);
            to.edges.push_back(edge.from);

            // 'zablokuj' zajete sciany
            from.BlockBorder(GetBorderIndex(from.pos, to.pos));
            to.BlockBorder(GetBorderIndex(to.pos, from.pos));

            // tu olewam sprawdzanie kolizji, bo MST nie ma szans powodowac przeciec
            CollisionMarkEdge(from.pos, to.pos);

            // dodaj do kolejki jego sasiadow
            for (size_t i = 0; i < nodes.size(); ++i)
                reachable.push(Edge(distances[i][mst[node]], edge.to, i));
        }

        return true;
    }

    bool GenerateRedundantEdges()
    {
        unsigned maxAttempts = nodes.size() + 25 + rand() % 25;
        // modyfikacja algorytmu Prima
        std::vector<Edge> edges;
        
        std::priority_queue<Edge, std::vector<Edge>, EdgeWeightLess> reachable;
        for (size_t i = 0; i < nodes.size(); ++i)
            for (size_t j = i + 1; j < nodes.size(); ++j)
                reachable.push(Edge(-Maths::LengthSQ(nodes[i].pos - nodes[j].pos), i, j));

        for (unsigned i = 0; i < maxAttempts; ++i)
        {
            if (reachable.empty())
                return false;

            Edge edge = reachable.top();
            reachable.pop();

            // jesli sciany sa zajete, trudno - proba nieudana
            if ((nodes[edge.from].BorderBlocked(GetBorderIndex(edge.from, edge.to))) ||
               (nodes[edge.to].BorderBlocked(GetBorderIndex(edge.to, edge.from))))
                continue;

            Node& from = nodes[edge.from], & to = nodes[edge.to];

            if (!CollisionCheck(from.pos, to.pos))
            {
                // dodaj krawedz
                from.edges.push_back(edge.to);
                to.edges.push_back(edge.from);

                // 'zablokuj' zajete sciany
                from.BlockBorder(GetBorderIndex(from.pos, to.pos));
                to.BlockBorder(GetBorderIndex(to.pos, from.pos));

                CollisionMarkEdge(from.pos, to.pos);
            }
        }

        return true;
    }

    bool GenerateSchemes(float borderSize)
    {
        schemes.clear();

        // pomocnicza zmienna, przydatna przy losowaniu wspolrzednych
        unsigned moduloFactor = (101 - (unsigned)borderSize * 2);

        std::string schemeNames[] = {
            "forest", "desert", "arctic"
        };
        unsigned numSchemes = sizeof(schemeNames) / sizeof(schemeNames[0]);
        schemes.reserve(numSchemes);

        // generujemy pare 'punktow centralnych'
        for (unsigned i = 0; i < numSchemes; ++i)
        {
            sf::Vector2f pos(borderSize + (float)(rand() % moduloFactor), borderSize + (float)(rand() % moduloFactor));
            schemes.push_back(Scheme(pos, schemeNames[i]));
        }

        return true;
    }

    bool ChooseCityPos()
    {
        assert(nodes.size() && "There should be at least 1 node!");

        // dobiera wezel o najwiekszej mozliwej liczbie polaczen
        // drugim kryterium jest odleglosc od srodka mapy
        unsigned best = 0;
        for (unsigned i = 1; i < nodes.size(); ++i)
            if (nodes[i].edges.size() > nodes[best].edges.size())
                best = i;
            else if (nodes[i].edges.size() == nodes[best].edges.size())
            {
                if (Maths::LengthSQ(nodes[i].pos - sf::Vector2f(50.f, 50.f)) < Maths::LengthSQ(nodes[best].pos - sf::Vector2f(50.f, 50.f)))
                    best = i;
            }

        nodes[best].isTown = true;
        return true;
    }

public:
    WorldGraphGenerator(float borderSize = 15.f): borderSize(borderSize) {}

    const std::vector<Node>& Generate(unsigned numNodes)
    {
        for (unsigned i = 0; i < 200; ++i)
            memset(collisionMap[i], 0, sizeof(collisionMap[0][0]) * 200);

        GenerateNodes(numNodes, borderSize);
        
        // ok, mamy wezly, teraz przydalyby sie jakies polaczenia
        GenerateMinimalSpanningTreePrim();
        // teraz powinno sie dac dojsc wszedzie z kazdego wezla

        // na wszelki wypadek: sprawdzanie poprawnosci
        for (size_t i = 0; i < nodes.size();)
            if (nodes[i].edges.size() == 0)
                // cholera.
                nodes.erase(nodes.begin() + i);
            else
                ++i;
        
        // jakies nadmiarowe krawedzie
        GenerateRedundantEdges();

        // generowanie scheme'ow
        GenerateSchemes(borderSize);
        // teraz przy przepisywaniu do CWorldGraph dla kazdego punktu znajdziemy scheme, ktory jest najblizej
        // (wg jakiejs tam funkcji, patrz GetBestScheme) i przypiszemy do tego

        for (size_t i = 0; i < nodes.size(); ++i)
            nodes[i].scheme = GetBestScheme(nodes[i].pos);

        // jeszcze wybierzmy pozycje miasta
        ChooseCityPos();

        return nodes;
    }
};

void CWorldGraph::Generate(unsigned numNodes)
{
    WorldGraphGenerator gen(15.f);
    const std::vector<WorldGraphGenerator::Node>& nodes = gen.Generate(numNodes);
    
    // przepisywanie wezlow do wlasciwego CWorldGraph
    maps.clear();

    std::vector<std::string> codes;
    codes.reserve(nodes.size());
    codes.push_back("a");
    for (size_t i = 1; i < nodes.size(); ++i)
        codes.push_back(StringUtils::GetNextCode_AZ(codes[i - 1]));

    // trzeba podmienic nazwy mapy 'town', wyciagamy jego id
    size_t townId = (size_t)-1;
    for (size_t i = 0; i < nodes.size(); ++i)
        if (nodes[i].isTown)
        {
            townId = i;
            break;
        }

    for (size_t node = 0; node < nodes.size(); ++node)
    {
        const WorldGraphGenerator::Node& n = nodes[node];

        CWorldGraphMap map;
        map.id = n.isTown ? "town" : codes[node];
        map.scheme = n.scheme;
        map.level = n.isTown ? CWorldGraphMap::MAP_LEVEL_CITY : 1;  // TODO
        map.boss = "";  // TODO
        map.final = false;  // TODO
        map.mapPos = n.pos;

        for (size_t edge = 0; edge < n.edges.size(); ++edge)
        {
            const WorldGraphGenerator::Node& dst = nodes[n.edges[edge]];

            CWorldGraphExit exit;
            exit.toMap = n.edges[edge] == townId ? "town" : codes[n.edges[edge]];
            exit.onBorder = WorldGraphGenerator::GetBorderName(n.pos, dst.pos);
            exit.toEntry = WorldGraphGenerator::GetBorderName(dst.pos, n.pos);
            exit.blocked = false; // TODO

            map.exits.push_back(exit);
        }

        maps.insert(std::make_pair(map.id, map));

        if (n.isTown)
        {
            startingMap = map.id;
            startingRegion = "north";
        }
    }
}
