#ifndef __CACTIONSLIST_H__
#define __CACTIONSLIST_H__

#include <list>
#include <string>
#include "Utils/CSingleton.h"

namespace Actions
{
    enum EActionType
    {
        none,
        changeTile,
        deleteDoodah,
        deletePhysical,
        deleteRegion,
        placeDoodah,
        placePhysical,
        placeRegion,
        modifySomething    // zmiana jakiegos obiektu = najpierw usuwamy, potem przechodzimy w stawianie
    };
}

struct STileChanged
{
    int oldMasks[4], newMasks[4];
    STileChanged()
    {
        for (int i = 0; i < 8; ++i)
            oldMasks[i] = 0;
    }
    //STileChanged(int old1, 
};

struct SAction
{
    Actions::EActionType type;
    float x, y, rot, scale;
    int z;
    std::string code, codeNew;
    STileChanged tileChanged;

    SAction(Actions::EActionType _type, float _x, float _y, int _z, float _rot, float _scale, const std::string _code = "", const std::string _codeNew = "", STileChanged _tileChanged = STileChanged()):
        type(_type), x(_x), y(_y), z(_z), rot(_rot), scale(_scale), code(_code), codeNew(_codeNew), tileChanged(_tileChanged) {}
    SAction(const SAction& copy)
    {
        type = copy.type;
        x = copy.x;
        y = copy.y;
        rot = copy.rot;
        scale = copy.scale;
        z = copy.z;
        code = copy.code;
        codeNew = copy.codeNew;
        tileChanged = copy.tileChanged;
    }

    static SAction None() { return SAction(Actions::none, 0.f, 0.f, 0, 0.f, 0.f); }
    static SAction ChangeTile(float x, float y, const std::string& oldCode, const std::string& newCode, STileChanged tileChanged) { return SAction(Actions::changeTile, x, y, 0, 0.f, 0.f, oldCode, newCode, tileChanged); }
    static SAction DeleteDoodah(float x, float y, int z, float rot, float scale, const std::string& path) { return SAction(Actions::deleteDoodah, x, y, z, rot, scale, path); }
    static SAction PlaceDoodah(float x, float y, int z, float rot, float scale, const std::string& path) { return SAction(Actions::placeDoodah, x, y, z, rot, scale, path); }

    // code == MapObject code; name == id
    static SAction DeletePhysical(float x, float y, float rot, const std::string& code, const std::string& name) { return SAction(Actions::deletePhysical, x, y, 0, rot, 1.f, code, name); }
    // code == MapObject code; name == id
    static SAction PlacePhysical(float x, float y, float rot, const std::string& code) { return SAction(Actions::placePhysical, x, y, 0, rot, 1.f, code); }

    // code == uniqueId, codeNew == region.Serialize()
    static SAction DeleteRegion(float x, float y, float scale, const std::string& id, const std::string& content) { return SAction(Actions::deleteRegion, x, y, 0, 0.f, scale, id, content); }
    // j/w
    static SAction PlaceRegion(float x, float y, float scale, const std::string& id, const std::string& content) { return SAction(Actions::placeRegion, x, y, 0, 0.f, scale, id, content); }

    static SAction Reverse(SAction action);
};

// klasa odpowiadajaca za pamietanie kolejki undo-redo
#define gActionsList    CActionsList::GetSingleton()

const int UNDO_MAX = 100;

class CActionsList: public CSingleton<CActionsList>
{
private:
    std::list<SAction> mDone, mUndone;

public:
    CActionsList() {};
    ~CActionsList() {};

    void Push(SAction action);
    SAction Undo();
    SAction Redo();
    SAction& Top() { return mDone.back(); };
    inline void Clear() { mDone.clear(); mUndone.clear(); }
};

#endif // __CACTIONSLIST_H__