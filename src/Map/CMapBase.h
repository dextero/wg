#ifndef __CMAPBASE_H__
#define __CMAPBASE_H__

#include <vector>

#include <cstdio>

// sf::Vector2i
#include <SFML/System/Vector2.hpp>

/* baza klas reprezentujacych mapy
 * w poczatkowej fazie bedzie sluzyc wylacznie do trzymania Tile'i w Mapie,
 * ale moze tez byc uzyta do trzymania pomocniczych struktur "nakladanych" na mape,
 * przykladowo trzymac dane potrzebne przy wyliczaniu AI
 *
 * klasa F musi miec publiczny, bezargumentowy konstruktor, jesli ma byc tworzona automatycznie
 */
template <class F> class CMapBase {
protected:
    // jednowymiarowy wektor kafli (optymalizacja)
    std::vector<F>   mFields;

    int mWidth;
    int mHeight;

    // pole brzegowe, zwracane w metodach safeAt() gdy wspolrzedne sa poza zakresem
    // przykladowo, w przypadku mapy poziomu borderField reprezentuje jednolita sciane
    F borderField;
public:
    CMapBase(){
		 
    }
	~CMapBase(){
		
	}
	 

    // ilosc pol (width*height)
    inline int FieldCount(){
        return mFields.size();
    }

    // zwraca pole o danych wspolrzednych, bez sprawdzania poprawnosci
    inline F &At(sf::Vector2i &c) {
        return mFields[mHeight*c.x+c.y];
    }

    inline F &At(int x, int y) {
        return mFields[mHeight*x + y];
    }

    inline F &At(int i) {
        return mFields[i];
    }

    // zwraca pole o danych wspolrzednych, lub borderField jesli poza zakresem
    inline F &SafeAt(sf::Vector2i &c) {
        if (OnMap(c))
            return At(c);
        else
            return borderField;
    }

    inline F &SafeAt(int x, int y) {
        if (OnMap(x, y))
            return At(x, y);
        else
            return borderField;
    }

    // przejscie pomiedzy indeksem a Vector2i
    inline sf::Vector2i CoordByIndex(int i) {
        sf::Vector2i c;
        c.x = i/mHeight;
        c.y = i%mHeight;
        return c;
    }

    // czy pole jest na planszy
    inline bool OnMap(int i) {
        return ((i >= 0) && (i < mFields.size()));
    }

    inline bool OnMap(sf::Vector2i &c) {
        return ((c.x >= 0) && (c.y >= 0) && (c.x < mWidth) && (c.y < mHeight));
    }

    inline bool OnMap(int x, int y) {
        return ((x >= 0) && (y >= 0) && (x < mWidth) && (y < mHeight));
    }

    inline int GetWidth(){
        return mWidth;
    }
    
    inline int GetHeight(){
        return mHeight;
    }
};

#endif /*__CMAPBASE_H__*/
