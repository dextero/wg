/*	
*	Mapa kolizji pozwalajaca sprawdzic czy dany punkt mapy jest wolny.
*	Zastosowanie: szukanie najkrotszej sciezki. 
*/

#ifndef _COLLISION_MAP_
#define _COLLISION_MAP_

#include "CCell.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

class CDynamicRegion;
class CPath;
class CPhysical;

class CCollisionMap
{
private:
	sf::Vector2i mSize;
	sf::Vector2f mCellPerTile;
	sf::Vector2f mSingleCellSize;
	std::vector< CCell > mCellList;
	std::vector< CDynamicRegion * > mRegionList;    // TODO: przerobic to na mape
    int mEnemyDensity;
    
    inline int CellToIndex(const sf::Vector2i & e) const   { return e.y * mSize.x + e.x; }

public:

	CCollisionMap();
	~CCollisionMap();

	void Initialize( sf::Vector2i size );
	void UpdateAllPhysicals();
    void UpdatePhysicals(int category);
	void UpdatePhysical( CPhysical * physical, bool collision );
	
	inline sf::Vector2i GetSize()							{ return mSize; }
	inline sf::Vector2f GetCellsPerTile()					{ return mCellPerTile; }

	bool GetCollisionByCell(const sf::Vector2i& c) const;
	bool GetCollisionByRect(const sf::Vector2i& leftUp, const sf::Vector2i& rightDown) const;

    int GetDensityByCell(const sf::Vector2i& c);
    int GetDensityByRect(const sf::Vector2i& leftUp, const sf::Vector2i& rightDown);
	
	void UpdateRegions();
	
	void AddPath(const CPath * path);
	void RemovePath(const CPath * path);
	void ModifyPath(const CPath * path, int delta);
    bool FindPath(const sf::Vector2f & start, const sf::Vector2f & goal, 
                  const sf::Vector2f & physicalSize, CPath ** path);
	
		/** /brief Zwraca pozycje kafla dla danych wspolrzednych globalnych
		 */
	sf::Vector2i GetCellByCoords( const sf::Vector2f& point );
		
		/** /brief Zwraca wspolrzedne globalne dla danego kafla
		 */
	sf::Vector2f GetCoordsByCell( const sf::Vector2i& cell );
	
	    /** \brief Zamienia rozmiar w ilosc zajmowanych pol
	     */
	sf::Vector2i ConvertSizeToCells( const sf::Vector2f & size );

    CDynamicRegion * GetRegionByCell(const sf::Vector2i & cell);
};

#endif

