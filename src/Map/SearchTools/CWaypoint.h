#ifndef _WAYPOINT_
#define _WAYPOINT_

#include <SFML/System/Vector2.hpp>
#include <vector>

class CWaypoint
{
public:
    virtual ~CWaypoint();
    
    const CWaypoint * GetParent() const     { return mParent; }

    bool IsOnOpen() const                   { return mIsOnOpen; }
    bool IsOnClosed() const                 { return mIsOnClosed; }

    void SetOnOpen(bool onOpen)             { mIsOnOpen = onOpen; }
    void SetOnClosed(bool onClosed)         { mIsOnClosed = onClosed; }

    int GetCostFromStart() const            { return mCostFromStart; }
    int GetTotalCost() const                { return mCostFromStart + mCostToGoal; }              

    void Update(int costFromStart, int costToGoal, CWaypoint * parent);

    bool IsInUse() const                    { return mIsInUse; }
    void Clear()                            { mIsInUse = false; }
    
    bool operator==(const CWaypoint & other);

    virtual int GetHash() const = 0;        //! Zwraca unikalny identyfikator tego waypointa
    virtual std::vector< CWaypoint * > GetNeighbours() const = 0;
    
    virtual CWaypoint * Clone() const = 0;
    
protected:
    CWaypoint();
    CWaypoint(int costFromStart, int costToGoal, CWaypoint * parent);

private:
    int mCostFromStart;                 //! Koszt dojscia do tego waypointa
    int mCostToGoal;                    //! Szacowany koszt do celu
    CWaypoint * mParent;                //! Waypoint z ktorego tu przyszlismy
    
    bool mIsOnOpen;                     //! Czy waypoint jest na liscie open
    bool mIsOnClosed;                   //! Czy waypoint jest na liscie closed
    
    bool mIsInUse;                      //! Czy dany waypoint jest uzyty podczas danego szukania
};

//////////////////////////////////////////////////////////////////////////

// template < class T >
// class CFindWaypointByIndex
// {
// public:
//     CFindWaypointByIndex(const T & index) : mIndex(index) {}
//         
//     bool operator()(const CWaypoint * waypoint) const   
//                                             { return mIndex == waypoint->GetIndex(); }
//     
// private:
//     T mIndex;
// };

//////////////////////////////////////////////////////////////////////////

#endif // _WAYPOINT_
