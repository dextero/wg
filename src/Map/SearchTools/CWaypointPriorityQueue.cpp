#include "CWaypointPriorityQueue.h"
#include <algorithm>

CWaypoint * CWaypointPriorityQueue::Pop()
{
    CWaypoint * waypoint = mHeap.front();
    std::pop_heap(mHeap.begin(), mHeap.end(), CWaypointPriorityQueue::CTotalCostGreater());
    mHeap.pop_back();

    return waypoint;
}

void CWaypointPriorityQueue::Push(CWaypoint * waypoint)
{
    mHeap.push_back(waypoint);
    std::push_heap(mHeap.begin(), mHeap.end(), CWaypointPriorityQueue::CTotalCostGreater());
}

void CWaypointPriorityQueue::Update(CWaypoint * waypoint)
{
    for (Heap::iterator it = mHeap.begin(); it != mHeap.end(); ++it)
    {
        if ((*it)->GetHash() == waypoint->GetHash())
        {
            std::push_heap(mHeap.begin(), it + 1, CWaypointPriorityQueue::CTotalCostGreater());
            return;
        }
    }
}
