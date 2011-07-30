#ifndef CLAIR_H_
#define CLAIR_H_

#include <string>
#include <vector>

#include "Factory/CEnemyTemplate.h"
#include "CActor.h"
#include <SFML/System/Vector2.hpp>

class CEnemy;
class CDetector;

class CLair: public CActor {
    friend class CPhysicalManager;
protected:
    CLair(const std::wstring& uniqueId);
    virtual ~CLair();
public:
	inline void SetSpawnLimit( int spawnLimit ) { mSpawnLimit = spawnLimit; }
	inline void SetSpawnRate( float spawnRate ) { mSpawnRate = spawnRate; }
	inline void SetSpawnPosition( const sf::Vector2f& position) { mSpawnPosition = position; }
    inline void SetSpawnSound( const std::string & spawnSound ) { mSpawnSound = spawnSound; }
    inline void SetMonsterCount(int c){ mMonsterCount = c; }
    inline void SetGroupingHint(int g){ mGroupingHint = g; }
    void Ready();

	// dodaje nowy typ potwora do produkcji
	void AddEnemyType( std::string enemy );

	// przypina danego potwora spowrotem do gniazda (po zaladowaniu stanu gry z dysku)
	void RegisterMonsterAsSpawned(CEnemy * enemy);

	// w update moze nastapic wytworzenie i rejestracja potwora
	virtual void Update(float dt);
protected:
	// funkcja wywolana jako callback gdy jeden z wytworzonych potworow umiera
	static void SpawneeDeath(CPhysical *,void *, void *);
    // funkcja wywolana, gdy detektor zostanie zniszczony (np. przed gniazdem)
    static void DetectorDestroyed(CPhysical *, void *, void *);
	// stwarza nowego potwora, bez zadnych rejestracji
	CEnemy *Spawn();
	void RemoveSpawnee(CEnemy *spawnee);

	// maksymalna liczba potworow przypisanych do gniazda
	int mSpawnLimit;
	// aktualna liczba potworow przypisanych do gniazda
	int mSpawnCount;
	// liczba potworow produkowanych na sekunde
	float mSpawnRate;
	// czas od ostatniego wyprodukowanego potwora
	float mTimeSinceSpawn;
	// relatywne wspolrzedne spawnowania
	sf::Vector2f mSpawnPosition;
    // sciezka do pliku dzwieku odtwarzanego w momencie wytworzenia potwora
    std::string mSpawnSound;
	// promien najwiekszego potwora 
	float mMaxEnemyRadius;
    // laczna ilosc potworow, jaka moze byc jeszcze wyprodukowana
    int mMonsterCount;
    // wielkosc grupki potworow, domyslnie 1
    int mGroupingHint;

	CDetector * mCollisionDetector;
	std::vector<CEnemyTemplate*> mToSpawn;
	std::vector<CEnemy*> mSpawned;
private:
    int mCurrentGroup;
    bool mLocked;
};

#endif /*CLAIR_H_*/
