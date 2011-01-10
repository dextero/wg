#ifndef __CBESTIARY_H__
#define __CBESTIARY_H__

#include "../Utils/CSingleton.h"
#include <set>
#include <string>

class CEnemy;
class CActorTemplate;

class CBestiary: public CSingleton<CBestiary>
{
private:
    std::set<std::string> mEnabledMonsters;
    size_t mAllMonsters;

    std::string mFilePath;

    void Save();
public:
    CBestiary(): mAllMonsters(0u) {}
    inline const std::set<std::string>& GetEnabledMonsters() const { return mEnabledMonsters; }
    inline size_t GetAllMonstersNum() const { return mAllMonsters; }
    inline void IncrementAllMonstersNum() { ++mAllMonsters; } // TODO: licznik odblokowane/wszystkie

    bool EnableMonster(const CActorTemplate* templ);
    bool EnableMonster(const CEnemy* monster);
    bool EnableMonster(const std::string& templateName);

    void Load(const std::string& path);
};

#define gBestiary CSingleton<CBestiary>::GetSingleton()

#endif // __CBESTIARY_H__