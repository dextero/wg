#ifndef __CCALLBACKDISPATCHER_H__
#define __CCALLBACKDISPATCHER_H__

/**
 * Liosan, 10.05.09
 * Klasa obslugujaca wykonywanie callbackow przy smierci obiektu
 */

#include <vector>

class CPhysical;

class CCallbackDispatcher{
public:
    CCallbackDispatcher(CPhysical *attached);
	/* sender - umierajacy aktor, 
	 * callee - obiekt na ktorym jest wywolywana funkcja,
	 * data - rozne, konkretne dane (przykladowo float - nie musi byc wskaznik, mozna rzutowac)
	 */
	typedef void (*EventCallbackFunc)(CPhysical *sender, void *callee, void *data);
	
	// rejestracja funkcji, ktora zostanie wywolana przy smierci aktora
	void RegisterEventDeath(EventCallbackFunc func, void *callee, void *data);
	// jesli obiekt rejestrujacy sie sam zostanie zniszczony, nalezy go odrejestrowac
	void UnregisterCallee(void *callee);
    // rozsyla callbacki
    void Dispatch();
private:
    // do ktorego physicala jest obiekt "przyczepiony" ?
    CPhysical *mAttached;

	struct SRegisteredFunc{
		EventCallbackFunc func;
		void *callee;
		void *data;
	};

	void ExecuteCallback(SRegisteredFunc &func);

	/* TODO: czy to nie jest strasznie niewydajne?
	 * moze jeden wspoldzielony wektor, i tam zrobic liste?
	 * przechodzenie tych callbackow jest dosc rzadkie, 
	 * a trzymanie wektora w kazdym goblinie jest z kolei drogie... nie?
	 */
	std::vector<SRegisteredFunc> mCallbacks;
};

#endif /*__CCALLBACKDISPATCHER_H__*/
