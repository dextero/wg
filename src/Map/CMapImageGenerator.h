  #pragma once
#include <SFML/Graphics.hpp>
#include "CWorldGraph.h"
#include "../Utils/CSingleton.h"

#define gMapImageGenerator Map::CMapImageGenerator::GetSingleton()

namespace Map
{

enum EMapBorder
{
	East,
	West,
	North,
	South,
	None,
};

class CWorldGraphMapEx;
class CWorldGraphExitEx {
public:
    EMapBorder fromBorder;
	EMapBorder toBorder;
    std::string toEntry;
    bool blocked;

	CWorldGraphMapEx* FirstParent;
	CWorldGraphMapEx* SecondParent;


	// Dla generowania obrazka mapy
	bool PathCreated;
	sf::Vector2f Point1;
	sf::Vector2f Point2;
};

class CWorldGraphMapEx {
	public:

	~CWorldGraphMapEx();

    std::string id;
    std::vector<CWorldGraphExitEx*> exits;
    std::string boss;
	std::string scheme;
    bool final;
    sf::Vector2f mapPos;

	bool IsCrossroad();
	bool PathsCreated();
};

class CWorldGraphEx {
	public:
    std::string startingMap;
    std::string startingRegion;
    std::map<std::string, CWorldGraphMapEx> maps;
	std::map<std::string, std::vector<std::string>> schemes;
	std::map<std::string, std::vector<std::string>> schemeConvexHulls;
	CWorldGraphEx() { };
    void BuildFromWorldGraph();
};


//
//
//class CQuadTreeNode
//{
//public:
//	std::vector<CQuadTreeNode> Childs;
//	CQuadTreeNode* Parent;
//};


class CMapImageGenerator: public CSingleton<CMapImageGenerator>
{
public:
	CMapImageGenerator(void);
	~CMapImageGenerator(void);

	void GenerateMapImage();

	static const int ImageWidth = 1024;
	static const int ImageHeight = 768;

private:	

	sf::Image BackgroundImg;
	sf::Sprite Background;
	sf::Image WorldMapMarkImg;
	sf::Sprite WorldMapMark;
	sf::Image RoadMarkImg;
	sf::Sprite RoadMark;
	
	sf::RenderWindow* RenderWindow;
	sf::Vector2f Proportion;

	float BezierPolynomial(float t, float& Start, float& CP1, float& CP2, float& End);
	sf::Vector2f GetBezierPoint(float t, sf::Vector2f& Start, sf::Vector2f& Point1, sf::Vector2f& Point2, sf::Vector2f& End);
	sf::Vector2f GetBezierPoint(int numberOfSegments, int segmentIndex, sf::Vector2f& Start, sf::Vector2f& Point1, sf::Vector2f& Point2, sf::Vector2f& End);
	void GeneratePathBetweenTwoExits(sf::Vector2f& Location1, sf::Vector2f& Location2, CWorldGraphExitEx &Exit);
	void GeneratePaths(CWorldGraphExitEx &Exit);

};
}