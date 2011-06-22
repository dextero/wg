#ifndef __CEFFECTMANAGER_H__
#define __CEFFECTMANAGER_H__

/**
 * Liosan, 30.03
 * Klasa przechowujaca logike aplikowania efektow do aktorow
 * na poziomie "globalnym" - aktorzy sami pamietaja wlasne
 * obiekty SAppliedEffect, natomiast nie potrafia ich obsluzyc. I dobrze.
 */

#include "../../Utils/CSingleton.h"
#include <limits>
#include "EEffectType.h"
#include "SEffectNode.h"
#include "CEffectSource.h"
#include "../Stats/EAspect.h"
#include "../../Utils/CXml.h"
#include "CAppliedEffect.h"
#include "CExecutionContext.h"
#include "CFocusObject.h"
#include "../Stats/EDamageType.h"

#include <SFML/System/Vector2.hpp>

#include <vector>
#include <map>
#include <ostream>

#define gEffectManager CEffectManager::GetSingleton()

class CEffectHandle;
class CPhysical;
class IDefferedLoadable;
class CExprParser;

using EffectTypes::EffectType;

class CEffectManager : public CSingleton<CEffectManager> {
    friend class CEffectLibrary;
private:
    std::vector<SEffectNode> effectNodes;
    std::vector<IDefferedLoadable*> mDelayedLoadables;
    std::vector<rapidxml::xml_node<>*> mDelayedLoadablesNodes;

    /* jak czesto efekt ma byc reaplikowany? 
     * np. trucizna mozliwie czesto, recast co 1.0s, a zmiana cechy wcale
     */
    float DefaultRecast(EffectType et);
    /* czy potrzebna jest jakakolwiek (1-sekundowa) dlugosc efektu?
     * niektore efekty sa bezsensowne (np. zmiana cechy) jesli sa natychmiastowe
     */
    bool RequiresDuration(EffectType et);
    /*
     */
    int Compile(int offset);
    /* przetworz efekt, tworzac obiekt SAppliedEfekt.
     * efekt zostanie albo wykonany od razu (perform(..)), albo
     * dodany do listy efektow aktora.
     */
    void Apply(CPhysical *physical, int offset, float duration, EffectSourcePtr source, ExecutionContextPtr context
        , FocusObjectPtr focus);

    bool ParseEffect(CXml &xml, xml_node<> *node);

	std::map<std::string,int> mTags;

	const SEffectNode &GetParamNode(int offset, EffectTypes::ParamType pt);
public:
    CEffectManager();
    virtual ~CEffectManager();

    /* wykonaj zmiany w aktorze zwiazane z tym efektem
     * moze byc wykonane wielokrotnie dla tego samego efektu
     * zasadniczo bedzie wywolywane co sekunde dla efektow trwajacych jakis czas.
     * Z reguly wywolywane przez aktora
     */
    void Perform(CPhysical *physical, CAppliedEffectPtr ae, float dt);

    // wykonaj na aktorze raz
    inline void ApplyOnce(CPhysical *physical, int offset, EffectSourcePtr source = EffectSourcePtr(NULL), 
            ExecutionContextPtr context = ExecutionContextPtr(NULL), FocusObjectPtr focus = FocusObjectPtr(NULL))
    {
        Apply(physical,offset,std::numeric_limits<float>::quiet_NaN(),source,context,focus);
    }
    // wykonaj na aktorze z okreslona dlugoscia trwania efektu
    inline void ApplyTimed(CPhysical *physical, int offset,float duration, EffectSourcePtr source = EffectSourcePtr(NULL), 
            ExecutionContextPtr context = ExecutionContextPtr(NULL), FocusObjectPtr focus = FocusObjectPtr(NULL))
    {
        Apply(physical,offset,duration,source,context,focus);
    }
    // wykonaj na potworze na stale
    inline void ApplyPermanent(CPhysical *physical, int offset, EffectSourcePtr source = EffectSourcePtr(NULL), 
            ExecutionContextPtr context = ExecutionContextPtr(NULL), FocusObjectPtr focus = FocusObjectPtr(NULL))
    {
        Apply(physical,offset,std::numeric_limits<float>::infinity(),source,context,focus);
    }

    /* funkcja powinna byc wywolana po skonczeniu czasu dzialania efektu
     * i po usunieciu go z listy appliedEffects.
     * zwykle nic nie robi, ale jest potrzebna przy modyfikacji aspektow aktorow:
     * wtedy wywola metode recalculateModifier
     */
    void Unapply(CPhysical *physical, CAppliedEffectPtr ae);
    /* przejdz po liscie efektow dotyczacych aktora, znajdz wszystkie efekty
     * typu betModify dotyczace aspektu aspekt, oblicz wypadkowa wartosc
     * i odpowiednio zmien wartosc tymczasowa tego aspektu.
     */
    void RecalculateModifier(CPhysical *physical, EAspect aspect);

    void PerformAt(const sf::Vector2f &pos, int offset, EffectSourcePtr source = EffectSourcePtr(NULL), 
        ExecutionContextPtr context = ExecutionContextPtr(NULL), FocusObjectPtr focus = FocusObjectPtr(NULL));

    CEffectHandle *LoadEffect(CXml &xml, xml_node<> *node);

    void Serialize(int offset, std::ostream &out, int indent);
};

#endif /*__CEFFECTAPPLIER_H__*/
