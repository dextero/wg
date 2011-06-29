#include <cstddef>
#include "CWaypoint.h"

CWaypoint::CWaypoint(
    int costFromStart, int costToGoal, CWaypoint * parent) :
    mCostFromStart(costFromStart), mCostToGoal(costToGoal),
    mParent(parent), mIsOnOpen(true), mIsOnClosed(false), mIsInUse(false)
{}

CWaypoint::CWaypoint() :
    mCostFromStart(0), mCostToGoal(0), mParent(NULL),
    mIsOnOpen(false), mIsOnClosed(false), mIsInUse(false)
{}

CWaypoint::~CWaypoint()
{}

// const CWaypoint * CWaypoint::GetParent() const
// {
//     return mParent;
// }
//
// bool CWaypoint::IsOnOpen() const
// {
//     return mIsOnOpen;
// }
//
// bool CWaypoint::IsOnClosed() const
// {
//     return mIsOnClosed;
// }
//
// int CWaypoint::GetCostFromStart() const
// {
//     return mCostFromStart;
// }
//
// int CWaypoint::GetTotalCost() const
// {
//     return mCostFromStart + mCostToGoal;
// }
//
// void
// CWaypoint::SetOnOpen(bool onOpen)
// {
//     mIsOnOpen = onOpen;
// }
//
// void
// CWaypoint::SetOnClosed(bool onClosed)
// {
//     mIsOnClosed = onClosed;
// }

void
CWaypoint::Update(int costFromStart, int costToGoal, CWaypoint * parent)
{
    mCostFromStart = costFromStart;
    mCostToGoal = costToGoal;
    mParent = parent;

    mIsOnOpen = true;
    mIsOnClosed = false;
    mIsInUse = true;
}

bool CWaypoint::operator==(const CWaypoint & other)
{
    return GetHash() == other.GetHash();
}

//////////////////////////////////////////////////////////////////////////

// CWaypoint::CFindWaypointByIndex::CFindWaypointByIndex(const sf::Vector2i & index) :
//     mIndex(index)
// {}
//
// bool CWaypoint::CFindWaypointByIndex::operator()(const CWaypoint * waypoint) const
// {
//     return mIndex == waypoint->GetIndex();
// }
