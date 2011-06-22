#ifndef __CPathFinder__
#define __CPathFinder__

#include <SFML/System/Vector2.hpp>

class CPath;
class CWaypointCell;
class CWaypointRegion;

class CPathFinder
{
public:
    /** /brief Szuka drogi uzywajac globalnej mapy gestosci.
     *  /param start Punkt startowy (w "jednostkach" swiata)
     *  /param goal Punkt docelowy (w "jednostkach" swiata)
     *  /param physicalSize Wielkosc stwora, ktory szuka drogi dojscia
     *  /param path Znaleziona (o ile istnieje) sciazka (indeksy kafli). NULL w przeciwnym wypadku
     *  /return True jezeli sciezka istnieje
     */
    bool FindPath(const sf::Vector2f & start, const sf::Vector2f & goal, 
                  const sf::Vector2f & physicalSize, CPath ** path);

private:
    CWaypoint * FindPath(const CWaypointRegion & start, const CWaypointRegion & goal) const;
    CWaypoint * FindPath(const CWaypointCell & start, const CWaypointCell & goal) const;
};

#endif // __CPathFinder__
