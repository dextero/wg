// #include "CTravelAgent.h"
// #include "../../Utils/MathsFunc.h"
// 
// CTravelAgent::CTravelAgent(int movementCost) :
//     mMovementCost(movementCost)
// {}
// 
// int
// CTravelAgent::GetTravelCost(const sf::Vector2i & start, const sf::Vector2i & goal) const
// {
//     return Maths::ManhattanDistance(start, goal) * mMovementCost;
// }
// 
// int
// CTravelAgent::GetMovementCost() const
// {
//     return mMovementCost;
// }