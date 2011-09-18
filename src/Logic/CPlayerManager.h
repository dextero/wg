#ifndef CPLAYERMANAGER_H_
#define CPLAYERMANAGER_H_

#include <vector>
#include <string>
#include "../Utils/CSingleton.h"

class CPlayer;
class CDynamicRegion;
class CPhysical;

#define gPlayerManager CPlayerManager::GetSingleton()

class CPlayerManager : public CSingleton<CPlayerManager>
{
public:
	CPlayerManager();
	virtual ~CPlayerManager();

	void LoadPlayer(size_t number);						/* Wczytuje z pliku podanego gracza i rejestruje */
	void RegisterPlayer(CPlayer *player,size_t number);	/* Jesli juz istnieje gracz o tym numerze - usuw go */
	void UnregisterPlayer(CPlayer* player);
	void UnregisterPlayer(size_t number);
	CPlayer *GetPlayerByNumber(size_t number);

	/* Metody do przegladania wszystkich zarejestrowanych graczy */

	inline CPlayer *GetPlayerByOrder(size_t order)	{ return mPlayers[order]; }
	inline size_t GetPlayerCount()							{ return mPlayers.size(); }

	/* Metody wplywajace na wszystkich zarejestrowanych graczy */

    void XPGained(float xp, bool ignoreSkillPoints = false, bool muteSound = false);
    void SetGodMode(bool gm);
    void MovePlayersToRegion(const std::string &region);
	void MovePlayersToFirstPlayer();
    bool VerifyKnownPhysical(CPhysical *phys);
private:
    std::vector<CPlayer*> mPlayers;
};

#endif /*CPLAYERMANAGER_H_*/
