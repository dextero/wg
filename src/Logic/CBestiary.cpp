#include "CBestiary.h"
#include "CActor.h"
#include "Factory/CActorTemplate.h"
#include "CEnemy.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Commands/CCommands.h"
#include "../Utils/StringUtils.h"

#include <sstream>

template<> CBestiary* CSingleton<CBestiary>::msSingleton = NULL;


void CBestiary::Save()
{
    if (mFilePath.size() == 0)
        return;

    std::stringstream ss;
    for (std::set<std::string>::iterator it = mEnabledMonsters.begin(); it != mEnabledMonsters.end(); ++it)
        ss << "bestiary-add " << it->c_str() << "\n";

    FILE *file = fopen(mFilePath.c_str(), "w");
    if (file)
    {
        fputs(ss.str().c_str(), file);
        fclose(file);
    }
}

bool CBestiary::EnableMonster(const CActorTemplate* templ)
{
    if (!templ)
    {
        fprintf(stderr, "Cannot enable monster in bestiary, its template is NULL\n");
        return false;
    }
    else if (mEnabledMonsters.find(templ->GetFilename()) == mEnabledMonsters.end())
    {
        mEnabledMonsters.insert(templ->GetFilename());
        //fprintf(stderr, "Added to bestiary: %s\n", templ->GetFilename().c_str());

        Save();

        return true;
    }
    //fprintf(stderr, "%s - already in bestiary\n", templ->GetFilename().c_str());
    return false;
}

bool CBestiary::EnableMonster(const CEnemy *monster)
{
    return EnableMonster(dynamic_cast<CActorTemplate*>(monster->GetTemplate()));
}

bool CBestiary::EnableMonster(const std::string& templateName)
{
    return EnableMonster(dynamic_cast<CActorTemplate*>(gResourceManager.GetPhysicalTemplate(templateName)));
}

void CBestiary::Load(const std::string& path)
{
    if (path.size() > 0)
    {
        mFilePath = path;
        gCommands.ParseCommand(L"exec " + StringUtils::ConvertToWString(path));
    }
}
