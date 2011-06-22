#ifndef __CRAND_H__
#define __CRAND_H__

#include "CSingleton.h"
#include <cstdlib>

#define gRand CRand::GetSingleton()

/// Klasa reprezentujaca generator liczb pseudolosowych
/// rAum: Mysle ze moze byc jako singleton...
/// 16.05 - napisalem druga wersje, wolniejsza i mniej losowa ale mozliwe ze stara wersja byla zle generowana
/// przez niektore kompilatory co moglo sie przekladac na rozne dziwne bledy

#if defined(FAST_RAND)

class CRand : public CSingleton<CRand>
{
    unsigned mSeed;
    /// Algorytm pseudolosowy opracowany przez demoscenowa grupe RGBA
    /// Szybki i bardzo dobry (23 losowe bity), 4x szybszy od rand()/MAX_RAND itp.
    /// http://rgba.scenesp.org/iq/computer/articles/sfrand/sfrand.htm
    inline float sfrand() {
        float res;
        mSeed *= 16807;
        *((unsigned int *)&res) = (mSeed & 0x007fffff) | 0x40000000;
        return res;
    }
public:
    CRand() { srand(mSeed=1); }

    /// Trzeba wywolac aby ustawic odpowiednie ziarno
    /// inaczej w kaadym uruchomieniu beda zwracane te same wartosci
    /// !!! Uwaga: seed != 0 !!!
    void Seed(unsigned seed) { srand(mSeed = (unsigned) seed); }

    /// Zwraca wartosc -1..1
    /// TODO: zmienic nazwe na lepsza
    float Rndnf() { return sfrand()-3.f; }
    /// Zwraca wartosci 0..1
    float Rndf() { return (sfrand()-2.f) * 0.5f; }
	/// Zwraca wartosci 0..1, im liczba bli¿sza mostProbabled tym wiêksza szansa na jej zwrócenie
	float Rndf( float mostProbabled );
    /// Zwraca wartosci begin..end
    float Rndf(float begin, float end) { return Rndf() * (end - begin) + begin; }

    /// Zwraca wartooci od 0...RAND_MAX
    int Rnd() { return rand(); }
    /// Zwraca wartosci min..max
    int Rnd(unsigned min, unsigned max) { return min != max? (Rnd() % (max - min) + min) : max; }
};

#else // not defined FAST_RAND

class CRand : public CSingleton<CRand>
{
    unsigned mSeed;
public:
	CRand() { srand(mSeed=1); }

    /// Trzeba wywolac aby ustawic odpowiednie ziarno
    /// inaczej w kaadym uruchomieniu beda zwracane te same wartosci
    /// !!! Uwaga: seed != 0 !!!
    void Seed(unsigned seed) { srand(mSeed = (unsigned) seed); }

    /// Zwraca wartosc -1..1
    /// TODO: zmienic nazwe na lepsza
    float Rndnf() { return rand() / static_cast<float>( (RAND_MAX / 2) ) - 1.f;  }
    /// Zwraca wartosci 0..1
    float Rndf() { return rand() / static_cast<float> ( RAND_MAX ); }
	/// Zwraca wartosci 0..1, im liczba bli¿sza mostProbabled tym wiêksza szansa na jej zwrócenie
	float Rndf( float mostProbabled );
    /// Zwraca wartosci begin..end
    float Rndf(float begin, float end) { return Rndf() * (end - begin) + begin; }

    /// Zwraca wartooci od 0...RAND_MAX
    int Rnd() { return rand(); }
    /// Zwraca wartosci min..max
    int Rnd(unsigned min, unsigned max) { return min != max? (Rnd() % (max - min) + min) : max; }
    /// Zwraca wartosc 0..max (wlacznie!)
    int Rnd(unsigned max) { return Rnd(0, max); }
};
#endif // not defined FAST_RAND

#endif //__CRAND_H__//
