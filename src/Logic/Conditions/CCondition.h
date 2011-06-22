#ifndef __CCONDITION_H__
#define __CCONDITION_H__

/**
 * Liosan, 21.05.09
 * Klasa reprezentujaca ogolny, latwo-sprawdzalny, wczytywalny z XMLa warunek
 * Moze byc zastosowana do zmieniania planszy, otwierania bram, uruchamiania pulapek itp
 */

#include <string>
#include <ostream>

#include "../../Utils/CXml.h"
#include "../../Utils/Memory/CSafePtr.h"

class CDynamicRegion;

class CCondition{
public:
    enum ConditionType{
        ctNone = 0, // nigdy nie jest triggerowany
        ctClear = 1, // wszyscy aktorzy podpadajacy pod filtr usunieci
        ctTimeElapsed = 2, // uplynal pewien czas na aktualnej mapie
        ctRegionTriggered = 3, // konkretny region na mapie jest aktywowany
        ctEnemyKilled = 4 // zabito konkretnego potwora
    };

    static ConditionType ParseConditionType(const std::string &str);

    // operator, ktory da sie nalozyc na warunek
    // uzywane tylko w metodzie SpecificCheck
    enum SpecificCheckType{ 
        sctIs = 0,
        sctIsnt = 1,
        sctOnce = 2,
        sctNever = 3
    };

    static int ParseSpecificCheckType(std::string &str);
protected:
    static const int ConditionTypeCount;
    static const int SpecificCheckTypeCount;
    static std::string ConditionTypeNames[];
    static std::string SpecificCheckTypeNames[];

    ConditionType mType;

    int mParam;
    float mfParam;
    std::string msParam;
    Memory::CSafePtr<CDynamicRegion> mRegionParam;

    SpecificCheckType mSct;

    bool mWasTriggered;
    bool mCurrentState;

    void LoadParam(const std::string &str);
    void SerializeParam(std::ostream &out, int indent = 0);
public:
    CCondition();
    virtual ~CCondition();

    void Register();
    void Unregister();
    void Reset();
    void ClearTriggered();

    inline ConditionType GetType(){ return mType; }
    void SetType(ConditionType type);
    void SetSpecificCheckType(int sct);

    inline void SetParam(int p){ mParam = p; }

    void Load(CXml &xml, rapidxml::xml_node<> *node);
    void Serialize(std::ostream &out, int indent = 2);

    bool IsTriggered();
    inline bool WasEverTriggered(){
        if (mWasTriggered)
            return true;
        else
            return mWasTriggered = IsTriggered();
    }

    bool SpecificCheck();
    bool SpecificCheck(int sct);

    // metody wolane przez CConditionManager
    
    // dt - na przyszlosc, moze niektore conditiony beda sprawdzane raz na sekunde? dla wydajnosci
    void TryCheck(float dt);
    void SwitchIndex(int i);
private:
    void Check();
    int mIndex;
};


#endif /*__CCONDITION_H__*/
