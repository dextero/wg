#include "CMapImageGenerator.h"
#include "CMapManager.h"
#include "../CGame.h"
#include <math.h>
#include <stack>

template<> Map::CMapImageGenerator* CSingleton<Map::CMapImageGenerator>::msSingleton = 0;

#define PI (3.141592653589793f)

namespace Map
{

	using namespace std;
	using namespace sf;


struct PositionMapiD
{

};

// Funckje pomocnicze
struct PositionAlphaStruct
{
	PositionAlphaStruct() { }

	sf::Vector2f Position;
	float Alpha;
	std::string MapID;
} ;

int det(Vector2f p1, Vector2f p2, Vector2f p3)
{
	return (int)(p1.x*p2.y + p2.x*p3.y + p3.x*p1.y - p3.x*p2.y - p1.x*p3.y - p2.x*p1.y);
}

// Nastêpny punkt jest w lewo czy w prawo?
int turning_direction(stack<CWorldGraphMapEx,vector<CWorldGraphMapEx> >&  S, Vector2f point3, Vector2f Center)
{
	Vector2f p1;
	Vector2f p2;
	Vector2f p3 = Vector2f(point3.y, point3.x) - Center;
	
	

	//Pobieramy ze szczytu stosu punkt p2 oraz punkt p1, bedacy tu¿ pod szczytem
	CWorldGraphMapEx temp = S.top();
	p2= Vector2f(temp.mapPos.y,  temp.mapPos.x) - Center;
	S.pop();
	p1= Vector2f(S.top().mapPos.y, S.top().mapPos.x) - Center;
	S.push(temp);
	 
	//Punkt p3 le¿y po lewej stronie wektora p1->p2
	int Det = det(p1,p2,p3);
	if (Det > 0)
		return 0;
	if (Det < 0) // Po prawej
		return 1;
}

bool PositionAlphaStructComparator (PositionAlphaStruct* i,PositionAlphaStruct* j) 
{ 
	if(i->Alpha < j->Alpha)
	{
		return true;
	}
	else if(i->Alpha == j->Alpha)
	{
		return (i->Position.x < j->Position.x); 
	}
	return false;
}



// Modyfikowany CWorldGraph
void CWorldGraphEx::BuildFromWorldGraph()
{
	CWorldGraph OldGraph = gMapManager.GetWorldGraph();

	startingMap = OldGraph.startingMap;
	startingRegion = OldGraph.startingRegion;
	
	// Insert maps
	map<string, CWorldGraphMap>::iterator i = OldGraph.maps.begin();
	for(; i != OldGraph.maps.end(); ++i)
	{
		CWorldGraphMapEx NewMap;
		NewMap.id = i->second.id;

		NewMap.boss = i->second.boss;
		NewMap.final = i->second.final;
		NewMap.mapPos = i->second.mapPos;
		NewMap.scheme = i->second.scheme;


		maps[NewMap.id] = NewMap;
		schemes[NewMap.scheme].push_back(NewMap.id);
	}

	// Posortuj mapy dla algorytmu generowania otoczki
	map<string, vector<std::string>>::iterator j = schemes.begin();
	for(; j != schemes.end(); j++)
	{
		std::vector<std::string>& Maps = j->second;
		if(Maps.size() <= 3) continue; // Skip triangles

		std::vector<PositionAlphaStruct*> DataStructs;



		PositionAlphaStruct* LowestPoint = NULL;

		for(unsigned int i = 0; i < Maps.size(); i++)
		{
			PositionAlphaStruct* Data = new PositionAlphaStruct();

			Data->MapID = Maps[i];

			CWorldGraphMapEx& CurrentMap = maps[Maps[i]];

			Data->Position = Vector2f(CurrentMap.mapPos.y, CurrentMap.mapPos.x);

				DataStructs.push_back(Data);

			if((LowestPoint == NULL) || (LowestPoint->Position.y > Data->Position.y))
			{
				LowestPoint = Data;
			}
			else if((LowestPoint->Position.y == Data->Position.y) && (LowestPoint->Position.x > Data->Position.x))
			{
				LowestPoint = Data;
			}
		}

		sf::Vector2f CenterVector = LowestPoint->Position;

		for(unsigned int i = 0; i < DataStructs.size(); i++)
		{
			PositionAlphaStruct* Data = DataStructs[i];
			Data->Position = Data->Position - CenterVector;
			Vector2f& P = Data->Position;
			float D = abs(P.x) + abs(P.y);

			if ((P.x >= 0) && ( P.y >= 0))
				Data->Alpha = P.y/D;
			else	 
			if ((P.x < 0) && (P.y >= 0))
				Data->Alpha = 2 - P.y/D;
			else 
			if ((P.x < 0) && (P.y < 0))
				Data->Alpha = 2 + abs(P.y)/D;
			else
			if ((P.x >= 0) && (P.y < 0))
				Data->Alpha = 4 - abs(P.y)/D;

		}
		
		LowestPoint->Alpha = 0;


		std::sort(DataStructs.begin(), DataStructs.end(), PositionAlphaStructComparator);
		Maps.clear();
		for(unsigned int i = 0; i < DataStructs.size(); i++)
		{
			Maps.push_back(DataStructs[i]->MapID);
			delete DataStructs[i];
		}
	}

	// Oblicz otoczki wypuk³e dla danych map
	j = schemes.begin();
	for(; j != schemes.end(); j++)
	{
		std::vector<std::string>& Maps = j->second;
		if(Maps.size() > 4)
		{
			std::vector<std::string>& SchemeConvexHulls = schemeConvexHulls[j ->first];
			// Umieszczamy trzy pierwsze punkty na stosie
			
			std::stack<CWorldGraphMapEx,vector<CWorldGraphMapEx> > S;
			CWorldGraphMapEx& Start = maps[Maps[0]];
			S.push(Start);
			S.push(maps[Maps[1]]);
			S.push(maps[Maps[2]]);
			
			Vector2f Center = Vector2f(Start.mapPos.y, Start.mapPos.x);

			for (unsigned int i = 3; i < Maps.size(); i++)
			{
				CWorldGraphMapEx& CurrentMap = maps[Maps[i]];
				while (turning_direction(S, CurrentMap.mapPos, Center)==1)
					S.pop();
				S.push(CurrentMap);
			}

			while (!(S.empty()))
			{
				SchemeConvexHulls.push_back(S.top().id);
				S.pop();
			}
		}
		else
		{
			int J = 1;
		}
	}

	// Insert exits
	i = OldGraph.maps.begin();
	for(; i != OldGraph.maps.end(); ++i)
	{
		CWorldGraphMapEx &CurrentMap = maps[i->second.id];
		for(unsigned int j = 0; j < i->second.exits.size(); j++)
		{
			CWorldGraphExitEx* NewExit = new CWorldGraphExitEx();
			NewExit->PathCreated = false;
			NewExit->toEntry = i->second.exits[j].toEntry;
			NewExit->blocked = i->second.exits[j].blocked;

			NewExit->fromBorder = None;
			NewExit->toBorder = None;

			if(i->second.exits[j].onBorder == "west")
			{
				NewExit->fromBorder = West;
				NewExit->toBorder = East;
			} else
			if(i->second.exits[j].onBorder == "east")
			{
				NewExit->fromBorder = East;
				NewExit->toBorder = West;
			} else
			if(i->second.exits[j].onBorder == "north")
			{
				NewExit->fromBorder = North;
				NewExit->toBorder = South;
			} else
			if(i->second.exits[j].onBorder == "south")
			{
				NewExit->fromBorder = South;
				NewExit->toBorder = North;
			}

			NewExit->FirstParent = &CurrentMap;

			CWorldGraphMapEx &TargetMap = maps[i->second.exits[j].toMap];
			NewExit->SecondParent = &TargetMap;
			
			bool AlreadyContainsThisExit = false;
			for(unsigned int k = 0; k < TargetMap.exits.size(); k++)
			{
				if(TargetMap.exits[k]->FirstParent != &CurrentMap && TargetMap.exits[k]->SecondParent != &CurrentMap)
				{

				}
				else
				{
					AlreadyContainsThisExit = true;
				}
			}

			if(!AlreadyContainsThisExit)
			{
				CurrentMap.exits.push_back(NewExit);
				TargetMap.exits.push_back(NewExit);
			}
		}
	}
}

bool CWorldGraphMapEx::IsCrossroad()
{
	if(exits.size() > 2) return true;
	else return false;
}

bool CWorldGraphMapEx::PathsCreated()
{
	for (unsigned int i = 0; i < exits.size(); i++)
	{
		if(!exits[i]->PathCreated) return false;
	}

	return true;
}



CWorldGraphMapEx::~CWorldGraphMapEx()
{
	// Zwolniæ wyjœcia
	// TODO
}



// Map image generator
CMapImageGenerator::CMapImageGenerator(void)
{
	// Wczytaj zasoby	
	WorldMapMarkImg.LoadFromFile("data/maps/world-map-mark.png");
	WorldMapMark = Sprite(WorldMapMarkImg);
	WorldMapMark.SetCenter(WorldMapMarkImg.GetWidth()/2.0f, WorldMapMarkImg.GetHeight()/2.0f);
	RoadMarkImg.LoadFromFile("data/maps/road-mark.png");
	RoadMark = Sprite(RoadMarkImg);
	RoadMark.SetCenter(RoadMarkImg.GetWidth()/2.0f, RoadMarkImg.GetHeight()/2.0f);
	RoadMark.SetColor(Color::Red);
	BackgroundImg.LoadFromFile("data/maps/map-background.png");
	Background = Sprite(BackgroundImg);
}

CMapImageGenerator::~CMapImageGenerator(void)
{
}

//Quad tree


// Funkcje pomocnicze
float DistanceSquared(Vector2f& Point1, Vector2f& Point2)
{
	return  (Point1.x - Point2.x)*(Point1.x - Point2.x) + (Point1.y - Point2.y)*(Point1.y - Point2.y);
}



float RandomPercent(int Min, int Max)
{
	return (((rand() % Max-Min) + Min )/100.0f);
}

// Generowanie œcie¿ek miêdzy lokacjami
float CMapImageGenerator::BezierPolynomial(float t, float& Start, float& CP1, float& CP2, float& End)
{
     return (float)((1 - t) * (1 - t) * (1 - t) * Start + 3 * (1 - t) * (1 - t) * t * CP1 + 3 * (1 - t) * t * t * CP2 + t * t * t * End);  
}


Vector2f CMapImageGenerator::GetBezierPoint(float t, Vector2f& Start, Vector2f& Point1, Vector2f& Point2, Vector2f& End)
{
	return Vector2f(BezierPolynomial(t, Start.x, Point1.x, Point2.x, End.x), BezierPolynomial(t, Start.y, Point1.y, Point2.y, End.y)); 
}

Vector2f CMapImageGenerator::GetBezierPoint(int numberOfSegments, int segmentIndex, Vector2f& Start, Vector2f& Point1, Vector2f& Point2, Vector2f& End)
{
	return GetBezierPoint((segmentIndex * 1.0f ) / (numberOfSegments + 1), Start, Point1, Point2, End);
}


void CMapImageGenerator::GeneratePathBetweenTwoExits(Vector2f& Location1, Vector2f& Location2, CWorldGraphExitEx &Exit)
{
	if(Exit.FirstParent->id == "f01")
	{
		int i = 4;
	}


	float Distance = sqrt(DistanceSquared(Location1, Location2));
	float HalfDistance = Distance/2.0f;
	int NofSegments = (int)(Distance/15.0f);
	Vector2f Point1, Point2;


	// ZnajdŸ punkty na prostej ³¹cz¹cej pocz¹tek i koniec - na bok od nich bêd¹ punkty kontrolne
	Vector2f StartToEndVector = Location2 - Location1; // Location1, location 2 delta

	float 
		temp = RandomPercent(50, 75) * Distance, temp2 = RandomPercent(0, 5) * Distance;


	short int Direction1 = rand() % 2;
	bool ContinueRoad = false;
	CWorldGraphExitEx OldExit;

	CWorldGraphMapEx Start = *Exit.FirstParent;
	CWorldGraphMapEx Destination = *Exit.SecondParent;

	if(!Start.IsCrossroad())
	{
		for(unsigned int i = 0; i < Start.exits.size(); i++)
		{
			if(Start.exits[i]->PathCreated)
			{
				OldExit = *Start.exits[i];
				ContinueRoad = true;
				break;
			}
		}
	}
	if(ContinueRoad)// Mapa jest "przejœciowa" - krzywa musi byæ ci¹g³a
	{
		Vector2f OldVector;
		if(OldExit.FirstParent == Exit.FirstParent) // Punkt 1 jest bli¿ej location1
		{
			OldVector = OldExit.Point1 - Location1;
		}
		else // Punkt 2 jest bli¿ej location2
		{
			OldVector = OldExit.Point2 - Location1;
		}	
		float Rand = RandomPercent(25, 75);

		// Jeœli punkt wygenerowa³ siê blisko koñca daj mu 15 d³ugoœci
		float OldLengthSquared = DistanceSquared(OldVector, Vector2f(0, 0));
		float QuaterLength = HalfDistance/2;
		if(OldLengthSquared < QuaterLength * QuaterLength)
		{
			float OldLength = sqrt(OldLengthSquared);

			OldVector = Vector2f((OldVector.x * Distance) / OldLength, (OldVector.y * Distance) / OldLength);
		}
		else
			OldVector = Vector2f(OldVector.x * Rand, OldVector.y * Rand);

		Point1 = Location1 - OldVector;
	}
	else
	{
		Vector2f DirectionVector, PerpendicularVector;

		switch(Exit.fromBorder)
		{
		case East:
			DirectionVector = Vector2f(1, 0);
			break;
		case West:
			DirectionVector = Vector2f(-1, 0);
			break;
		case North:
			DirectionVector = Vector2f(0, -1);
			break;
		case South: 
			DirectionVector = Vector2f(0, 1);
			break;

		}

		if(rand() % 2 == 0)
			PerpendicularVector = Vector2f(DirectionVector.y, DirectionVector.x);
		else
			PerpendicularVector = Vector2f(-DirectionVector.y, -DirectionVector.x);

		Point1 = Location1 + Vector2f(DirectionVector.x * temp, DirectionVector.y * temp) + Vector2f(PerpendicularVector.x * temp2, PerpendicularVector.y * temp2);
	}


	temp = RandomPercent(50, 75) * Distance, temp2 = RandomPercent(0, 5) * Distance;

	Vector2f DirectionVector, PerpendicularVector;
	switch(Exit.toBorder)
	{
	case East:
		DirectionVector = Vector2f(1, 0);
		break;
	case West:
		DirectionVector = Vector2f(-1, 0);
		break;
	case North:
		DirectionVector = Vector2f(0, -1);
		break;
	case South: 
		DirectionVector = Vector2f(0, 1);
		break;
	}
	if(rand() % 2 == 0)
		PerpendicularVector = Vector2f(DirectionVector.y, DirectionVector.x);
	else
		PerpendicularVector = Vector2f(-DirectionVector.y, -DirectionVector.x);
	Point2 = Location2 + Vector2f(DirectionVector.x * temp, DirectionVector.y * temp) + Vector2f(PerpendicularVector.x * temp2, PerpendicularVector.y * temp2);

	Exit.Point1 = Point1;
	Exit.Point2 = Point2;

	Vector2f OldPosition = GetBezierPoint(NofSegments, 0, Location1, Point1, Point2, Location2);
	Vector2f NewPosition;;
	Vector2f CurrentPosition = GetBezierPoint(NofSegments, 1, Location1, Point1, Point2, Location2);

	int NofImages = 8;
	for(int i = 2; i <= NofSegments + 1; i++)
	{
		NewPosition = GetBezierPoint(NofSegments, i, Location1, Point1, Point2, Location2);

		// SprawdŸ dystans, aby nie malowaæ po mapy
		if(i <= 5)
		{
			if(DistanceSquared(CurrentPosition, Location1) < 150) goto calculate_positions; // Pomiñ rysowanie i przeskocz na koniec funkcji
		}
		else if(NofSegments - i < 3)
		{
			if(DistanceSquared(CurrentPosition, Location2) < 150) goto calculate_positions; // Pomiñ rysowanie i przeskocz na koniec funkcji
		}

		// Oblicz nachylenie wektora do OX		
		RoadMark.SetRotation(-(atan2(NewPosition.y - OldPosition.y, NewPosition.x - OldPosition.x) * 180/ PI) + 90);

		RoadMark.SetPosition(CurrentPosition);
		RenderWindow->Draw(RoadMark);

		// Skaczemy tu, jeœli któraœ z kropek jest za blisko obazka lokacji i nie trzeba rysowaæ
calculate_positions:

		CurrentPosition = NewPosition;
		OldPosition = CurrentPosition;;
	}

	Exit.PathCreated = true;
}

void CMapImageGenerator::GeneratePaths(CWorldGraphExitEx &Exit)
{		
	GeneratePathBetweenTwoExits(Vector2f(Exit.FirstParent->mapPos.x * Proportion.x, Exit.FirstParent->mapPos.y * Proportion.y), Vector2f(Exit.SecondParent->mapPos.x * Proportion.x, Exit.SecondParent->mapPos.y * Proportion.y), Exit);
	Exit.PathCreated = true;
}


short int CoordsToX(short int Width, short int I)
{
	return I&Width;
}
short int CoordsToY(short int Width, short int I)
{
	return I / Width;
}

short int CoordsToIndex(short int& Width, short int& X, short int& Y)
{
	return (Y * Width) + X;
}


void CMapImageGenerator::GenerateMapImage()
{  
	srand ( (unsigned int)time(NULL) );

	Image ResultImage(1024, 768);
	RenderWindow = new sf::RenderWindow(VideoMode(1024,768), "NONE", Style::None);

	// Utówrz RenderWindow do którego wyrendrujemy mapê
	RenderWindow->Show(false);
	RenderWindow->Clear(Color(162,126, 71));

	



	CWorldGraphEx WorldGraph;
	WorldGraph.BuildFromWorldGraph();

	// Narysuj t³o
	RenderWindow->Draw(Background);

	// Oblicz najwy¿sz¹ i najni¿sz¹ lokacjê, wycentruj mapê
	int TopMapBound = 1024;
	int BottomMapBound = 0;
	int LeftMapBound = 768;
	int RightMapBound = 0;
	
	std::map<std::string, CWorldGraphMapEx>::iterator i = WorldGraph.maps.begin();
	for(; i != WorldGraph.maps.end(); ++i)
	{
		Vector2f MapPosition = i->second.mapPos;

		if(i->second.mapPos.x < (float)TopMapBound) TopMapBound = (int)i->second.mapPos.x;
		if(i->second.mapPos.x > (float)BottomMapBound) BottomMapBound = (int)i->second.mapPos.x;

		if(i->second.mapPos.y > (float)RightMapBound) RightMapBound = (int)i->second.mapPos.y;
		if(i->second.mapPos.y < (float)LeftMapBound) LeftMapBound = (int)i->second.mapPos.y;
	}
	
	// Dodaj marginesy
	const int Margin = 10;

	TopMapBound -= Margin;
	BottomMapBound += Margin;
	LeftMapBound -= Margin;
	RightMapBound += Margin;

	 Proportion = Vector2f((1024.0f/BottomMapBound), (768.0f/RightMapBound));

	//// Mapa kolizji
	//short int CollisionMapWidth = (int)(ImageWidth/10.0f);
	//short int CollisionMapHeight = (int)(ImageHeight/10.0f);

	//short int* Data = new short int[CollisionMapWidth * CollisionMapHeight];
	//
	//
	//short int NumberOfSchemes = 0;
	//sf::Color* SchemeColors;
	//string* SchemeIDs;

	//// Narysuj otoczki obszarów
	//{
	//	sf::RenderWindow ConvexRender(VideoMode(1024, 768), "NONE", Style::None);
	//	ConvexRender.Show(false);
	//	RenderWindow->Clear(Color(0,0,0));

	//	NumberOfSchemes = (short int)WorldGraph.schemeConvexHulls.size();
	//	SchemeColors = new sf::Color[NumberOfSchemes];
	//	SchemeIDs = new string[NumberOfSchemes];
	//		
	//	sf::Image PrintingResult;
	//	short int Index = 0;
	//	map<string, vector<string>>::iterator j = WorldGraph.schemeConvexHulls.begin();
	//	for(; j != WorldGraph.schemeConvexHulls.end(); j++)
	//	{
	//		std::vector<string>& Maps = j->second;

	//		Shape ConvexHull;
	//		for(unsigned int i = 0; i < Maps.size(); i++)
	//		{
	//			CWorldGraphMapEx& CurrentMap = WorldGraph.maps[Maps[i]];

	//			ConvexHull.AddPoint(CurrentMap.mapPos.x * Proportion.x, CurrentMap.mapPos.y * Proportion.y);
	//		}
	//		sf::Color CurrentColor = sf::Color(Index, Index, Index);
	//		SchemeColors = CurrentColor
	//		SchemeIDs = j->first;

	//		ConvexHull.SetColor(CurrentColor);
	//		ConvexRender.Draw(ConvexHull);
	//		PrintingResult = ConvexRender.Capture();
	//		Index++;
	//	}

	//	// Pobierz kolory do maski kolizji
	//	sf::Color Result
	//	for(short int y = 0; y < CollisionMapHeight; y ++)
	//	{
	//		for(short int x = 0; x < CollisionMapWidth; x ++)
	//		{
	//			Result = PrintingResult.GetPixel(x, y);
	//			if(Result.r == 0 && Result.g == 0 && Result.b == 0) // Pusto
	//				Data[(y * CollisionMapWidth) + x] = -1;
	//			else
	//			{
	//				int IndexS = 0;
	//				for(IndexS = 0; IndexS < NumberOfSchemes; IndexS++)
	//				{						
	//					if(Result.r == SchemeColors[IndexS].r && Result.g == SchemeColors[IndexS].g && Result.b == SchemeColors[IndexS].b)
	//						break;
	//				}
	//				Data[(y * CollisionMapWidth) + x] = IndexS;
	//			}
	//		}
	//	}
	//}

	// Rysuj œcie¿ki
	i = WorldGraph.maps.begin();
	for(; i != WorldGraph.maps.end(); ++i)
	{
		WorldMapMark.SetPosition(i->second.mapPos.x * Proportion.x, i->second.mapPos.y * Proportion.y);
		RenderWindow ->Draw(WorldMapMark);

		for(unsigned int e = 0; e < i->second.exits.size(); e++)
		{
			if(!i->second.exits[e]->PathCreated)
			{
				GeneratePaths(*i->second.exits[e]);
			}
		}
	}

	// Zapisz
	ResultImage = RenderWindow->Capture();
	
	RenderWindow->Close();
    delete RenderWindow;
    RenderWindow = NULL;

    // dex: przy zamykaniu jakiegokolwiek RenderWindow SFML radosnie robi
    // sobie wglMakeCurrent(NULL, NULL), wiec zeby sie GL nie zesral to
    // trzeba wlaczyc jakikolwiek kontekst, np. tak:
    gGame.GetRenderWindow()->SetActive(true);

	ResultImage.SaveToFile("data/maps/world-map.png");
}
}
