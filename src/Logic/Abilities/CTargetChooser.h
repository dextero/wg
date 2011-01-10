#ifndef __CTARGETCHOOSER_H__
#define __CTARGETCHOOSER_H__

/**
 * Liosan, 08.07:
 * klasa umozliwiajaca automatyczne celowanie
 * wybiera cel na podstawie kilku kryteriow
 */

#include "../../Utils/CXml.h"
#include "../Effects/CExecutionContext.h"
#include "../Effects/CEffectSource.h"
#include "../Effects/CComputedValue.h"

#include <SFML/System/Vector2.hpp>
#include <vector>

class CDetector;
class CPhysical;
class CActor;

class CTargetChooser{
private:
    CComputedValue range;
    int filter;
    CComputedValue angle;
	int aimAtGround;
	CComputedValue count;
	int deadBodies;

    std::vector<CPhysical*> work;
public:
    CTargetChooser();
    ~CTargetChooser();

    void Load(CXml &xml, rapidxml::xml_node<> *node);

    // wykonuje efekt na jakims physicalu w zasiegu
    void Perform(CPhysical *emiter, int offset, EffectSourcePtr source = NULL, ExecutionContextPtr context = NULL);
	// wybierz jakis cel; ignoruje count i zwraca tylko 1 cel
	CPhysical *Choose(CPhysical *emiter, ExecutionContextPtr context = NULL);
	// wybierz count roznych celi (lub mniej, jesli tyle sie nie da)
	void ChooseMultiple(CPhysical *emiter, std::vector<CPhysical*> &result, ExecutionContextPtr context = NULL);
private:
    // wybierz quasi-losowo jeden z physicali z listy
    // zakladamy, ze wszystkie spelniaja range i filter
	void ChooseFromList(CPhysical *emiter, const std::vector<CPhysical*> &observed, std::vector<CPhysical*> &result, CPhysical *taboo = NULL, ExecutionContextPtr context = NULL);
    // wybierz jakis kawalek terenu
    sf::Vector2f ChoosePos(CPhysical *emiter, ExecutionContextPtr context, bool &outOK);

	inline bool GetAimAtGround(){ return aimAtGround > 0; }
};

#endif /* __CTARGETCHOOSER_H__ */

