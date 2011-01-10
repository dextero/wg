#ifndef CREGION_H_
#define CREGION_H_

#include <string>
#include <vector>

#include "../CPhysical.h"
#include "../../Utils/Memory/CSafePtrable.h"

class CPlayer;
class CCondition;
class CEffectHandle;

class CDynamicRegion: public CPhysical, public Memory::CSafePtrable<CDynamicRegion> {
    friend class CPhysicalManager;
protected:
    CDynamicRegion(const std::wstring& uniqueId);
    virtual ~CDynamicRegion();

    // wskazniki na stringa w CRegionDescriptorze; 
    // przy zmianie planszy wywola sie destruktor regionu, 
    // nie mozemy dopuscic do utraty stringa
    std::string *mNextMap;
    std::string *mNextMapRegion;

    bool mDisplay; // wyswietlac jako prostokat, azali nie?

    CCondition *mCond;

    float mTriggerTime;

    CEffectHandle *mEffectOnEnter;

    float mRotation;

    int mDescriptorId;
    // edytor - do zmiany koloru po najechaniu mysza
    bool mSelectedToErase;

public:
    virtual void Update(float dt);

    // gracz stanal na regionie
    void Trigger(CPlayer *player);

    inline std::string *GetNextMap(){ return mNextMap; }
    inline void SetNextMap(std::string *nextMap){ mNextMap = nextMap; }
    
    inline std::string *GetNextMapRegion(){ return mNextMapRegion; }
    inline void SetNextMapRegion(std::string *nextMapRegion){ mNextMapRegion = nextMapRegion; }

    inline void SetDisplay(bool display) { mDisplay = display; }

    inline CCondition* GetCondition(){ return mCond; }
    void SetCondition(CCondition *cond);

    bool IsTriggered();

    void SetEffectOnEnter(CEffectHandle *effect);

    void SetRotation(float newValue) { mRotation = newValue; }
    float GetRotation() { return mRotation; }

    void SetScale(float newValue) { SetBoundingRect(sf::Vector2f(newValue, newValue)); }
    float GetScale() { return mRectSize.x; }

    void SetDescriptorId(int id) { mDescriptorId = id; }
    int GetDescriptorId() { return mDescriptorId; };

    void SetSelectedToErase(bool selected) { mSelectedToErase = selected; }
};

#endif /*CREGION_H_*/
