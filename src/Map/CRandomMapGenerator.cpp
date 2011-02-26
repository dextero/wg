#include "CRandomMapGenerator.h"

#include "CMap.h"

#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"

#include "../Utils/CXml.h"
#include "../Utils/MathsFunc.h"
#include "../Utils/StringUtils.h"
#include "../Utils/FileUtils.h"

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
    CTimer(const std::string& msg): msg(msg) { clock.Reset(); }
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
    return (first.level < last.level);
}

// -----------------------------------

// metody generowania tuneli
bool CRandomMapGenerator::GenerateTunnelsFromRandomCenter()
{
    // punkt, od ktorego zaczynamy kopac
    unsigned int origX = rand() % mDesc.sizeX, origY = rand() % mDesc.sizeY;
    // punkt, w ktorym kopiemy
    unsigned int nextX = origX, nextY = origY;
    // ile pol ma byc 'przejezdnych'?
    unsigned int mPassableLeft = (unsigned int)(((100.f - mDesc.obstaclesAreaPercent) / 100.f) * (float)(mDesc.sizeX * mDesc.sizeY));

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
    unsigned int size = 2;
    // ilosc wierzcholkow grafu
    const unsigned int NUM_VERTS = 50;

    // losowanie wierzcholkow grafu
    // bool = flaga oznaczajaca, czy dany wierzcholek jest juz z czyms polaczony
    std::pair<sf::Vector2i, bool> verts[NUM_VERTS];
    for (size_t i = 0; i < NUM_VERTS; ++i)
        verts[i] = std::make_pair(sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY), false);

    // ile pol ma byc 'przejezdnych'?
    unsigned int mPassableLeft = (unsigned int)(((100.f - mDesc.obstaclesAreaPercent) / 100.f) * (float)(mDesc.sizeX * mDesc.sizeY));

    for (unsigned int i = 0; i < mPassableLeft;)   // NIE blad, ma byc pusto po ostatnim ;
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

        // wybieramy deltaMax punktow na odcinku pomiedzy start i end
        // i kopiemy przejscie w kazdym punkcie (+ 3 pola przylegle)
        // pojedynczy krok
        sf::Vector2i deltaInt(verts[end].first - verts[start].first);
        sf::Vector2f delta((float)(deltaInt.x), (float)(deltaInt.y));
        // ile punktow bedzie sprawdzanych, wiecej = lepiej
        int deltaMax = std::max(abs(deltaInt.x), abs(deltaInt.y)) * 2;
        delta /= (float)deltaMax;

        // kopanie tunelu od startu do konca
        sf::Vector2f at((float)verts[start].first.x, (float)verts[start].first.y);
        for (int j = 0; j < deltaMax; ++j)
        {
            at += delta;
            unsigned int atX = (unsigned int)at.x, atY = (unsigned int) at.y;

            if (atX >= mDesc.sizeX || atY >= mDesc.sizeY)
                break;

            // kopanie + poszerzanie
            for (unsigned int x = 0; x < size; ++x)
                if (atX + x < mDesc.sizeX)
                    for (unsigned int y = 0; y < size; ++y)
                        if (atY + y < mDesc.sizeY)
                            if (mCurrent[atX + x][atY + y] == BLOCKED)
                            {
                                mCurrent[atX + x][atY + y] = FREE;
                                ++i;
                            }
        }
    }
    
    mPassableLeft = mDesc.sizeX * mDesc.sizeY - mPassableLeft;

    return true;
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
    if (topLeft.find("data/maps/generated_tiles") != std::string::npos ||
        topRight.find("data/maps/generated_tiles") != std::string::npos ||
        bottomLeft.find("data/maps/generated_tiles") != std::string::npos ||
        bottomRight.find("data/maps/generated_tiles") != std::string::npos)
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

    return GenerateTunnelsGraph();
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

    // losujemy co 4 kafel, reszte bedziemy dobierac wg sasiadow
    // [*][ ][*]
    // [ ][ ][ ]
    // [*][ ][*]
    // [*] - kafel losowany, reszta to "posrednie"
    for (unsigned int y = 0; y < mDesc.sizeY; y += 2)
        for (unsigned int x = 0; x < mDesc.sizeX; x += 2)
            tiles[x][y] = rand() % set.tiles.size();

    // mapa na dobrane kafle, zeby nie duplikowac
    std::map<std::string, unsigned int> generatedTiles;
    // i wektor, bo jak sie doda do set.tiles to drugie generowanie jest o dupe rozbic
    std::vector<std::string> tilePaths = set.tiles;

    // tu dobieramy reszte tak, zeby pasowaly
    for (unsigned int y = 0; y < mDesc.sizeY; ++y)
        for (unsigned int x = 0; x < mDesc.sizeX; ++x)
        {
            std::string nameTopLeft, nameTopRight, nameBottomLeft, nameBottomRight;

            // IOCCC mode on
            // dex: ja to pisalem, mnie mordujcie...
            // tu ponizej i TYLKO tu ma byc set.tiles, jesli sie pokrzaczy na indeksach,
            // to znaczy, ze chcialo brac indeks generowanego kafla, a tak byc nie moze
            // [x] = tu jestes
            if (x % 2)
            {
                if (y % 2)
                {
                    // [*][ ][*]
                    // [ ][x][ ]
                    // [*][ ][*]
                    nameTopLeft = set.tiles[tiles[x - 1][y - 1]];
                    nameTopRight = set.tiles[tiles[(x + 1 < mDesc.sizeX ? x + 1 : x - 1)][y - 1]];
                    nameBottomLeft = set.tiles[tiles[x - 1][(y + 1 < mDesc.sizeY ? y + 1 : y - 1)]];
                    nameBottomRight = set.tiles[tiles[(x + 1 < mDesc.sizeX ? x + 1 : x - 1)][(y + 1 < mDesc.sizeY ? y + 1 : y - 1)]];
                }
                else
                {
                    // [*][x][*]
                    nameTopLeft = set.tiles[tiles[x - 1][y]];
                    nameTopRight = set.tiles[tiles[(x + 1 < mDesc.sizeX ? x + 1 : x - 1)][y]];
                    nameBottomLeft = set.tiles[tiles[x - 1][y]];
                    nameBottomRight = set.tiles[tiles[(x + 1 < mDesc.sizeX ? x + 1 : x - 1)][y]];
                }
            }
            else
            {
                if (y % 2)
                {
                    // [*]
                    // [x]
                    // [*]
                    nameTopLeft = set.tiles[tiles[x][y - 1]];
                    nameTopRight = set.tiles[tiles[x][y - 1]];
                    nameBottomLeft = set.tiles[tiles[x][(y + 1 < mDesc.sizeY ? y + 1 : y - 1)]];
                    nameBottomRight = set.tiles[tiles[x][(y + 1 < mDesc.sizeY ? y + 1 : y - 1)]];
                }
                else
                {
                    // jestesmy na wylosowanym polu
                    continue;
                }
            }

            // losowanie maski
            unsigned int tileMask = (mTileMasks.size() ? (unsigned int)(rand()) % mTileMasks.size() : (unsigned int)-1);

            // IOCCC mode off

            if (!boost::filesystem::exists("data/maps/generated_tiles"))
                boost::filesystem::create_directory("data/maps/generated_tiles");
            
            // nazwa przejsciowego kafla
            unsigned long long hash = StringUtils::GetStringHash(nameTopLeft + nameTopRight + nameBottomLeft + nameBottomRight);
            std::string imgName = "data/maps/generated_tiles/" +
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
    mXmlText << "\t<objtype code=\"iw\" file=\"data/physicals/invisible-wall.xml\" />\n";

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

    // za malo miejsca, zeby postawic chociazby regiony wejscia i wyjscia..
    if (mPassableLeft < 2) return false;

    // wejscie i exit
    {
        sf::Vector2i entry, exit;
        unsigned int dist = 0;

        for (size_t i = 0; i < 100; ++i)
        {
            sf::Vector2i newEntry, newExit;
            // do skutku, az bedzie na "przejezdnym"
            do
                newEntry = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
            while (!mCurrent[newEntry.x][newEntry.y]);
            do
                newExit = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
            while (!mCurrent[newExit.x][newExit.y]);

            // wybieramy najdluzsza droge
            unsigned int newDist = DistanceDijkstra(newEntry, newExit);
            if (newDist != (unsigned int)-1 && newDist > dist)
            {
                entry = newEntry;
                exit = newExit;
                dist = newDist;
            }
        }
        mXmlText << "\t<region name=\"entry\" x=\"" << entry.x + 0.5f << "\" y=\"" << entry.y + 0.5f << "\" rot=\"0\" scale=\"1\"></region>\n"
            << "\t<region name=\"exit\" x=\"" << exit.x + 0.5f << "\" y=\"" << exit.y + 0.5f << "\" rot=\"0\" scale=\"1\">\n"
            << "\t\t<next-map>data/maps/level01.xml</next-map>\n"
            << "\t\t<next-map-region>entry</next-map-region>\n"
            << "\t</region>\n"
            // tlo pod wyjscie - portal
            << "\t<objtype code=\"exit-portal\" file=\"data/physicals/walls/test-barrier.xml\" />\n"
            << "\t<obj code=\"exit-portal\" x=\"" << exit.x + 0.5f << "\" y=\"" << exit.y + 0.5f << "\" />\n";

        // te 2 regiony "zapychaja" pola
        mPassableLeft -= 2;
    }

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

                mXmlText << "\t<sprite file=\"" << set.doodahs[doodahNum].file
                    << "\" x=\"" << (float)x + offsetX
                    << "\" y=\"" << (float)y + offsetY
                    << "\" scale=\"" << scale
                    << "\" rot=\"" << rot
                    << "\" z=\"" << (set.doodahs[doodahNum].isInForeground ? "foreground" : "background")
                    << "\" />\n";
            }
        }

    return true;
}

bool CRandomMapGenerator::PlaceLairs()
{
    CTimer timer("- lairs: ");

    // gniazda
    if (mLairs.size() && mDesc.lairs && mPassableLeft)
    {
        // znajdz 'najtrudniejsze' gniazda pasujace do levela
        size_t hard = (size_t)-1, medium = (size_t)-1, easy = (size_t)-1;
        size_t at = 0;

        // przesuniecie 'wskaznika'
        for (; at < mLairs.size() - 1 && mLairs[at + 1].level <= mDesc.level; ++at);

        if (at < mLairs.size())
        {
            hard = at;
            mXmlText << "\t<objtype code=\"lair0\" file=\"" << mLairs[hard].file << "\" />\n";
        }
        if (at - 1 < mLairs.size()) // przekrecenie licznika?
        {
            medium = at - 1;
            mXmlText << "\t<objtype code=\"lair1\" file=\"" << mLairs[medium].file << "\" />\n";
        }
        if (at - 2 < mLairs.size())
        {
            easy = at - 2;
            mXmlText << "\t<objtype code=\"lair2\" file=\"" << mLairs[easy].file << "\" />\n";
        }

        for (unsigned int i = 0; i < std::min(mPassableLeft, mDesc.lairs); ++i)
        {
            // znalezienie wolnego pola
            sf::Vector2i tile;
            do
                tile = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
            while (!mCurrent[tile.x][tile.y]);

            // zaklepanie pola, zeby sie 2 gniazda nie zespawnowaly na jednym
            mCurrent[tile.x][tile.y] = LAIR;

            // i teraz pasowaloby cos tu postawic..
            float offsetX = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // offsety, zeby nie staly tak bardzo jednolicie
            float offsetY = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // + 0.5, zeby wycentrowac na kaflach
            int rot = rand() % 360;                                     // jeszcze obrot do tego
            // skala zadeklarowana w xmlu

            size_t what = rand() % 9;
            if (easy < mLairs.size() && what < 2)   // 2/9 gniazd 'easy'
                mXmlText << "\t<obj code=\"lair2\" x=\"" << tile.x + offsetX << "\" y=\"" << tile.y + offsetY << "\" rot=\"" << rot << "\" />\n";
            else if (medium < mLairs.size() && what < 5)    // 3/9 gniazd 'medium' lub 5/9, gdy nie ma easy
                mXmlText << "\t<obj code=\"lair1\" x=\"" << tile.x + offsetX << "\" y=\"" << tile.y + offsetY << "\" rot=\"" << rot << "\" />\n";
            else    // 4/9 gniazd 'hard' lub wszystkie, gdy nie ma medium
                mXmlText << "\t<obj code=\"lair0\" x=\"" << tile.x + offsetX << "\" y=\"" << tile.y + offsetY << "\" rot=\"" << rot << "\" />\n";
        }

        // odejmujemy, ile dodalismy...
        mPassableLeft -= std::min(mPassableLeft, mDesc.lairs);
    }

    return true;
}

bool CRandomMapGenerator::PlaceMonsters()
{
    CTimer timer("- monsters: ");

    // potwory
    for (size_t i = 0; i < mMonsters.size(); ++i)
        mXmlText << "\t<objtype code=\"monster" << StringUtils::ToString(i) << "\" file=\"" << mMonsters[i].file << "\" />\n";

    for (unsigned int i = 0; i < mDesc.monsters; ++i)
    {
        // znalezienie wolnego pola
        sf::Vector2i pos;
        do
            pos = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
        while (!mCurrent[pos.x][pos.y]);

        // i teraz pasowaloby cos tu postawic..
        float offsetX = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // offsety, zeby nie staly tak bardzo jednolicie
        float offsetY = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // + 0.5, zeby wycentrowac na kaflach
        int rot = rand() % 360;                                     // jeszcze obrot do tego
        // skala zadeklarowana w xmlu

        size_t what = rand() % mDesc.monsters;
        for (unsigned int j = 0; j < mMonsters.size(); ++j)
        {
            // zeby te mocniejsze jakos czesciej 'wypadaly', ciekawe, czy bedzie dzialac...
            if (j * j > what || mMonsters[j].level >= mDesc.level)
            {
                mXmlText << "\t<obj code=\"monster" << StringUtils::ToString(j) << "\" x=\"" << pos.x + offsetX << "\" y=\"" << pos.y + offsetY << "\" rot=\"" << rot << "\" />\n";
                break;
            }
        }
    }

    return true;
}

bool CRandomMapGenerator::PlaceItems()
{
    CTimer timer("- items: ");

    if (mItems.size() && mDesc.items && mPassableLeft)
    {
        // najlepsze przedmioty, ale o levelu nie wiekszym od podanego
        std::vector<size_t> mItemsToPlace;

        for (size_t i = mItems.size() - 1; i != (size_t)-1; --i)
            if (mItems[i].level <= mDesc.level)
            {
                mItemsToPlace.push_back(i);

                // max powiedzmy... 10 rodzajow itemow
                if (mItemsToPlace.size() > 9)
                    break;
            }

        for (size_t i = 0; i < mItemsToPlace.size(); ++i)
            mXmlText << "\t<objtype code=\"item" << i << "\" file=\"" << mItems[mItemsToPlace[i]].file << "\" />\n";

        for (unsigned int i = 0; i < std::min(mPassableLeft, mDesc.items); ++i)
        {
            // znalezienie wolnego pola
            sf::Vector2i tile;
            do
                tile = sf::Vector2i(rand() % mDesc.sizeX, rand() % mDesc.sizeY);
            while (!mCurrent[tile.x][tile.y]);

            // zaklepanie pola, zeby sie 2 itemy nie zespawnowaly na jednym
            mCurrent[tile.x][tile.y] = ITEM;

            // i teraz pasowaloby cos tu postawic..
            float offsetX = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // offsety, zeby nie staly tak bardzo jednolicie
            float offsetY = ((float)rand() / RAND_MAX + 0.5f) / 2.f;    // + 0.5, zeby wycentrowac na kaflach
            // obrot w przypadku przedmiotow nie ma sensu, skala zadeklarowana w xmlu

            size_t what = rand() % mItemsToPlace.size();
            mXmlText << "\t<obj code=\"item" << what << "\" x=\"" << tile.x + offsetX << "\" y=\"" << tile.y + offsetY << "\" />\n";
        }

        // odejmujemy, ile dodalismy...
        mPassableLeft -= std::min(mPassableLeft, mDesc.items);
    }

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

// -----------------------------------

CRandomMapGenerator::CRandomMapGenerator():
    mCurrent(NULL),
    mPassableLeft(0u)
{
    LoadPartSets("data/generator.xml");
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
                set.doodahs.push_back(SPartSet::SDoodah(xml.GetString(doodah, "file"), !!(xml.GetString(doodah, "z") == "foreground")));
        
        // i przynajmniej jednego doodaha
        if (!set.doodahs.size())
        {
            fprintf(stderr, "CRandomMapGenerator::LoadPartSets: set %s doesn't contain any doodahs, omitting...\n", name.c_str());
            continue;
        }

        mPartSets.insert(std::make_pair(xml.GetString(n, "name"), set));
    }

    // gniazda
    if (xml.GetChild(xml.GetRootNode(), "lairs"))
        for (rapidxml::xml_node<>* n = xml.GetChild(xml.GetChild(xml.GetRootNode(), "lairs"), "lair"); n; n = xml.GetSibl(n, "lair"))
            mLairs.push_back(SPhysical(xml.GetString(n, "file"), xml.GetInt(n, "level")));

    // potwory
    if (xml.GetChild(xml.GetRootNode(), "monsters"))
        for (rapidxml::xml_node<>* n = xml.GetChild(xml.GetChild(xml.GetRootNode(), "monsters"), "monster"); n; n = xml.GetSibl(n, "monster"))
            mMonsters.push_back(SPhysical(xml.GetString(n, "file"), xml.GetInt(n, "level")));

    // przedmioty
    if (xml.GetChild(xml.GetRootNode(), "items"))
        for (rapidxml::xml_node<>* n = xml.GetChild(xml.GetChild(xml.GetRootNode(), "items"), "item"); n; n = xml.GetSibl(n, "item"))
            mItems.push_back(SPhysical(xml.GetString(n, "file"), xml.GetInt(n, "level")));

    // maski do kafli
    if (xml.GetChild(xml.GetRootNode(), "tile-masks"))
        for (rapidxml::xml_node<>* n = xml.GetChild(xml.GetChild(xml.GetRootNode(), "tile-masks"), "tile-mask"); n; n = xml.GetSibl(n, "tile-mask"))
            mTileMasks.push_back(xml.GetString(n, "file"));
    
    // sortowanie gniazd, potworow i przedmiotow po levelu, zeby potem bylo latwiej dobierac
    std::sort(mLairs.begin(), mLairs.end(), VectorCompareFunc);
    std::sort(mMonsters.begin(), mMonsters.end(), VectorCompareFunc);
    std::sort(mItems.begin(), mItems.end(), VectorCompareFunc);

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


    if (!GenerateMap())     return false;
    if (!PlaceTiles())      return false;
    if (!PlaceWalls())      return false;
    if (!PlaceDoodahs())    return false;
//    if (!PlaceRegions())    return false;
//    if (!PlaceMonsters())   return false;
//    if (!PlaceLairs())      return false;
//    if (!PlaceItems())      return false;

    // koniec
    mXmlText << "</map>";

    // skoro doszlismy tutaj, to znaczy, ze mozna zapisywac!
	if (!FileUtils::WriteToFile(filename, mXmlText.str()));
    {
        fprintf(stderr, "CRandomMapGenerator::GenerateRandomMap: couldn't write file \"%s\"!\n", filename.c_str());
        return false;
    }

    return true;
}
