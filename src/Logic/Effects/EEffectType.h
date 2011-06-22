#ifndef __EEFFECTTYPE_H__
#define __EEFFECTTYPE_H__

#include <string>
#include <vector>

/**
 * Liosan, 28.05 - generyczny opis parametrow efektu; patrz .cpp
 *
 * Liosan, 15.03
 * rodzaj efektu - potrzebny przy przetwarzaniu wektorow EffectNode w Effect-ach
 *
 * dodajac rodzaj efektu, nalezy _koniecznie_ dodac obsluge w Effect::compile
 * i EffectApplier::perform/apply, a takze dodac je do effectNames
 */

namespace EffectTypes{
    const int metaEffectCount = 11;

    enum EffectType {
        etNoEffect = 0,
        // "meta"-efekty, zmieniajace sposob dzialania innych
        etTimedEffect = 1, // efekt trwajacy jakis czas
        etRecastableEffect = 2, // efekt trwajacy jakis czas, ktory bedzie recastowany (na tego samego aktora) w zmieniony sposob
        etAreaEffect = 3, // zastosowanie jakiegos efektu do wszystkich aktorow na pewnym obszarze
        etChanceEffect = 4, // wykonanie efektu z pewnym prawdopodobienstwem
        etGraphicalEffect = 5, // efekt graficzny
        etEffectList = 6, // lista efektow
        etDelayedEffect = 7, // efekt wykonujacy sie po jakims czasie
        etRetarget = 8,
        etNonCumulative = 9, // efekt niekumulajacy - przy aplikacji usuwane sa wszystkie poprzednie instancje
        // (zeby non-cumulative dzialal, sub-efekt musi byc timed albo recastable)
        etCheckLevel = 10, // efekt wykonywane pod warunkiem, ze poziom umiejetnosci jest odpowiedni
		etRay = 11, // zastosowanie efektu do wszystkich aktorow kolidujacych z promieniem wyslanym od zrodla
        // "bazowe" efekty, bezposrednio wplywajace na aktora
        betDamage = metaEffectCount + 1,
        betHeal = metaEffectCount + 2,
        betModify = metaEffectCount + 3, // zmiana aspektu
        betAIState = metaEffectCount + 4,
        betRestoreMana = metaEffectCount + 5,
        betDrainMana = metaEffectCount + 6,
        betConsole = metaEffectCount + 7,
        betBullet = metaEffectCount + 8,
        betStun = metaEffectCount + 9,
        betKnockback = metaEffectCount + 10,
        betLinkedGraphicalEffect = metaEffectCount + 11,
		betJump = metaEffectCount + 12,
        betSummon = metaEffectCount + 13,
		betInject = metaEffectCount + 14,
		betSound = metaEffectCount + 15,
        betTransferLife = metaEffectCount + 16,
        betTransferMana = metaEffectCount + 17,
		betAnnihilate = metaEffectCount + 18,
		betHealthcast = metaEffectCount + 19,
		betIncreaseNextAbilityLevel = metaEffectCount + 20,
        betHealTarget = metaEffectCount + 21
    };

    // typy parametrow efektow
    enum ParamType { // <0, zeby rozrozniac od EffectType
        ptBadParam = -1,
        ptSubEffect = -2,
        ptTime = -3,
        ptRecast = -4,
        ptArea = -5,
        ptFilter = -6,
        ptChance = -7,
        ptName = -8, // string interpretowany kontekstowo, np. przy graphical-effect bedzie to EffectHandle
        ptAmount = -9,
        ptAspect = -10,
        ptText = -11,
        ptBulletEmitter = -12,
        ptTargetChooser = -13,
		ptOffsetX = -14,
		ptOffsetY = -15,
        ptDamageType = -16,
        ptSummoner = -17,
		ptTree = - 18,
		ptCode = -19,
        ptShowAmount = -20 // latajace cyferki w leczeniu
    };
    void BuildData();

    std::wstring &effectName(EffectType e);
    EffectType parseEffect(std::wstring &namem,const std::string &filename = "");

    std::string &paramName(ParamType pt);
    int paramOffset(EffectType et, ParamType pt);
    int totalStaticOffset(EffectType et);
    std::vector<ParamType> &availableParams(EffectType et);
    bool hasParam(EffectType et,ParamType pt);

	// metoda sluzaca do wyswietlenia posiadanych informacji o skladni i parametrach XMLi efektow
	void PrintOutSyntax();
}

#endif /*__EEFFECTTYPE_H__*/
