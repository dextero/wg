#ifndef __CANIMSET_H__
#define __CANIMSET_H__

#include <string>
#include <map>
#include "../../Utils/CXml.h"

/**
 * Liosan, 02.04.09
 * Klasa reprezentujaca zestaw animacji aktora.
 */
struct SAnimation;

class CAnimSet
{
public:
    CAnimSet();

    static std::string acDefault;
    static std::string acMove;
    static std::string acAttack;
    static std::string acDeath;
    static std::string acStrafe;

    void SetAnimation(const std::string & ac, SAnimation * animPtr);
    void SetAnimation(const std::string & ac, const std::string & animName);

    const std::string & GetAnimName(const std::string & ac);
    SAnimation * GetAnim(const std::string & ac);

    // metody pomocniczne
    inline SAnimation * GetDefaultAnim(){ return GetAnim("default"); }
    inline SAnimation * GetMoveAnim(){ return GetAnim("move"); }
    inline SAnimation * GetAttackAnim(){ return GetAnim("attack"); }
    inline SAnimation * GetDeathAnim(){ return GetAnim("death"); }
public:
    class CNameAnimPair{
    public:
        CNameAnimPair() : name(""), anim(0), isReady(false) {}
        CNameAnimPair(const std::string & name) : name(name), anim(0), isReady(false) {}

        void SetAnim(SAnimation *anim){
            this->anim = anim;
            isReady = true;
        }

        std::string name;
        SAnimation * anim;
        bool isReady;
    };

    typedef std::map<std::string, CNameAnimPair> NameAnimPairMap;
    void Parse(CXml &xml, rapidxml::xml_node<> *root = 0);
//    inline const std::vector<CNameAnimPair> & GetAnims() const { return mAnims; }
    inline const NameAnimPairMap & GetAnims() const { return mAnims; }
private:
    NameAnimPairMap mAnims;
};

#endif// __CANIMSET_H__

