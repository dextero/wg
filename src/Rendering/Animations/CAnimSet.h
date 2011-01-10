#ifndef __CANIMSET_H__
#define __CANIMSET_H__

#include <string>
#include <vector>
#include "../../Utils/CXml.h"

/**
 * Liosan, 02.04.09
 * Klasa reprezentujaca zestaw animacji aktora.
 * Liosan, 19.05.09
 * przerabiam na slownik uchwytow, dodaje EAnimType - nazwa animacji
 * przenosze tez tutaj parsowanie, bo zrobilo sie troche skomplikowane
 */
struct SAnimation;

class CAnimSet
{
public:
    CAnimSet();

    // EAnimClass
    enum EAnimClass{
        acDefault = 0,
        acMove = 1,
        acAttack = 2,
        acDie = 3,
        acStrafe
    }; // dodajac nowy, nalezy poprawic w .cpp mAnimClassNumber i mAnimClassNames

    // parsuje na podstawie nazw (zwykle zachowanie) lub inta 
    static int ParseAnimClass(std::string &input);
private:
    static int mAnimClassNumber;
    static std::string mAnimClassNames[];
public:
    // jesli podamy wiecej niz mAnimClassNumber,
    // zostanie to zapisane mimo wszystko - pod wlasnie tym kluczem.
    // uwaga - to jest vector, klucze niech beda male :)
    void SetAnimation(int ac, SAnimation *animPtr);
    void SetAnimation(int ac, std::string &animName);

    std::string *GetAnimName(int ac);
    SAnimation *GetAnim(int ac);

    // metody pomocniczne
    inline SAnimation *GetDefaultAnim(){ return GetAnim(acDefault); }
    inline SAnimation *GetMoveAnim(){ return GetAnim(acMove); }
    inline SAnimation *GetAttackAnim(){ return GetAnim(acAttack); }
    inline SAnimation *GetDieAnim(){ return GetAnim(acDie); }
public:
    class CNameAnimPair{
    public:
        CNameAnimPair(): name(""), anim(0), isReady(false){}
        CNameAnimPair(std::string &name):name(name),anim(0),isReady(false){}

        void SetAnim(SAnimation *anim){
            this->anim = anim;
            isReady = true;
        }

        std::string name;
        SAnimation *anim;
        bool isReady;
    };
    void Parse(CXml &xml,rapidxml::xml_node<> *root = 0);
    inline const std::vector<CNameAnimPair> &GetAnims() const { return mAnims; }
private:
    std::vector<CNameAnimPair> mAnims;
};

#endif// __CANIMSET_H__

