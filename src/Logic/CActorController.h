#ifndef CACTOR_CONTROLLER_H_
#define CACTOR_CONTROLLER_H_

#include <string>
#include "Effects/CEffectSource.h"

class CActor;

class CActorController {
protected:
    CActor *mActor;
	int mStrafe;
	int mMove;
    float mTrueRot;
public:
    CActorController(CActor *actor);
    virtual ~CActorController();

    // MoveBy ustawia stan i predkosc
    void SetSpeed(float value);
    // TurnBy ustawia stan i wykonuje obrot
    void TurnBy(float value);
	// Pobiera aktualny obrót
	float GetTrueRot();
    void SetTrueRot(float rot) { mTrueRot = rot; }
		// Ustawia stan i wykonuje strafe
	void SetStrafe(int value);

	void SetMove(int value);	
    virtual void Update(float dt);
    // metoda informujaca o otrzymaniu obrazen z danego zrodla
    // wywolywana przez CActor, moze sluzyc do realizacji
    // "grrr, zaatakuje gracza, ktory mnie zaatakowal"
    virtual void GotDamageFrom(float dmg, EffectSourcePtr source);
};

#endif /*CACTOR_CONTROLLER_H_*/
