#ifndef __CACHIEVEMENTMANAGER_H__
#define __CACHIEVEMENTMANAGER_H__

#include "../../Utils/CSingleton.h"
#include <vector>
#include <string>

#define gAchievementManager CAchievementManager::GetSingleton()

class CAchievementManager: public CSingleton<CAchievementManager>
{
public:
    struct SAchievement
    {
    public:
        std::string name;
        std::string desc;
        std::string image;
        bool completed;

    private: // nikt nie ma prawa tworzyc achievementow, oprocz managera
        SAchievement(): name(""), desc(""), image(""), completed(false) {}

        friend class CAchievementManager;
    };

private:
    std::vector<SAchievement> mData;
    std::string mLoadedFilePath;

public:
    void Load(const std::string& file);
    void Save(const std::string& file) const;

    // ustawia wszedzie completed na false
    void Clear();

    // ustawia completed na true
    void AddAchievement(size_t argc, const std::vector<std::wstring>& argv);

    inline const std::vector<SAchievement>& GetAchievements() const { return mData; }
};

#endif // __CACHIEVEMENTMANAGER_H__