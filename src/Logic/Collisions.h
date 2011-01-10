#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include <SFML/System/Vector2.hpp>

class CPhysical;

namespace Collisions
{
	/*
	*	Dla wszystkich funkcji poza CheckAllCollisions zakladamy ze A->GetCategory() < B->GetCategory()
	*
	*	Czy nalezy sprawdzac kolizje miedzy obiektami wskazanych typow 
	*/

	bool NeedToCheckCollision( CPhysical* physicalA, CPhysical* physicalB );

	/* 
	*	Odpowiedz kolizji zwiazana z logika; wszelkie obrazenia, zmiany AI etc.
	*	makePhysicalResponse - jesli jest rowne true zostanie wykonana odpowiedz fizyczna.
	*/

	void LogicalResponse( CPhysical* physicalA, CPhysical* physicalB, bool& makePhysicalResponse );

	/* 
	*	Odpowiedz kolizji zwiazana z fizyka; rozdzielenie kolidujacych obiektow
	*	separateVector - wektor o jaki nalezy przesunac physical B zeby nie kolidowal z physicalem A
	*/

	void PhysicalResponse( CPhysical* physicalA, CPhysical* physicalB, const sf::Vector2f& separateVector );

	/* 
	*	Sprawdz czy wystapila kolizja i odpowiednio zareaguj
	*/

    void CheckCollision( CPhysical* _physicalA, CPhysical* _physicalB );

    void CheckAllCollisions();
}

#endif //__COLLISIONS_H__
