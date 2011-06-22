#ifndef __EASPECT_H__
#define __EASPECT_H__

/**
 * Liosan, 12.03
 * Aspekt opisuje pewna w miare stala, latwo opisywalna ceche potwora,
 * np. maksymalna predkosc, sile albo rozmiar
 */

#include <string>

namespace Aspects{
    const std::wstring aspectNames[] = { L"", L"level", L"scale", L"radius", L"mass", L"strength", L"speed", L"turnspeed",
        L"maxhp", L"maxmana", L"manaregen", L"intelligence", L"poisonous",
        L"physresist", L"fireresist", L"iceresist", L"lightningresist", L"poisonresist"};

    const int aspectCount = sizeof(aspectNames)/sizeof(std::wstring);

	// symbole specjalne - przydatne przy obliczaniu modyfikatorow w efektach logicznych
	const int invValue = 1 << (aspectCount + 1);
	const int logValue = 1 << (aspectCount + 2);
}

enum EAspect { // dodajac nowy aspekt, nalezy poprawic aspectNames
    aNoAspect = 0,
    aLevel = 1 << 0, // najpierw aNoAspect, potem aLevel, reszta dowolnie
    aScale = 1 << 1, // tylko chce moc z tego robic maski binarne ;)
	aRadius = 1 << 2,   // uwaga: aspekty, ktore nie powinny byc widoczne dla gracza
    aMass = 1 << 3,     // (w bestiariuszu) maja byc przed aStrength
    aStrength = 1 << 4,
    aSpeed = 1 << 5,
    aTurnSpeed = 1 << 6,
    aMaxHP = 1 << 7,
    aMaxMana = 1 << 8,
    aManaRegen = 1 << 9,
    aIntelligence = 1 << 10,
    aPoisonous = 1 << 11,
    aPhysResist = 1 << 12,
    aFireResist = 1 << 13,
    aIceResist = 1 << 14,
    aLightningResist = 1 << 15,
    aPoisonResist = 1 << 16
};

EAspect parseAspect(const std::wstring &s);

int parseModifiers(const std::wstring &s);

std::wstring aspectName(EAspect a);

std::wstring availableAspects();

#endif /*__EASPECT_H__*/
