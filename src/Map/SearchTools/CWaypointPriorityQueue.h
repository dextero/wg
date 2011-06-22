#ifndef __CWaypointPriorityQueue_h__
#define __CWaypointPriorityQueue_h__

#include "CWaypoint.h"
#include <vector>
#include <algorithm>

    /** \brief Zoptymalizowana klasa do kolejkowania waypointow.
     *         Waypointy ustawione sa w kolejnosci rosnacej wzgledem lacznego
     *         kosztu dotarcia do celu
     */
class CWaypointPriorityQueue
{
public:
    	/** /brief Zwraca CWaypoint o najnizszym lacznym koszcie dotarcia do celu.
    	 *  /return Waypoint o najnizszym koszcie dotarcia do celu. Ty odpowiadasz za zniszczenie tego obiektu.
    	 */
    CWaypoint * Pop();
    
    	/** /brief Dodaje waypointa w odpowiednie miejsce do kolejki.
    	 *  /param Nowy waypoint. Ty odpowiadasz za zniszczenie tego obiektu.
    	 */
    void Push(CWaypoint * waypoint);
    
    	/** /brief Uaktualnia dany waypoint przesuwajac go w kolejce jezeli potrzeba.
    	 *  /param waypoint Wskaznik na waypoint, ktory chcemy uaktualnic.
    	 */
    void Update(CWaypoint * waypoint);
    
    	/** /brief Sprawdza, czy kolejka jest pusta
    	 */
    bool IsEmpty() const                { return mHeap.empty(); }
    
    	/** /brief Czysci kolejke. Waypointy nie sa niszczone
    	 */
    void Clear()                        { mHeap.clear(); }  

private:
    class CTotalCostGreater;

    typedef std::vector< CWaypoint * > Heap;
    Heap mHeap;
};

//////////////////////////////////////////////////////////////////////////

    /** \brief Klasa do porownywania laczenego kosztu do celu dwoch waypointow
     */
class CWaypointPriorityQueue::CTotalCostGreater
{
public:
    	/** /brief Sprawdza, ktory waypoint ma wiekszy koszt dotarcia do celu
    	 *  /return true jezeli koszt dotarcia do celu first jest wiekszy niz second
    	 */
    bool operator()(CWaypoint * first, CWaypoint * second)
        {  return first->GetTotalCost() > second->GetTotalCost(); }
};

//////////////////////////////////////////////////////////////////////////

#endif // __CWaypointPriorityQueue_h__
