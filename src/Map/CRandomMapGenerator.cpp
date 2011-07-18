#include "CRandomMapGenerator.h"

#include "CMap.h"

#include "../Logic/Loots/CLoot.h"
#include "../Logic/Items/CItem.h"

#include "../Logic/Factory/CLootTemplate.h"

#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"

#include "../Utils/CXml.h"
#include "../Utils/Maths.h"
#include "../Utils/CRand.h"
#include "../Utils/StringUtils.h"
#include "../Utils/FileUtils.h"

#include "../CGameOptions.h"

#include <fstream>
#include <string.h>
#include <list>

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Image.hpp>

#include <boost/filesystem.hpp>

// do pomiarow czasu, z auto-wypisywaniem po smierci ;)
class CTimer
{
    sf::Clock clock;
    std::string msg;
public:
    CTimer(const std::string& msg): msg(msg) { fprintf(stderr, "%s started\n", msg.c_str()); clock.Reset(); }
    ~CTimer() { fprintf(stderr, "%s %fs\n", msg.c_str(), clock.GetElapsedTime()); }
};

// operator do GenerateIntermediateTile
const sf::Color operator*(const sf::Color& col, float f)
{
    return sf::Color((sf::Uint8)(col.r * f), (sf::Uint8)(col.g * f), (sf::Uint8)(col.b * f), 255);
}

template<> CRandomMapGenerator* CSingleton<CRandomMapGenerator>::msSingleton = NULL;

// -----------------------------------

// to chyba bylo do std::sort, albo czegos takiego...
bool VectorCompareFunc(const CRandomMapGenerator::SPhysical& first, const CRandomMapGenerator::SPhysical& last)
{
    return (first.minLevel < last.minLevel);
}

// -----------------------------------

typedef std::vector<CRandomMapGenerator::SPhysical> PhysicalsVector;

PhysicalsVector FilterByType(const PhysicalsVector & input, const std::string & Type);

PhysicalsVector FilterByLevel(const PhysicalsVector & input, int level)
{
    PhysicalsVector filteredOut;
    for (PhysicalsVector::const_iterator it = input.begin() ; it != input.end() ; it++) {
        if (it->minLevel <= level && it->maxLevel >= level) filteredOut.push_back(*it);
    }
    return filteredOut;
}

CRandomMapGenerator::SPhysical ChooseRandomlyRegardingFrequency(const PhysicalsVector & input) {
    float totalFreq = 0.0f;
    for (PhysicalsVector::const_iterator it = input.begin() ; it != input.end() ; it++) {
        totalFreq += it->frequency;
    }
    float chosenFreq = gRand.Rndf(0.0f, totalFreq);
    for (PhysicalsVector::const_iterator it = input.begin() ; it != input.end() ; it++) {
        chosenFreq -= it->frequency;
        if (chosenFreq <= 0.0f) {
            return CRandomMapGenerator::SPhysical(*it);
        }
    }
    return CRandomMapGenerator::SPhysical();
}

std::string CRandomMapGenerator::GetRandomWeaponFile(int level) {
    PhysicalsVector weapons = FilterByLevel(FilterByType(mPhysicals, "weapon"), level);
    SPhysical weapon = ChooseRandomlyRegardingFrequency(weapons);
    return weapon.file;
}

// metody generowania tuneli
bool CRandomMapGenerator::GenerateTunnelsFromRandomCenter()
{
    // punkt, od ktorego zaczynamy kopac
    unsigned int origX = rand() % mDesc.sizeX, origY = rand() % mDesc.sizeY;
    // punkt, w ktorym kopiemy
    unsigned int nextX = origX, nextY = origY;
    // ile pol ma byc 'przejezdnych'?
    mPassableLeft = (unsigned int)(((100.f - mDesc.obstaclesAreaPercent) / 100.f) * (float)(mDesc.sizeX * mDesc.sizeY));

    for (unsigned int i = 0; i < mPassableLeft;)   // NIE blad, ma byc pusto po ostatnim ;
    {
        unsigned int direction = rand() % 4;

        nextX += (direction == 1 /* E */ ? 1 : (direction == 3 /* W */ ? -1 : 0));
        nextY += (direction == 0 /* N */ ? -1 : (direction == 2 /* S */ ? 1 : 0));

        // jesli doszlismy do skraju mapy, reset
        if (nextX >= mDesc.sizeX || nextY >= mDesc.sizeY)
        {
            nextX = origX;
            nextY = origY;
        }
        else if (mCurrent[nextX][nextY] == BLOCKED)
        {
            ++i;
            mCurrent[nextX][nextY] = FREE;
        }
    }

    mPassableLeft = mDesc.sizeX * mDesc.sizeY - mPassableLeft;

    return true;
}

bool CRandomMapGenerator::GenerateTunnelsGraph()
{
    // "szerokosc tunelu"
    unsigned int narrowTunnelSize = 2;
    unsigned int wideTunnelSize = 3;
    // ilosc wierzcholkow grafu - wieksze od 0 bo bedzie crash
    const unsigned int NUM_VERTS = 50;

    // losowanie wierzcholkow grafu
    // bool = flaga oznaczajaca, czy dany wierzcholek jest juz z czyms polaczony
    std::pair<sf::Vector2i, bool> verts[NUM_VERTS];
    for (size_t i = 0; i < NUM_VERTS; ++i)
        verts[i] = std::make_pair(sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY), false);

    // ile pol ma byc 'przejezdnych'?
    mPassableLeft = (unsigned int)(((100.f - mDesc.obstaclesAreaPercent) / 100.f) * (float)(mDesc.sizeX * mDesc.sizeY));

    unsigned int i = 0;
    for (i = 0; i < mPassableLeft;)   // NIE blad, ma byc pusto po ostatnim ;
    {
        // losowanie startu i konca wierzcholka
        size_t start;
        if (i == 0)
            start = rand() % NUM_VERTS;
        else    // upewniamy sie, ze tunel bedzie laczyl sie z jakims istniejacym
            do start = rand() % NUM_VERTS; while (!verts[start].second);
        size_t end = rand() % NUM_VERTS;

        verts[start].second = true;
        verts[end].second = true;

        // losowanie szerokosci tunelu
        unsigned int tunnelSize = ((float)rand() / RAND_MAX) < (mDesc.narrowPathsPercent / 100.f) ? narrowTunnelSize : wideTunnelSize;

        // kopu kopu
        i += GenerateStraightTunnel(verts[start].first, verts[end].first, tunnelSize);
    }
    
    mPassableLeft = mDesc.sizeX * mDesc.sizeY - i;

    return true;
}

bool CRandomMapGenerator::GenerateTunnelsBossArena()
{
    unsigned int tunnelSize = 3;
    unsigned int minDistFromMapEdge = 6;

    // -tunnelSize, zeby nie wyszedl tunel o szerokosci 1 w dolnym rogu
    sf::Vector2i entry(0, rand() % (mDesc.sizeY - tunnelSize - 2 * minDistFromMapEdge) + minDistFromMapEdge),                // lewa
                 exit(mDesc.sizeX - 1, rand() % (mDesc.sizeY - tunnelSize - 2 * minDistFromMapEdge) + minDistFromMapEdge);   // prawa

    // na ostatnim bossie nie ma wyjscia z mapy, ustawiamy exit na srodek
    if (mDesc.mapType == SRandomMapDesc::MAP_FINAL_BOSS)
        exit = sf::Vector2i(mDesc.sizeX / 2, mDesc.sizeY / 2);

    // zapewniamy polaczenie miedzy tymi dwoma
    mPassableLeft = mDesc.sizeX * mDesc.sizeY - GenerateStraightTunnel(entry, exit, tunnelSize);

    // wlasciwa arena
    // http://roguebasin.roguelikedevelopment.org/index.php?title=Irregular_Shaped_Rooms
    unsigned int borderSize = 5;    // szerokosc "ramki", w ktorej nie beda drazone tunele
    unsigned int rectSize = 5;      // szerokosc prostokata - patrz link w komentarzu wyzej
    // UWAGA: niech tunnelSize + minDistFromMapEdge >= borderSize + rectSize, bo sciezka moze sie nie przeciac z arena

    mPassableLeft -= GenerateIrregularCave(sf::IntRect(borderSize, borderSize, mDesc.sizeX - borderSize, mDesc.sizeY - borderSize), rectSize);

    return true;
}


// generowanie pojedynczego prostego tunelu miedzy dwoma punktami - zwraca ilosc wykopanych pol
unsigned int CRandomMapGenerator::GenerateStraightTunnel(const sf::Vector2i& from, const sf::Vector2i& to, unsigned int tunnelSize)
{
    unsigned int fieldsFreed = 0;

    // wybieramy deltaMax punktow na odcinku pomiedzy start i end
    // i kopiemy przejscie w kazdym punkcie (+ 3 pola przylegle)
    // pojedynczy krok
    sf::Vector2i deltaInt(to - from);
    sf::Vector2f delta((float)(deltaInt.x), (float)(deltaInt.y));
    // ile punktow bedzie sprawdzanych, wiecej = lepiej
    int deltaMax = std::max(abs(deltaInt.x), abs(deltaInt.y)) * 2;
    delta /= (float)deltaMax;

    // kopanie tunelu od startu do konca
    sf::Vector2f at((float)from.x, (float)from.y);
    for (int j = 0; j < deltaMax; ++j)
    {
        at += delta;
        unsigned int atX = (unsigned int)at.x, atY = (unsigned int) at.y;

        if (atX >= mDesc.sizeX || atY >= mDesc.sizeY)
            break;

        // kopanie + poszerzanie
        for (unsigned int x = 0; x < tunnelSize; ++x)
            if (atX + x < mDesc.sizeX)
                for (unsigned int y = 0; y < tunnelSize; ++y)
                    if (atY + y < mDesc.sizeY)
                        if (mCurrent[atX + x][atY + y] == BLOCKED)
                        {
                            mCurrent[atX + x][atY + y] = FREE;
                            ++fieldsFreed;
                        }
    }

    return fieldsFreed;
}

// pomocnicze funkcje, uzywane w GenerateIrregularCave
bool VectorXLess(sf::Vector2i a, sf::Vector2i b) { return a.x < b.x; }
bool VectorXGreater(sf::Vector2i a, sf::Vector2i b) { return a.x > b.x; }
bool VectorYLess(sf::Vector2i a, sf::Vector2i b) { return a.y < b.y; }
bool VectorYGreater(sf::Vector2i a, sf::Vector2i b) { return a.y > b.y; }

// generowanie "jaskini" o nieregularnych ksztaltach (wielokat); rectSize - "grubosc" prostokatow, w ktorych moga znalezc sie wierzcholki
// patrz: http://roguebasin.roguelikedevelopment.org/index.php?title=Irregular_Shaped_Rooms
unsigned int CRandomMapGenerator::GenerateIrregularCave(const sf::IntRect& outsideRect, unsigned int rectSize)
{
    unsigned int fieldsFreed = 0;

    unsigned int minVertsInRect = 2;
    unsigned int maxVertsInRect = 6;

    std::vector<sf::Vector2i> points; // lista wierzcholkow wielokata
    std::vector<sf::Vector2i>::iterator it;
    points.reserve(maxVertsInRect * 4 + 1);

    unsigned int rectWidth = outsideRect.GetWidth() - 2 * rectSize;     // szerokosc gornego/dolnego prostokata
    unsigned int rectHeight = outsideRect.GetHeight() - 2 * rectSize;   // wysokosc lewego/prawego prostokata

    // gorny prostokat
    unsigned int vertsInRect = rand() % (maxVertsInRect - minVertsInRect) + minVertsInRect;
    for (unsigned int i = 0; i < vertsInRect; ++i)
        points.push_back(sf::Vector2i(rand() % rectWidth + outsideRect.Left + rectSize, rand() % rectSize + outsideRect.Top));

    // sortowanie po x, rosnaco
    it = points.begin();
    std::sort<std::vector<sf::Vector2i>::iterator>(it, points.end(), VectorXLess);
    it = --points.end();

    // prawy prostokat
    vertsInRect = rand() % (maxVertsInRect - minVertsInRect) + minVertsInRect;
    for (unsigned int i = 0; i < vertsInRect; ++i)
        points.push_back(sf::Vector2i(rand() % rectSize + outsideRect.Right - rectSize, rand() % rectHeight + outsideRect.Top + rectSize));

    // sortowanie po y, rosnaco
    ++it;
    std::sort<std::vector<sf::Vector2i>::iterator>(it, points.end(), VectorYLess);
    it = --points.end();

    // dolny prostokat
    vertsInRect = rand() % (maxVertsInRect - minVertsInRect) + minVertsInRect;
    for (unsigned int i = 0; i < vertsInRect; ++i)
        points.push_back(sf::Vector2i(rand() % rectWidth + outsideRect.Left + rectSize, rand() % rectSize + outsideRect.Bottom - rectSize));

    // sortowanie po x, malejaco
    ++it;
    std::sort<std::vector<sf::Vector2i>::iterator>(it, points.end(), VectorXGreater);
    it = --points.end();

    // lewy prostokat
    vertsInRect = rand() % (maxVertsInRect - minVertsInRect) + minVertsInRect;
    for (unsigned int i = 0; i < vertsInRect; ++i)
        points.push_back(sf::Vector2i(rand() % rectSize + outsideRect.Left, rand() % rectHeight + outsideRect.Top + rectSize));

    // sortowanie po y, malejaco
    ++it;
    std::sort<std::vector<sf::Vector2i>::iterator>(it, points.end(), VectorYGreater);

    for (int y = outsideRect.Top; y < outsideRect.Bottom; ++y)
        for (int x = outsideRect.Left; x < outsideRect.Right; ++x)
        {
            unsigned int i, j;
            bool c = false;

            for (i = 0, j = points.size() - 1; i < points.size(); j = i++)
                if (((points[i].y > y) != (points[j].y > y)) &&
                    (x < (points[j].x - points[i].x) * (y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
                    c = !c;

            if (c && mCurrent[x][y] == BLOCKED)
            {
                mCurrent[x][y] = FREE;
                ++fieldsFreed;
            }
        }

    return fieldsFreed;
}


// BFS
unsigned int CRandomMapGenerator::DistanceDijkstra(sf::Vector2i start, sf::Vector2i end)
{
    unsigned int dist = 0;
    std::list<sf::Vector2i> current;
    current.push_back(start);

    // tablica na juz sprawdzone wierzcholki..
    unsigned int** checked = new unsigned int*[mDesc.sizeX];
    for (unsigned int i = 0; i < mDesc.sizeX; ++i)
    {
        checked[i] = new unsigned int[mDesc.sizeY];
        memset(checked[i], 0, mDesc.sizeY * sizeof(unsigned int));
    }
    checked[start.x][start.y] = 1;

    // jesli dist == sizeX * sizeY, to nie ma zadnej drogi
    while (dist < mDesc.sizeX * mDesc.sizeY && current.size())
    {
        std::list<sf::Vector2i> next;

        for (std::list<sf::Vector2i>::iterator it = current.begin(); it != current.end(); ++it)
        {
            // koniec w tym punkcie?
            if (*it == end)
            {
                for (unsigned int i = 0; i < mDesc.sizeX; ++i)
                    delete[] checked[i];
                delete[] checked;
                return dist;
            }

            // sprawdzamy wszystkie przylegle pola
            if ((it->x > 0) && (mCurrent[it->x - 1][it->y] != BLOCKED) && (!checked[it->x - 1][it->y]))
            {
                checked[it->x - 1][it->y] = 1;
                next.push_back(sf::Vector2i(it->x - 1, it->y));
            }
            if ((it->x + 1 < (int)mDesc.sizeX) && (mCurrent[it->x + 1][it->y] != BLOCKED) && (!checked[it->x + 1][it->y]))
            {
                checked[it->x + 1][it->y] = 1;
                next.push_back(sf::Vector2i(it->x + 1, it->y));
            }
            if ((it->y > 0) && (mCurrent[it->x][it->y - 1] != BLOCKED) && (!checked[it->x][it->y - 1]))
            {
                checked[it->x][it->y - 1] = 1;
                next.push_back(sf::Vector2i(it->x, it->y - 1));
            }
            if ((it->y + 1 < (int)mDesc.sizeY) && (mCurrent[it->x][it->y + 1] != BLOCKED) && (!checked[it->x][it->y + 1]))
            {
                checked[it->x][it->y + 1] = 1;
                next.push_back(sf::Vector2i(it->x, it->y + 1));
            }
        }

        current = next;
        ++dist;
    }

    // nie znalazlo :(
    for (unsigned int i = 0; i < mDesc.sizeX; ++i)
        delete[] checked[i];
    delete[] checked;
    return (unsigned int)-1;
}


// generowanie posrednich kafli
bool CRandomMapGenerator::GenerateIntermediateTile(const std::string& outFile, const std::string& topLeft, const std::string& topRight, const std::string& bottomLeft, const std::string& bottomRight, unsigned int tileMask)
{
    // moze juz istnieje?
    if (FileUtils::FileExists(outFile.c_str()))
        return true;

    // nie ma prawa wystapic sytuacja, kiedy generujemy posrednie kafle z
    // wczesniej wygenerowanych, bo inaczej wszystkie beda sie robic do 2012
    if (topLeft.find(mTilesFolder) != std::string::npos ||
        topRight.find(mTilesFolder) != std::string::npos ||
        bottomLeft.find(mTilesFolder) != std::string::npos ||
        bottomRight.find(mTilesFolder) != std::string::npos)
    {
        fprintf(stderr, "CRandomMapGenerator::GenerateIntermediateTile: OMG FATAL ERROR (trying to generate from generated)!\n");
        return false;
    }

    sf::Image* tl = gResourceManager.GetImage(topLeft);
    sf::Image* tr = gResourceManager.GetImage(topRight);
    sf::Image* bl = gResourceManager.GetImage(bottomLeft);
    sf::Image* br = gResourceManager.GetImage(bottomRight);

    if (!tl->GetWidth())
    {
        fprintf(stderr, "ERROR: Tile image not loaded: %s\n", topLeft.c_str());
        return false;
    }
    if (!tr->GetWidth())
    {
        fprintf(stderr, "ERROR: Tile image not loaded: %s\n", topRight.c_str());
        return false;
    }
    if (!bl->GetWidth())
    {
        fprintf(stderr, "ERROR: Tile image not loaded: %s\n", bottomLeft.c_str());
        return false;
    }
    if (!br->GetWidth())
    {
        fprintf(stderr, "ERROR: Tile image not loaded: %s\n", bottomRight.c_str());
        return false;
    }

    if (tl == tr && tr == bl && bl == br)
    {
        // wszystkie rogi te same? super!
        tl->SaveToFile(outFile);
        return true;
    }

    // do tego obrazka generujemy
    sf::Image generated((unsigned int)Map::TILE_SIZE, (unsigned int)Map::TILE_SIZE);
    sf::Image mask; // maska ma miec 64x64px! (Map::TILE_SIZE)

    // jesli mamy maske, uzywamy. jesli nie ma, robimy brzydkie przejscie
    if (mTileMasks.size() > tileMask)
        mask.LoadFromFile(mTileMasks[tileMask]);

    for (int y = 0; y < Map::TILE_SIZE; ++y)
        for (int x = 0; x < Map::TILE_SIZE; ++x)
        {
            float tileSizef = (float)Map::TILE_SIZE;

            float tlf, trf, blf, brf;
            // ile % koloru z danego naroznika ma miec piksel?
            if (mask.GetWidth() != 0)
            {
                // uzywamy maski
                sf::Color maskCol = mask.GetPixel(x, y);
                tlf = (float)maskCol.r / 255.f;
                trf = (float)maskCol.g / 255.f;
                blf = (float)maskCol.b / 255.f;
                brf = 1.f - ((float)maskCol.a / 255.f); // 1-, bo im bardziej przezroczyste, tym wiecej kafla w kaflu

                // dla ulatwienia tworzenia masek: alpha ma wiekszy priorytet
                tlf = Maths::Clamp(tlf - brf);
                trf = Maths::Clamp(trf - brf);
                blf = Maths::Clamp(blf - brf);
            }
            else
            {
                // brzydkie przejscie
                tlf = (1.f - (x / tileSizef)) * (1 - (y / tileSizef));
                trf = (x / tileSizef) * (1 - (y / tileSizef));
                blf = (1.f - (x / tileSizef)) * (y / tileSizef);
                brf = (x / tileSizef) * (y / tileSizef);
            }

            // dla kazdego obrazka osobne wspolrzedne, bo kazdy moze miec inna wielkosc
            unsigned int tlw = (unsigned int)((float)x * ((float)tl->GetWidth() / tileSizef)),
                tlh = (unsigned int)((float)y * ((float)tl->GetHeight() / tileSizef));
            unsigned int trw = (unsigned int)((float)x * ((float)tr->GetWidth() / tileSizef)),
                trh = (unsigned int)((float)y * ((float)tr->GetHeight() / tileSizef));
            unsigned int blw = (unsigned int)((float)x * ((float)bl->GetWidth() / tileSizef)),
                blh = (unsigned int)((float)y * ((float)bl->GetHeight() / tileSizef));
            unsigned int brw = (unsigned int)((float)x * ((float)br->GetWidth() / tileSizef)),
                brh = (unsigned int)((float)y * ((float)br->GetHeight() / tileSizef));

            sf::Color pixel = tl->GetPixel(tlw, tlh) * tlf +
                tr->GetPixel(trw, trh) * trf +
                bl->GetPixel(blw, blh) * blf +
                br->GetPixel(brw, brh) * brf;

            generated.SetPixel(x, y, pixel);
        }

    generated.SaveToFile(outFile);
    
    return true; 
}


// samo generowanie pol pustych/nie do przejscia
// stawianie invisible-walli: PlaceWalls()
bool CRandomMapGenerator::GenerateMap()
{
    CTimer timer("- map (tunnels): ");

    // upewniamy sie, ze % jest poprawny
    mDesc.obstaclesAreaPercent = Maths::Clamp(mDesc.obstaclesAreaPercent, 0.f, 100.f);
    mDesc.narrowPathsPercent = Maths::Clamp(mDesc.narrowPathsPercent, 0.f, 100.f);

    switch (mDesc.mapType)
    {
    case SRandomMapDesc::MAP_BOSS:
    case SRandomMapDesc::MAP_FINAL_BOSS:
        return GenerateTunnelsBossArena();
    default:
        return GenerateTunnelsGraph();
    }
}

bool CRandomMapGenerator::PlaceTiles()
{
    CTimer timer("- tiles: ");

    if (mPartSets.find(mDesc.set) == mPartSets.end())
    {
        fprintf(stderr, "CRandomMapGenerator::PlaceTiles: map set \"%s\" does not exist!\n", mDesc.set.c_str());
        return false;
    }

    // jakie kafle na jakich polach
    unsigned int** tiles = new unsigned int*[mDesc.sizeX];
    for (unsigned int i = 0; i < mDesc.sizeX; ++i)
    {
        tiles[i] = new unsigned int[mDesc.sizeY];
        memset(tiles[i], (unsigned int)-1, mDesc.sizeY * sizeof(unsigned int));
    }

    // dla wygody..
    SPartSet& set = mPartSets[mDesc.set];

    if (set.tiles.size() == 0)
    {
        fprintf(stderr, "ERROR: no tiles in set \"%s\" (PlaceTiles)\n", mDesc.set.c_str());
        return false;
    }

    // losujemy rogi
    unsigned int** corners = new unsigned int*[mDesc.sizeX + 1];
    for (unsigned int i = 0; i < mDesc.sizeX + 1; ++i)
    {
        corners[i] = new unsigned int[mDesc.sizeY + 1];
        for (unsigned int j = 0; j < mDesc.sizeY + 1; ++j)
            corners[i][j] = rand() % set.tiles.size();
    }

    // losujemy co 4 kafel, reszte bedziemy dobierac wg sasiadow
    // [*][ ][*]
    // [ ][ ][ ]
    // [*][ ][*]
    // [*] - kafel losowany, reszta to "posrednie"
    for (unsigned int x = 0; x < mDesc.sizeX; x += 2)
        for (unsigned int y = 0; y < mDesc.sizeY; y += 2)
            tiles[x][y] = rand() % set.tiles.size();

    // mapa na dobrane kafle, zeby nie duplikowac
    std::map<std::string, unsigned int> generatedTiles;
    // i wektor, bo jak sie doda do set.tiles to drugie generowanie jest o dupe rozbic
    std::vector<std::string> tilePaths = set.tiles;

    // tu dobieramy reszte tak, zeby pasowaly
    for (unsigned int x = 0; x < mDesc.sizeX; ++x)
        for (unsigned int y = 0; y < mDesc.sizeY; ++y)
        {
            std::string nameTopLeft, nameTopRight, nameBottomLeft, nameBottomRight;

            nameTopLeft = set.tiles[corners[x][y]];
            nameTopRight = set.tiles[corners[x + 1][y]];
            nameBottomLeft = set.tiles[corners[x][y+1]];
            nameBottomRight = set.tiles[corners[x+1][y+1]];

            // losowanie maski
            unsigned int tileMask = (mTileMasks.size() ? (unsigned int)(rand()) % mTileMasks.size() : (unsigned int)-1);

            // IOCCC mode off

            if (!boost::filesystem::exists(mTilesFolder))
                boost::filesystem::create_directory(mTilesFolder);
            
            // nazwa przejsciowego kafla
            boost::uint64_t hash = StringUtils::GetStringHash(nameTopLeft + nameTopRight + nameBottomLeft + nameBottomRight);
            std::string imgName = mTilesFolder + "/" +
                StringUtils::ToString(hash) + "_" +
                StringUtils::ToString(tileMask) + ".png";


            // jesli nie wygenerowalismy takiego kafla, to to robimy
            if (generatedTiles.find(imgName) == generatedTiles.end())
            {
                if (!GenerateIntermediateTile(imgName, nameTopLeft, nameTopRight, nameBottomLeft, nameBottomRight, tileMask))
                {
                    fprintf(stderr, "Error: generating intermediate tile from files:\n- %s\n- %s\n- %s\n- %s\nfailed!\n", nameTopLeft.c_str(), nameTopRight.c_str(), nameBottomLeft.c_str(), nameBottomRight.c_str());
                    return false;
                }

                tiles[x][y] = tilePaths.size();
                generatedTiles.insert(std::make_pair(imgName, tilePaths.size()));
                tilePaths.push_back(imgName);
            }
            else
                tiles[x][y] = generatedTiles[imgName];
        }

    // generowanie kodow kafli
    std::vector<std::string> tileCodes;
    for (size_t i = 0; i < tilePaths.size(); ++i)
    {
        std::string code;
        size_t tmp = i;

        do {
            size_t c = tmp % ('z' - 'a');
            code += ('a' + c);
            tmp /= ('z' - 'a');
        } while (tmp);

        tileCodes.push_back(code);
    }

    // typy kafli
    for (size_t i = 0; i < tilePaths.size(); ++i)
	    mXmlText << "\t<tiletype code=\"" << tileCodes[i] << "\" image=\"" << tilePaths[i] << "\"/>\n";
    
    // uklad kafli na mapie
    mXmlText << "\t<tiles>\n";
    for (unsigned int y = 0; y < mDesc.sizeY; ++y)
    {
        mXmlText << "\t";
        for (unsigned int x = 0; x < mDesc.sizeX; ++x)
            mXmlText << tileCodes[tiles[x][y]] << " ";
        mXmlText << "\n";
    }
    mXmlText << "\t</tiles>\n";

    
    for (unsigned int i = 0; i < mDesc.sizeX; ++i)
        delete[] tiles[i];
    delete[] tiles;

    return true;
}

bool CRandomMapGenerator::PlaceWalls()
{
    CTimer timer("- walls: ");

    // invisible-walls
    mXmlText << "\t<objtype code=\"iw\" file=\"data/physicals/walls/invisible-wall.xml\" />\n";

    for (unsigned int y = 0; y < mDesc.sizeY; ++y)
        for (unsigned int x = 0; x < mDesc.sizeX; ++x)
            if (mCurrent[x][y] == BLOCKED)  // jesli to pole jest zablokowane..
            {
                if ((x > 0 && mCurrent[x - 1][y] != BLOCKED) ||                 // a to po lewej nie..
                    (x < mDesc.sizeX - 1 && mCurrent[x + 1][y] != BLOCKED) ||   // lub to po prawej nie..
                    (y > 0 && mCurrent[x][y - 1] != BLOCKED) ||                 // lub to powyzej nie..
                    (y < mDesc.sizeY - 1 && mCurrent[x][y + 1] != BLOCKED))     // lub to ponizej nie..
                    // to postaw sciane, bo jestesmy na skraju
                    mXmlText << "\t<obj code=\"iw\" x=\"" << (float)(x) + 0.5f << "\" y=\"" << (float)(y) + 0.5f << "\" />\n";
            }

    return true;
}

bool CRandomMapGenerator::PlaceRegions()
{
    CTimer timer("- regions: ");

    // ostatni boss ma wejscie, ale wyjscia juz nie
    unsigned int regionsToPlace = (mDesc.mapType == SRandomMapDesc::MAP_FINAL_BOSS ? 1 : 2);
    
    // za malo miejsca, zeby postawic chociazby region wejscia (i ew. wyjscia)..
    if (mPassableLeft < regionsToPlace) return false;
    
    sf::Vector2i entry, exit;
    unsigned int first, last;   // uzywane przy wyznaczaniu pozycji wejscia.wyjscia na mapach z bossami

    switch (mDesc.mapType)
    {
    case SRandomMapDesc::MAP_BOSS:
        for (first = 0; first < mDesc.sizeY && mCurrent[mDesc.sizeX - 1][first] == BLOCKED; ++first);     // poczatek wyjscia
        for (last = first; last < mDesc.sizeY && mCurrent[mDesc.sizeX - 1][last] == FREE; ++last);        // koniec wyjscia

        if (first == mDesc.sizeY && last == mDesc.sizeY) // nie ma wyjscia
            return false;

        exit = sf::Vector2i(mDesc.sizeX - 1, (last + first) / 2);
        // bez break, bo 'zwykly' boss potrzebuje wejscia i wyjscia, 'finalowy' - tylko wejscia

    case SRandomMapDesc::MAP_FINAL_BOSS:
        for (first = 0; first < mDesc.sizeY && mCurrent[0][first] == BLOCKED; ++first);     // poczatek wejscia
        for (last = first; last < mDesc.sizeY && mCurrent[0][last] == FREE; ++last);        // koniec wejscia

        if (first == mDesc.sizeY && last == mDesc.sizeY) // nie ma wejscia
            return false;
    
        entry = sf::Vector2i(0, (last + first) / 2);    // srodek tunelu
        break;

    default:
        {
            // jesli to nie mapa z bossem, to regiony moga byc gdzie im sie podoba
            unsigned int dist = 0;

            for (size_t i = 0; i < 100; ++i)
            {
                sf::Vector2i newEntry, newExit;
                // do skutku, az bedzie na "przejezdnym"
                do
                    newEntry = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
                while (mCurrent[newEntry.x][newEntry.y] != FREE);
                do
                    newExit = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
                while (mCurrent[newExit.x][newExit.y] != FREE);

                // odslaniamy okolice wyjscia
                for (int x = std::max<int>(newExit.x - 1, 0); x < std::min<int>(newExit.x + 2, mDesc.sizeX); ++x)
                    for (int y = std::max<int>(newExit.y - 1, 0); y < std::min<int>(newExit.y + 2, mDesc.sizeY); ++y)
                        if (!mCurrent[x][y]) // == BLOCKED?
                        {
                            mCurrent[x][y] = FREE;
                            ++mPassableLeft;
                        }

                // wybieramy najdluzsza droge
                unsigned int newDist = DistanceDijkstra(newEntry, newExit);
                if (newDist != (unsigned int)-1 && newDist > dist)
                {
                    mEntryPos = entry = newEntry;
                    exit = newExit;
                    dist = newDist;
                }
            }
        }
    }

    // wejscie
    mXmlText << "\t<region name=\"entry\" x=\"" << entry.x + 0.5f << "\" y=\"" << entry.y + 0.5f << "\" rot=\"0\" scale=\"1\"></region>\n";
    mCurrent[entry.x][entry.y] = REGION;
    
    if (mDesc.mapType != SRandomMapDesc::MAP_FINAL_BOSS) {
        // wyjscie
        mXmlText << "\t<region name=\"exit\" x=\"" << exit.x + 0.5f << "\" y=\"" << exit.y + 0.5f << "\" rot=\"0\" scale=\"1\">\n"
            << "\t\t<next-map>" << mDesc.nextMap << "</next-map>\n"
    //            << "\t\t<next-map>data/maps/level01.xml</next-map>\n"
    //            << "\t\t<next-map-region>entry</next-map-region>\n"
            << "\t</region>\n"
            // tlo pod wyjscie - portal
            << "\t<objtype code=\"exit-portal\" file=\"data/physicals/walls/test-barrier.xml\" />\n"
            << "\t<obj code=\"exit-portal\" x=\"" << exit.x + 0.5f << "\" y=\"" << exit.y + 0.5f << "\" />\n";
        mCurrent[exit.x][exit.y] = REGION;
    }

    // regiony "zapychaja" pola
    mPassableLeft -= regionsToPlace;

    return true;
}

bool CRandomMapGenerator::PlaceDoodahs()
{
    CTimer timer("- doodahs: ");

    if (mPartSets.find(mDesc.set) == mPartSets.end())
    {
        fprintf(stderr, "CRandomMapGenerator::PlaceDoodahs: map set \"%s\" does not exist!\n", mDesc.set.c_str());
        return false;
    }

    // dla wygody..
    SPartSet& set = mPartSets[mDesc.set];

    if (set.doodahs.size() == 0)
    {
        fprintf(stderr, "ERROR: no doodahs in set \"%s\" (PlaceDoodahs)\n", mDesc.set.c_str());
        return false;
    }
    
    // doodahy - 1 na kafla
    for (unsigned int y = 0; y < mDesc.sizeY; ++y)
        for (unsigned int x = 0; x < mDesc.sizeX; ++x)
        {
            if (mCurrent[x][y] == BLOCKED)
            {
                size_t doodahNum = rand() % set.doodahs.size();             // ktory doodah?
                float offsetX = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // offsety, zeby nie staly tak bardzo jednolicie
                float offsetY = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // + 0.5, zeby wycentrowac na kaflach
                float scale = ((float)rand() / RAND_MAX + 3.3f) / 3.f;      // skala doodaha: 1.1 - ~1.4 (rand() zwraca signed int)
                int rot = rand() % 360;                                     // jeszcze obrot do tego

                if ((x > 0 && mCurrent[x - 1][y] != BLOCKED) ||
                    (x < mDesc.sizeX - 1 && mCurrent[x + 1][y] != BLOCKED) ||
                    (y > 0 && mCurrent[x][y - 1] != BLOCKED) ||
                    (y < mDesc.sizeY - 1 && mCurrent[x][y + 1] != BLOCKED))
                    scale = Maths::Clamp(scale, 0.f, 1.1f);

                scale *= set.doodahs[doodahNum].scale;

                mXmlText << "\t<sprite file=\"" << set.doodahs[doodahNum].file
                    << "\" x=\"" << (float)x + offsetX
                    << "\" y=\"" << (float)y + offsetY
                    << "\" scale=\"" << scale
                    << "\" rot=\"" << rot
                    << "\" z=\"" << (set.doodahs[doodahNum].isInForeground ? "foreground" : "background")
                    << "\" />\n";
            }
        }

    // doodahy pod nogami
    if (set.doodahsOnGround.size() > 0)
    {
		unsigned int doodahsCount = rand() % (set.maxDoodahsOnGround - set.minDoodahsOnGround) + set.minDoodahsOnGround; // troche doodahow

        for (unsigned int i = 0; i < doodahsCount; ++i)
        {
            sf::Vector2i pos;
            do {
                pos = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
            } while (mCurrent[pos.x][pos.y] != FREE);

            // copypasta :x
            size_t doodahNum = rand() % set.doodahsOnGround.size();     // ktory doodah?
            float offsetX = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // offsety, zeby nie staly tak bardzo jednolicie
            float offsetY = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // + 0.5, zeby wycentrowac na kaflach
            float scale = ((float)rand() / RAND_MAX + 3.3f) / 3.f - 0.4;// skala doodaha: 0.6 - ~1.0 (rand() zwraca signed int)
            int rot = rand() % 360;                                     // jeszcze obrot do tego

            if ((pos.x > 0 && mCurrent[pos.x - 1][pos.y] != BLOCKED) ||
                ((unsigned int)pos.x < mDesc.sizeX - 1 && mCurrent[pos.x + 1][pos.y] != BLOCKED) ||
                (pos.y > 0 && mCurrent[pos.x][pos.y - 1] != BLOCKED) ||
                ((unsigned int)pos.y < mDesc.sizeY - 1 && mCurrent[pos.x][pos.y + 1] != BLOCKED))
                scale = Maths::Clamp(scale, 0.f, 1.1f);

            scale *= set.doodahsOnGround[doodahNum].scale;

            mXmlText << "\t<sprite file=\"" << set.doodahsOnGround[doodahNum].file
                << "\" x=\"" << (float)pos.x + offsetX
                << "\" y=\"" << (float)pos.y + offsetY
                << "\" scale=\"" << scale
                << "\" rot=\"" << rot
                << "\" z=\"" << (set.doodahsOnGround[doodahNum].isInForeground ? "foreground" : "background")
                << "\" />\n";

            // dobrze, zeby na tym polu juz nic nie ladowalo
            mCurrent[pos.x][pos.y] = DOODAH;
        }
    }

    return true;
}

bool CRandomMapGenerator::PlaceBossDoors()
{
    CTimer timer("- boss doors: ");

    // finalowy boss nie ma wyjscia, mozna olac MAP_FINAL_BOSS
    if (mDesc.mapType != SRandomMapDesc::MAP_BOSS)
        return true;    // nie potrzeba drzwi

    PhysicalsVector doors = FilterByType(mPhysicals, "door");
    if (doors.empty())
        return false;   // buu, nie ma drzwi :(

    // wybieramy jeden rodzaj drzwi, bedzie uzyty pare razy...
    SPhysical door = doors[rand() % doors.size()];

    mXmlText << "\t<objtype code=\"door\" file=\"" << door.file << "\" />\n";
    
    unsigned int x = mDesc.sizeX - 3;   // stawiamy drzwi na wszystkich polach znajdujacych sie 3 kafle od prawej krawedzi

    for (unsigned int i = 0; i < mDesc.sizeY; ++i)
        if (mCurrent[x][i] != BLOCKED)
        {
            mXmlText << "\t<obj code=\"door\" x=\"" << (float)mDesc.sizeX - 2.5f
                     << "\" y=\"" << (float)i + 0.5f << "\">\n"
                     << "\t\t<cond check=\"once\">\n"
                     << "\t\t\t<type>killed</type>\n"
                     << "\t\t\t<param>boss</param>\n"   // zakladamy, ze boss ma id "boss"
                     << "\t\t</cond>\n"
                     << "\t\t<effect-on-open type=\"console\">\n"
                     << "\t\t\t<text>load-playlist data/music/testpl.xml</text>\n"
                     << "\t\t</effect-on-open>\n"
                     << "\t</obj>\n";

            mCurrent[x][i] = DOOR;
        }

    return true;
}

bool CRandomMapGenerator::PlaceLairs()
{
    CTimer timer("- lairs: ");

    PhysicalsVector lairs = FilterByLevel(FilterByType(mPhysicals, "lair"), mDesc.level);
    if (lairs.empty()) {
        return true;
    }

	std::map<std::string, int> mapPhysicalToObjTypeCode;
	int objTypesCount = 0;

    // gniazda
    if (lairs.size() && mDesc.lairs && mPassableLeft)
    {
        for (unsigned int i = 0; i < std::min(mPassableLeft, mDesc.lairs); ++i)
        {
            // znalezienie wolnego pola
            sf::Vector2i tile;
            do
                tile = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
            while (mCurrent[tile.x][tile.y] != FREE ||
                ((float)std::min(mDesc.sizeX, mDesc.sizeY) > mDesc.minMonsterDist * 2.f &&     // jesli mapa nie ma rozmiaru przynajmniej 2*minMonsterDist, to olej sprawdzanie odleglosci
                Maths::LengthSQ(sf::Vector2f((float)(tile.x - mEntryPos.x), (float)(tile.y - mEntryPos.y))) <= mDesc.minMonsterDist * mDesc.minMonsterDist) );

            // zaklepanie pola, zeby sie 2 gniazda nie zespawnowaly na jednym
            mCurrent[tile.x][tile.y] = LAIR;

            // i teraz pasowaloby cos tu postawic..
            float offsetX = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // offsety, zeby nie staly tak bardzo jednolicie
            float offsetY = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // + 0.5, zeby wycentrowac na kaflach
            int rot = rand() % 360;                                     // jeszcze obrot do tego
            // skala zadeklarowana w xmlu

            SPhysical lair = ChooseRandomlyRegardingFrequency(lairs);
			if (mapPhysicalToObjTypeCode.find(lair.file) == mapPhysicalToObjTypeCode.end()) {
				mapPhysicalToObjTypeCode[lair.file] = objTypesCount;
	            mXmlText << "\t<objtype code=\"lair" << StringUtils::ToString(objTypesCount) << "\" file=\"" << lair.file << "\" />\n";
				objTypesCount++;
			}
			int what = mapPhysicalToObjTypeCode[lair.file]; 
			mXmlText << "\t<obj code=\"lair" << StringUtils::ToString(what) << "\" x=\"" << tile.x + offsetX << "\" y=\"" << tile.y + offsetY << "\" rot=\"" << rot << "\" />\n";
        }

        // odejmujemy, ile dodalismy...
        mPassableLeft -= std::min(mPassableLeft, mDesc.lairs);
    }

    return true;
}

bool CRandomMapGenerator::PlaceMonsters()
{
    CTimer timer("- monsters: ");

    if (mDesc.mapType == SRandomMapDesc::MAP_BOSS || mDesc.mapType == SRandomMapDesc::MAP_FINAL_BOSS)
    {
        PhysicalsVector bosses = FilterByLevel(FilterByType(mPhysicals, "boss"), mDesc.level);
        if (bosses.empty())
            return false; // zadamy mapy z bossem, a takiego nie ma :C

        sf::Vector2i pos(mDesc.sizeX / 2, mDesc.sizeY / 2);
        while (mCurrent[pos.x][pos.y] != FREE) {
            // ta petla nie powinna sie wykonac, ale niech zostanie dla zabezpieczenia.
            // boss powinien moc sie zespawnowac na srodku mapy, jesli tak nie jest, to niedobrze
            pos = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
        }

        SPhysical boss = bosses[rand() % bosses.size()];
        mXmlText << "\t<objtype code=\"boss\" file=\"" << boss.file << "\" />\n";
        mXmlText << "\t<obj code=\"boss\" x=\"" << pos.x    // boss niech sie spawnuje na srodku mapy
            << "\" y=\"" << pos.y
            << "\" rot=\"" << rand() % 360
            << "\" name=\"boss\" trigger-radius=\"" << boss.bossTriggerRadius
            << "\" trigger-ai=\"" << boss.bossTriggerAI
            << "\" trigger-playlist=\"" << boss.bossPlaylist
            << "\" />\n";
    }

    PhysicalsVector monsters = FilterByLevel(FilterByType(mPhysicals, "monster"), mDesc.level);
    if (monsters.empty()) {
        return true;
    }

	std::map<std::string, int> mapPhysicalToObjTypeCode;
	int objTypesCount = 0;

    for (unsigned int i = 0; i < mDesc.monsters; ++i)
    {
        // znalezienie wolnego pola
        sf::Vector2i pos;
        do
            pos = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
        while (mCurrent[pos.x][pos.y] != FREE ||
                (std::min(mDesc.sizeX, mDesc.sizeY) > mDesc.minMonsterDist * 2.f &&     // jesli mapa nie ma rozmiaru przynajmniej 2*minMonsterDist, to olej sprawdzanie odleglosci
                Maths::LengthSQ(sf::Vector2f((float)(pos.x - mEntryPos.x), (float)(pos.y - mEntryPos.y))) <= mDesc.minMonsterDist * mDesc.minMonsterDist) );

        // i teraz pasowaloby cos tu postawic..
        float offsetX = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // offsety, zeby nie staly tak bardzo jednolicie
        float offsetY = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // + 0.5, zeby wycentrowac na kaflach
        int rot = rand() % 360;                                     // jeszcze obrot do tego
        // skala zadeklarowana w xmlu

        SPhysical monster = ChooseRandomlyRegardingFrequency(monsters);
		if (mapPhysicalToObjTypeCode.find(monster.file) == mapPhysicalToObjTypeCode.end()) {
			mapPhysicalToObjTypeCode[monster.file] = objTypesCount;
            mXmlText << "\t<objtype code=\"monster" << StringUtils::ToString(objTypesCount) << "\" file=\"" << monster.file << "\" />\n";
			objTypesCount++;
		}
		int what = mapPhysicalToObjTypeCode[monster.file]; 
        mXmlText << "\t<obj code=\"monster" << StringUtils::ToString(what) << "\" x=\"" << pos.x + offsetX << "\" y=\"" << pos.y + offsetY << "\" rot=\"" << rot << "\" />\n";
    }

    return true;
}

PhysicalsVector FilterByType(const PhysicalsVector & input, const std::string & type) {
    PhysicalsVector filteredOut;
    for (PhysicalsVector::const_iterator it = input.begin() ; it != input.end() ; it++) {
        if (it->type == type) filteredOut.push_back(*it);
    }

    return filteredOut;
}

std::string CRandomMapGenerator::GenerateNextLootTemplateFile(bool canBeObstacle, float additionalWeaponProbability)
{
    if (canBeObstacle && mSpawnedChestsCount < 3 && gRand.Rndf() < 0.25) {
        fprintf(stderr, "Spawning chest!\n");
        return "data/physicals/obstacles/chest.xml";
    }
    float realWeaponSpawnProbability = mSpawnWeaponProbability + (additionalWeaponProbability * (3 - mSpawnedWeaponsCount));
    fprintf(stderr, "realWeaponSpawnProbability = %f\n", realWeaponSpawnProbability);
    if (gRand.Rndf() < realWeaponSpawnProbability) {
        fprintf(stderr, "Spawning weapon!\n");
        mSpawnedWeaponsCount++;
        mSpawnWeaponProbability = 0.25f - (mSpawnedWeaponsCount * 0.30f);
        return "data/loots/weapon.xml";
    } else {
        mSpawnWeaponProbability += 0.05f - (mSpawnedWeaponsCount * 0.02f);
    }

    PhysicalsVector loots = FilterByLevel(FilterByType(mPhysicals, "loot"), mDesc.level);
    if (loots.empty()) {
        return "";
    }
    SPhysical loot = ChooseRandomlyRegardingFrequency(loots);
    
    // pozniej tutaj wstawic takie bajery, zeby bralo pod uwage,
    // * czy na aktualnym levelu juz byla wylosowana jakas bron?
    // * jak dawno temu byla wylosowana bron?
    // tak zeby zrownowazyc :) wypadanie ciekawych rzeczy
    
    return loot.file;

    // o kurczaki, trzeba bedzie jakis mechanizm dorobic, zeby dalo sie przepychac
    // konkretne weapony poprzez pliki .xml.... uga buga...
}

CLoot * CRandomMapGenerator::GenerateNextLoot(float additionalWeaponProbability)
{
    const std::string lootTemplateFile = GenerateNextLootTemplateFile(false, additionalWeaponProbability);
    if (lootTemplateFile.empty()) return NULL;

    CLoot * loot = dynamic_cast<CLootTemplate*>(gResourceManager.GetPhysicalTemplate(lootTemplateFile))->Create();
    if (loot->GetGenre() == L"weapon") { //a moze "random weapon" ?
        const std::string ability = GetRandomWeaponFile(mDesc.level);
        fprintf(stderr, "Generated random weapon: %s\n", ability.c_str());
        loot->SetAbility(ability);
    }
    return loot;
}

bool CRandomMapGenerator::PlaceLoots()
{
    CTimer timer("- loots: ");

    if (mDesc.loots && mPassableLeft)
    {
        PhysicalsVector objTypes;

        for (unsigned int i = 0; i < std::min(mPassableLeft, mDesc.loots); ++i)
        {
            // znalezienie wolnego pola
            sf::Vector2i tile;
            do
                tile = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
            while (mCurrent[tile.x][tile.y] != FREE);

            // zaklepanie pola, zeby sie 2 znajdki nie zespawnowaly na jednym
            mCurrent[tile.x][tile.y] = LOOT;

            // i teraz pasowaloby cos tu postawic..
            float offsetX = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // offsety, zeby nie staly tak bardzo jednolicie
            float offsetY = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // + 0.5, zeby wycentrowac na kaflach
            // obrot w przypadku przedmiotow nie ma sensu, skala zadeklarowana w xmlu

            std::string lootTemplateFile = GenerateNextLootTemplateFile(true);
            // brzydki hak:
            if (lootTemplateFile == "data/loots/weapon.xml") {
                const std::string ability = GetRandomWeaponFile(mDesc.level);
                fprintf(stderr, "Generated random weapon: %s\n", ability.c_str());

                mXmlText << "\t<obj templateFile=\"" << lootTemplateFile << "\" x=\"" << tile.x + offsetX << "\" y=\"" << tile.y + offsetY << "\" ><ability>" << ability << "</ability></obj>\n";
            } else {
                mXmlText << "\t<obj templateFile=\"" << lootTemplateFile << "\" x=\"" << tile.x + offsetX << "\" y=\"" << tile.y + offsetY << "\" />\n";
            }
        }

        // odejmujemy, ile dodalismy...
        mPassableLeft -= std::min(mPassableLeft, mDesc.loots);
    }

    return true;
}

// inne efekty, niepowiazane z konkretnym obiektem - np. ekran victory po zabiciu bossa
bool CRandomMapGenerator::PlaceMiscEffects()
{
    // na razie inne efekty sa tylko na ostatniej mapie
    if (mDesc.mapType != SRandomMapDesc::MAP_FINAL_BOSS)
        return true;

    // niewidzialne drzwi, uzuwane przez wszystkie 
    mXmlText << "\t<objtype code=\"invisible-door\" file=\"data/physicals/doors/invisible-door.xml\" />\n";

    // ostatnia mapa: cutscenka po zabiciu bossa
    mXmlText << "\t<obj code=\"invisible-door\" x=\"-5\" y=\"-5\">\n"
        << "\t\t<cond check=\"once\">\n"
        << "\t\t\t<type>killed</type>\n"
        << "\t\t\t<param>boss</param>\n"
        << "\t\t</cond>\n"
        << "\t\t<effect-on-open type=\"console\">\n"
        << "\t\t\t<text>[\n"
        << "\t\t\t\tdelay 3\n"
        << "\t\t\t\tclear-physicals\n"
        << "\t\t\t\tclear-logic\n"
        << "\t\t\t\tunload-map\n"
        << "\t\t\t\texit-to-main-menu\n"
        << "\t\t\t\tplay-cutscene data/cutscene/enddemo.xml\n"
        << "\t\t\t]</text>\n"
        << "\t\t</effect-on-open>\n"
        << "\t</obj>\n";

    return true;
}


void CRandomMapGenerator::ReleaseCurrent()
{
    if (mCurrent)
    {
        for (unsigned int i = 0; i < mDesc.sizeX; ++i)
            delete[] mCurrent[i];
        delete[] mCurrent;
        mCurrent = NULL;
    }

    mXmlText.str("");
}

std::string CRandomMapGenerator::GetSetForLevel(unsigned int level)
{
	std::string best = "";
	unsigned int bestValue = 0;
	std::map<std::string, SPartSet>::iterator iterator = this->mPartSets.begin();
	for (; iterator != this->mPartSets.end(); iterator++)
	{
		if (iterator->second.fromLevel <= level && iterator->second.fromLevel >= bestValue){
			best = iterator->first;
			bestValue = iterator->second.fromLevel;
		}
	}
	return best;
}

// -----------------------------------

CRandomMapGenerator::CRandomMapGenerator():
    mCurrent(NULL),
    mPassableLeft(0u)
{
    LoadPartSets("data/generator.xml");
	mTilesFolder = gGameOptions.GetUserDir() + "/generated_tiles";
}

CRandomMapGenerator::~CRandomMapGenerator()
{
}


bool CRandomMapGenerator::LoadPartSets(const std::string& filename)
{
    CXml xml(filename, "root");

    if (!xml.GetRootNode())
        return false;

    if (xml.GetString(xml.GetRootNode(), "type") != "map-gen")
    {
        fprintf(stderr, "CRandomMapGenerator::LoadPartSets: invalid XML type \"%s\", \"map-gen\" expected", xml.GetString(xml.GetRootNode(), "type").c_str());
        return false;
    }

    for (rapidxml::xml_node<>* n = xml.GetChild(xml.GetRootNode(), "set"); n; n = xml.GetSibl(n, "set"))
    {
        // jesli zestaw juz istnieje, olac go
        std::string name = xml.GetString(n);
        if (mPartSets.find(name) != mPartSets.end())
        {
            fprintf(stderr, "CRandomMapGenerator::LoadPartSets: set %s already loaded, omitting...\n", name.c_str());
            continue;
        }

        SPartSet set;

        // kafle
        if (xml.GetChild(n, "tiles"))
            for (rapidxml::xml_node<>* tile = xml.GetChild(xml.GetChild(n, "tiles"), "tile"); tile; tile = xml.GetSibl(tile, "tile"))
                set.tiles.push_back(xml.GetString(tile, "image"));

        // zestaw MUSI zawierac chociaz jeden kafel
        if (!set.tiles.size())
        {
            fprintf(stderr, "CRandomMapGenerator::LoadPartSets: set %s doesn't contain any tiles, omitting...\n", name.c_str());
            continue;
        }

        // doodahy
        if (xml.GetChild(n, "doodahs"))
            for (rapidxml::xml_node<>* doodah = xml.GetChild(xml.GetChild(n, "doodahs"), "doodah"); doodah; doodah = xml.GetSibl(doodah, "doodah"))
                if (xml.GetInt(doodah, "underfoot") > 0)   // do postawienia pod nogami
                    set.doodahsOnGround.push_back(SPartSet::SDoodah(xml.GetString(doodah, "file"), !!(xml.GetString(doodah, "z") == "foreground"), xml.GetFloat(doodah, "scale", 1.0f)));
                else
                    set.doodahs.push_back(SPartSet::SDoodah(xml.GetString(doodah, "file"), !!(xml.GetString(doodah, "z") == "foreground"), xml.GetFloat(doodah, "scale", 1.0f)));

		set.maxDoodahsOnGround = xml.GetInt(xml.GetChild(n, "max-doodahs-on-ground"), "value", 5);
		set.minDoodahsOnGround = xml.GetInt(xml.GetChild(n, "min-doodahs-on-ground"), "value", 2);
		set.fromLevel = xml.GetInt(n, "level");
        
        // i przynajmniej jednego doodaha
        if (!set.doodahs.size())
        {
            fprintf(stderr, "CRandomMapGenerator::LoadPartSets: set %s doesn't contain any doodahs, omitting...\n", name.c_str());
            continue;
        }

        mPartSets.insert(std::make_pair(xml.GetString(n, "name"), set));
    }

    if (xml.GetChild(xml.GetRootNode(), "physicals")) {
        for (rapidxml::xml_node<>* n = xml.GetChild(xml.GetRootNode(), "physicals")->first_node(); n; n = n->next_sibling()) {
            std::string type = n->name();
            int level = xml.GetInt(n, "level");
            int minLevel = xml.GetInt(n, "minLevel");
            int maxLevel = xml.GetInt(n, "maxLevel");
            if (level != 0) {
                minLevel = maxLevel = level;
            }
            float frequency = xml.GetFloat(n, "frequency", 1.0f);
            std::string file = xml.GetString(n, "file");
            // opcjonalne, tycza sie tylko bossow
            float bossTriggerRadius = xml.GetFloat(n, "trigger-radius");
            std::string bossTriggerAI = xml.GetString(n, "trigger-ai");
            std::string bossPlaylist = xml.GetString(n, "trigger-playlist");

            mPhysicals.push_back(SPhysical(type, file, minLevel, maxLevel, frequency, bossTriggerRadius, bossTriggerAI, bossPlaylist));
        }
    }

    // maski do kafli
    if (xml.GetChild(xml.GetRootNode(), "tile-masks"))
        for (rapidxml::xml_node<>* n = xml.GetChild(xml.GetChild(xml.GetRootNode(), "tile-masks"), "tile-mask"); n; n = xml.GetSibl(n, "tile-mask"))
            mTileMasks.push_back(xml.GetString(n, "file"));
    
    // sortowanie gniazd, potworow i przedmiotow po levelu, zeby potem bylo latwiej dobierac
    std::sort(mPhysicals.begin(), mPhysicals.end(), VectorCompareFunc);

    return true;
}


bool CRandomMapGenerator::GenerateRandomMap(const std::string& filename, const SRandomMapDesc& desc)
{
    CTimer timer("Whole map: ");

    // zwalnianie poprzedniej mapy
    ReleaseCurrent();

    if (!desc.sizeX || !desc.sizeY)
    {
        fprintf(stderr, "CRandomMapGenerator::GenerateRandomMap: invalid width and/or height: w=%u h=%u\n", desc.sizeX, desc.sizeY);
        return false;
    }

    mSpawnedChestsCount = 0;
    mSpawnedWeaponsCount = 0;
    mSpawnWeaponProbability = 0.2f;

    mDesc = desc;
    mPassableLeft = mDesc.sizeX * mDesc.sizeY;

    mCurrent = new unsigned int*[mDesc.sizeX];
    for (unsigned int i = 0; i < mDesc.sizeX; ++i)
    {
        mCurrent[i] = new unsigned int[mDesc.sizeY];
        // domyslnie wszystkie pola sa zablokowane, potem kopiemy tunele
        memset(mCurrent[i], BLOCKED, mDesc.sizeY * sizeof(unsigned int));
    }

    // 'naglowek'
    mXmlText << "<map>\n"
        << "\t<version>" << Map::CURRENT_MAP_VERSION << "</version>\n"
        << "\t<width>" << mDesc.sizeX << "</width>\n"
        << "\t<height>" << mDesc.sizeY << "</height>\n";


    if (!GenerateMap())         return false;
    if (!PlaceTiles())          return false;
    if (!PlaceRegions())        return false;
    if (!PlaceWalls())          return false;
    if (!PlaceDoodahs())        return false;
    if (!PlaceBossDoors())      return false;   // zwraca true, jesli nie ma bossa na mapie
    if (!PlaceLoots())          return false;
    if (!PlaceMonsters())       return false;
    if (!PlaceLairs())          return false;
    if (!PlaceMiscEffects())    return false;

    // koniec
    mXmlText << "</map>";

	if (!boost::filesystem::exists(filename)) // jesli nie ma pliku, to moze nie byc folderow
		boost::filesystem::create_directories(filename.substr(0, filename.find_last_of("/\\")));
    else // upewniamy sie, ze zapisujemy nowa mape, nie dopisujemy do jakiejs starej
        std::remove(filename.c_str());

    // skoro doszlismy tutaj, to znaczy, ze mozna zapisywac!
	if (!FileUtils::WriteToFile(filename, mXmlText.str()))
    {
        fprintf(stderr, "CRandomMapGenerator::GenerateRandomMap: couldn't write file \"%s\"!\n", filename.c_str());
        return false;
    }

    return true;
}
